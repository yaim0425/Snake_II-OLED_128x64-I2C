#include "esp32-hal.h"
#include <string.h>

#include "MenuCredits.h"

#include "Globals.h"

// ====================================================================================
// MenuCredits — roll vertical de créditos (ver el .h)
// ====================================================================================

// ====================================================================================
// Constructor: sin acciones manuales (comienza apagado: _done = true)
// ====================================================================================

MenuCredits::MenuCredits()
  : _fromGame(false),
    _winner(false),
    _clear(true),
    _done(true),
    _phase(Phase::CARD),
    _blockCount(0),
    _block(0),
    _py(START_PY),
    _from(0),
    _to(0),
    _moveMs(0),
    _anim(),
    _pause(),
    _moving(false)
{
}

// ====================================================================================
// Inicialización: arranca el roll y la música de fondo en bucle
// ====================================================================================

void MenuCredits::begin(bool fromGame, bool winner) {
  _fromGame = fromGame;
  _winner = winner;

  buttons.ignoreUntilRelease();

  // Primer frame (para llegar): clear() completo + header
  _clear = true;

  _done = false;
  _phase = Phase::ROLL;

  computeBlocks();
  _block = 0;
  _py = START_PY;            // el roll entra desde abajo del carril

  startMoveTo(0);            // primer movimiento hacia el centro del bloque 0

  // Música: la introducción saluda, el bucle de fondo acompaña el roll
  sound.stop();
  sound.play(fromGame && winner ? Sound::SFX_PIRATES : Sound::SFX_CANON, true);
}

// ====================================================================================
// tick: salida con ACTION_UP, y avance del roll (mover / pausar / siguiente)
// ====================================================================================

void MenuCredits::update() {
  if (_done) return;

  // Salida: corta la música de fondo, suena el rechazo y se vuelve al menú
  if (buttons.pressed(Buttons::ACTION_UP)) {
    sound.stop();
    sound.play(Sound::SFX_BACK);
    _done = true;
    return;
  }

  // La tarjeta final es estática: no hay animación que actualizar
  if (_phase != Phase::ROLL) return;

  if (_moving) {
    // Avanzando hacia el centro del bloque
    uint32_t elapsed = (uint32_t)_anim.elapsed();
    if (elapsed >= _moveMs) {
      _py = _to;                      // llegó: se posa en el centro exacto
      _moving = false;
      _pause.start();                 // y comienza la pausa con el bloque centrado
    } else {
      // Interpolación lineal: px por ms (frame-rate independiente).
      // dist <= 299 y elapsed <= ~12 s -> dist*elapsed <= ~3,6e6 (cabe en uint32).
      uint16_t dist = _to > _from ? _to - _from : _from - _to;
      int16_t off = (int16_t)(dist * elapsed / _moveMs);
      _py = _to > _from ? _from + off : _from - off;
    }
  } else if (_pause.expired(PAUSE_MS)) {
    _block++;
    if (_block >= _blockCount) _phase = Phase::CARD;          // el último bloque ya se mostró: tarjeta final
    else startMoveTo(_block);  // a centrar el bloque siguiente
  }
}

// ====================================================================================
// Dibujar
// ====================================================================================

void MenuCredits::print() {
  firstPrint();
  if (_phase == Phase::ROLL) drawRoll();
  else drawCard();
}

void MenuCredits::firstPrint(bool forceRedraw) {
  if (!_clear && !forceRedraw) return;

  display.clear();
  _clear = false;

  // Header fijo: "Credits" 12x16 centrado
  display.drawText(TITLE, centerX(TITLE, CHAR_W * 2), HEADER_TOP + 1, TEXT_12x16, true, false);
}

// ====================================================================================
// Estado
// ====================================================================================

bool MenuCredits::done() const {
  return _done;
}

bool MenuCredits::rolling() const {
  return _phase == Phase::ROLL;
}

// ====================================================================================
// Bloques
// ====================================================================================

// py (de la fila 0) para que el bloque quede centrado en el Body:
//    Bloque = filas start..start+count-1; su centro en contenido = start*LINE_H + count*LINE_H/2.
//    py = BODY_CTR - centro.
int16_t MenuCredits::blockTargetY(uint8_t block) const {
  int16_t center = _blocks[block].start * LINE_H + _blocks[block].count * LINE_H / 2;
  return BODY_CTR - center;
}

// Deriva los bloques: filas no vacías consecutivas (el vacío es separador)
void MenuCredits::computeBlocks() {
  _blockCount = 0;
  for (uint8_t i = 0; i < COUNT; ) {
    const char* t = CREDITS[i];
    if (t[0] == '\0') { i++; continue; }
    uint8_t start = i;
    while (i < COUNT && CREDITS[i][0] != '\0') i++;
    if (_blockCount >= MAX_BLOCKS) break;   // defensa: ya no caben más bloques
    _blocks[_blockCount].start = start;
    _blocks[_blockCount].count = (int16_t)(i - start);
    _blockCount++;
  }
}

// Arranca el movimiento _py -> centro del bloque (a PX_MS por px)
void MenuCredits::startMoveTo(uint8_t block) {
  _from = _py;
  _to = blockTargetY(block);
  // dist <= 299 px -> también sin int32
  int16_t dist = _to > _from ? _to - _from : _from - _to;
  _moveMs = (uint32_t)((uint16_t)dist * PX_MS);
  _anim.start();
  _moving = true;
}

// ====================================================================================
// Dibujo del roll
// ====================================================================================

void MenuCredits::drawRoll() {
  // Borra el carril (Body)
  display.fillRect(0, BODY_TOP, WIDTH, BODY_H, false);

  int16_t bodyBottom = (int16_t)(BODY_TOP + BODY_H);

  for (uint8_t i = 0; i < COUNT; i++) {
    const char* text = CREDITS[i];
    if (text[0] == '\0') continue;             // separador: fila en blanco

    int16_t y = _py + (int16_t)(i * LINE_H);

    // Solo se dibuja la fila completa dentro del carril (no asoma a medias
    // por los bordes, que pintaría sobre el Header o el pie de pantalla)
    if (y < BODY_TOP || y + CHAR_H > bodyBottom) continue;

    // Título = primera fila no vacía tras un separador (la fila 0 del
    // array no lleva barra porque inicia el roll)
    bool title = (i > 0) && (CREDITS[i - 1][0] == '\0');

    if (title) {
      display.fillRect(0, y, WIDTH, LINE_H, true);   // barra blanca de ancho completo
      display.drawText(text, centerX(text), y + 1, TEXT_6x8, false, true);   // letra negra sobre la barra
    } else {
      display.drawText(text, centerX(text), y, TEXT_6x8, true, false);   // nombre normal
    }
  }
}

// ====================================================================================
// Dibujo de la tarjeta final
// ====================================================================================

void MenuCredits::drawCard() {
  // Borra el carril (Body) y apaga también el Header: el foco es la tarjeta
  display.fillRect(0, BODY_TOP, WIDTH, BODY_H, false);
  display.fillRect(0, HEADER_TOP, WIDTH, HEADER_H, false);

  if (_fromGame) {
    // Desde Game: el mensaje de agradecimiento centrado en el Body
    int16_t y = BODY_TOP + (BODY_H - CHAR_H) / 2;
    display.drawText(CARD_THANKS, centerX(CARD_THANKS), y, TEXT_6x8, true, false);
  } else {
    // Desde el menú: NAME / VERSION / RELEASE_DATE centradas (3 filas)
    const char* lines[3] = { CARD_NAME, CARD_VERSION, CARD_DATE };

    int16_t top = BODY_TOP + (BODY_H - 3 * LINE_H) / 2;
    for (uint8_t i = 0; i < 3; i++) {
      int16_t y = top + (int16_t)(i * LINE_H);
      display.drawText(lines[i], centerX(lines[i]), y, TEXT_6x8, true, false);
    }
  }
}

// ====================================================================================
// Helper de centrado horizontal
// ====================================================================================

int16_t MenuCredits::centerX(const char* line, uint16_t charW) {
  return (int16_t)((WIDTH - (int16_t)strlen(line) * (int16_t)charW) / 2);
}

// ====================================================================================
// Fin
// ====================================================================================