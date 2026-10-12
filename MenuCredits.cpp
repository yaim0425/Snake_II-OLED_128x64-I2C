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
    _blink(),
    _yUp(0),
    _yMiddle(0),
    _blockStart(0),
    _blockH(0),
    _phase(Phase::CARD),
    _blockCount(0),
    _block(0),
    _scroller(),
    _pause(),
    _paused(false)
{}

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

  advance();  // compone el bloque 0 y arranca su slide

  // Música: la introducción saluda, el bucle de fondo acompaña el roll
  sound.stop();
  sound.play(fromGame && winner ? Sound::SFX_PIRATES : Sound::SFX_DRAGON_BALL, true);
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

  if (_scroller.done()) {
    if (!_paused) {
      _pause.start();  // el bloque terminó de entrar: ancla la pausa
      _paused = true;
    }
    if (!_pause.expired(PAUSE_MS)) return;

    _block++;
    if (_block >= _blockCount) {
      _phase = Phase::CARD;  // el último bloque ya se mostró: tarjeta final
      return;
    }
    advance();
  }

  // El slide del bloque entra en curso: avanza y devuelve (print()
  // repinta el bloque en el mismo frame)
  _scroller.update();
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

  // Header fijo: el título 12x16 centrado
  char* title;
  if (!_fromGame) title = TITLE_CREDITS;
  else if (_winner) title = TITLE_WINNER;
  else title = TITLE_GAME_OVER;

  display.drawText(title,
    display.centerX(title, TEXT_12x16), HEADER_TOP + 1,
    TEXT_12x16, true, false);

  _blink.start();
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

// ====================================================================================
// Avance al bloque actual: compone sus líneas en el ScrollerV y arranca el slide
// ====================================================================================

void MenuCredits::advance() {
  _blockStart = _blocks[_block].start;
  _blockH = _blocks[_block].count * LINE_H;
  _paused = false;

  // Compone las filas del bloque (máx. MAX_BLOCKS, defensa contra overrun)
  const char* lines[MAX_BLOCKS];
  uint8_t n = 0;
  for (uint8_t i = 0; i < (uint8_t)_blocks[_block].count && n < MAX_BLOCKS; i++)
    lines[n++] = CREDITS[_blockStart + i];

  // Sube desde abajo; la primera fila es barra blanca (título) salvo en el bloque 0
  _scroller.setBloque(lines, n, BODY_TOP, BODY_H, true, 1, _block > 0);
  _scroller.startSlide();
}

// ====================================================================================
// Dibujo del roll
// ====================================================================================

void MenuCredits::drawRoll() {
  _scroller.print();
}

// ====================================================================================
// Dibujo de la tarjeta final
// ====================================================================================

void MenuCredits::drawCard() {
  // Borra la banda del roll (Body); el Header permanece visible
  display.fillRect(0, BODY_TOP, WIDTH, BODY_H, false);

  if (_fromGame) {
    // Desde Game: el mensaje de agradecimiento centrado en el Body
    int16_t y = BODY_TOP + (BODY_H - CHAR_H) / 2;
    display.drawText(CARD_THANKS, display.centerX(CARD_THANKS, TEXT_6x8), y, TEXT_6x8, true, false);
  } else {
    // Desde el menú: NAME / VERSION / RELEASE_DATE centradas (3 filas)
    const char* lines[3] = { CARD_NAME, CARD_VERSION, CARD_DATE };

    int16_t top = BODY_TOP + (BODY_H - 3 * LINE_H) / 2;
    for (uint8_t i = 0; i < 3; i++) {
      int16_t y = top + (int16_t)(i * LINE_H);
      display.drawText(lines[i], display.centerX(lines[i], TEXT_6x8), y, TEXT_6x8, true, false);
    }
  }
}

// ====================================================================================
// Fin
// ====================================================================================