#include "esp32-hal.h"
#include <string.h>

#include "MenuCredits.h"

#include "Globals.h"
#include "Draw.h"

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
    // _blink(),
    _stay(true),
    // _yUp(0),
    // _yMiddle(0),
    // _blockStart(0),
    // _blockH(0),
    // _rest(0),
    // _phase(Phase::CARD),
    // _blockCount(0),
    // _block(0),
    // _scroller(),
    // _pause(),
    _paused(false),
    _exiting(false) {}

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
  _stay = true;
  _line = 0;
  // _phase = Phase::ROLL;

  // computeBlocks();
  // _block = 0;

  startStay();  // compone el bloque 0 y arranca su slide

  // Música: la introducción saluda, el bucle de fondo acompaña el roll
  sound.stop();
  sound.play(fromGame && winner ? Sound::SFX_DRAGON_BALL : Sound::SFX_PIRATES, true);
}

// ====================================================================================
// tick: salida con ACTION_UP, y avance del roll (mover / pausar / siguiente)
// ====================================================================================

void MenuCredits::update() {
  if (_done) return;

  // Salida: corta la música de fondo, suena el rechazo y se vuelve al menú
  bool exit = false;

  if (buttons.anyHeld())
    sound.stop();

  if (buttons.pressed(Buttons::MOVE_UP) || buttons.pressed(Buttons::MOVE_RIGHT) || buttons.pressed(Buttons::MOVE_DOWN) || buttons.pressed(Buttons::MOVE_LEFT)) {
    sound.play(Sound::SFX_CLICK);
    exit = true;
  } else if (buttons.pressed(Buttons::ACTION_UP)) {
    sound.play(Sound::SFX_BACK);
    exit = true;
  } else if (buttons.pressed(Buttons::ACTION_RIGHT)) {
    sound.play(Sound::SFX_CONFIRM);
    exit = true;
  } else if (buttons.pressed(Buttons::ACTION_DOWN) || buttons.pressed(Buttons::ACTION_LEFT)) {
    sound.play(Sound::SFX_CLICK);
    exit = true;
  }

  if (exit) {
    _done = true;
    return;
  }

  // if (buttons.pressed(Buttons::ACTION_UP) || buttons.pressed(Buttons::ACTION_RIGHT)) {
  //   sound.stop();
  //   sound.play(Sound::SFX_BACK);
  //   _done = true;
  //   return;
  // }

  // // La tarjeta final es estática: no hay animación que actualizar
  // if (_phase != Phase::ROLL) return;

  // if (!_exiting) {
  //   // 1er vuelo (STAY): entra y reposa centrado; tras la pausa externa
  //   // se lanza el 2º vuelo de salida
  //   if (_scroller.done()) {
  //     if (!_paused) {
  //       _pause.start();  // el bloque terminó de entrar: ancla la pausa
  //       _paused = true;
  //     }
  //     if (!_pause.expired(PAUSE_MS)) return;

  //     startExit();
  //     return;
  //   }
  // } else {
  //   // 2º vuelo (EXIT): desaparece por el límite superior del Body; al
  //   // terminar pasa al bloque siguiente (o a la tarjeta final)
  //   if (_scroller.done()) {
  //     _block++;
  //     if (_block >= _blockCount) {
  //       _phase = Phase::CARD;  // el último bloque ya se mostró: tarjeta final
  //       return;
  //     }
  //     advance();
  //     return;
  //   }
  // }

  // // El slide del bloque está en curso: avanza y devuelve (print()
  // // repinta el bloque en el mismo frame)
  // _scroller.update();

  if (_isScrolling) {
    if (_scroller.update()) return;
    if (_stay) {
      _isScrolling = false;
      _pause.start();
    } else {
      _stay = true;
      startStay();
      return;
    }
  }

  if (_stay) {
    if (!_pause.expired(PAUSE_MS)) return;
    _isScrolling = true;
    _stay = false;
    startExit();
  } 
  
}

// ====================================================================================
// Dibujar
// ====================================================================================

void MenuCredits::print() {
  firstPrint();
  _scroller.print();

  // if (_phase == Phase::ROLL) drawRoll();
  // else drawCard();
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

  display.drawText(
    title,
    display.centerX(title, TEXT_12x16), HEADER_TOP + 1,
    TEXT_12x16, true, false);

  // _blink.start();
}

// ====================================================================================
// Estado
// ====================================================================================

bool MenuCredits::done() const {
  return _done;
}

// bool MenuCredits::rolling() const {
//   // return _phase == Phase::ROLL;
// }

// ====================================================================================
// Bloques
// ====================================================================================

// Deriva los bloques: filas no vacías consecutivas (el vacío es separador)
// void MenuCredits::computeBlocks() {
//   // _blockCount = 0;
//   // for (uint8_t i = 0; i < COUNT; ) {
//   //   const char* t = CREDITS[i];
//   //   if (!t || t[0] == '\0') { i++; continue; }
//   //   uint8_t start = i;
//   //   while (i < COUNT && CREDITS[i] && CREDITS[i][0] != '\0') i++;
//   //   if (_blockCount >= MAX_BLOCKS) break;   // defensa: ya no caben más bloques
//   //   _blocks[_blockCount].start = start;
//   //   _blocks[_blockCount].count = (int16_t)(i - start);
//   //   _blockCount++;
//   // }
// }

// ====================================================================================
// Avance al bloque actual: compone sus líneas en el Scroller y arranca el slide
// ====================================================================================

void MenuCredits::startStay() {
  if (_line >= COUNT) return;
  // _blockStart = _blocks[_block].start;

  // uint8_t n = (uint8_t)_blocks[_block].count;
  // if (n > MAX_BLOCKS) n = MAX_BLOCKS;
  // _blockH = (int16_t)(n * LINE_H);
  // _paused = false;
  // _exiting = false;

  // // 1er vuelo: sube desde abajo y reposa centrado (STAY)
  _scroller.begin(Draw::DIR_UP, Scroller::EndMode::STAY);

  int8_t i;
  for (i = 0; _line + i < COUNT; i++) {
    const char* text = CREDITS[_line + i];
    if (text[0] == '\0') break;
    _scroller.addText(text, TEXT_6x8, i != 0, i == 0);
  }
  _line += i + 1;
  _blockH = i * (CHAR_H + 2) * TEXT_6x8;

  // El bloque se detiene centrado en la banda del Body
  _rest = BODY_TOP + (BODY_H - _blockH) / 2;
  _scroller.setVertical(HEIGHT - 1, _rest);
  _scroller.startSlide();

  _isScrolling = true;
  _stay = true;
}

// ====================================================================================
// Salida del bloque actual: 2º vuelo desde el reposo hasta fuera de la banda
// ====================================================================================

void MenuCredits::startExit() {
  // _exiting = true;

  // Continúa desde el reposo (misma posición: sin salto visual) hasta
  // quedar completo por encima del Body (lo recorta el Scroller)
  _scroller.setDir(Scroller::EndMode::EXIT);
  // addBlockLines();
  _scroller.setVertical(_rest, BODY_TOP - _blockH);
  // _scroller.setVertical(_rest, BODY_TOP);
  _scroller.startSlide();
}

// ====================================================================================
// Líneas del bloque actual (mismos colores en los dos vuelos)
// ====================================================================================

// void MenuCredits::addBlockLines() {
//   bool first = true;
//   _blockH = 0;
//   for (_line = 0; _line < COUNT; _line++) {
//     if(CREDITS[_line] == "") return;
//     _scroller.addText(CREDITS[_line], TEXT_6x8, first, !first);
//     _blockH = CHAR_H * TEXT_6x8;
//     first = !first;
//   }

//   // uint8_t n = (uint8_t)_blocks[_block].count;
//   // if (n > MAX_BLOCKS) n = MAX_BLOCKS;

//   // // La primera fila es barra blanca (título) salvo en el bloque 0
//   // for (uint8_t i = 0; i < n; i++) {
//   //   bool bar = (_block > 0) && (i == 0);
//   //   _scroller.addText(CREDITS[_blockStart + i], TEXT_6x8, !bar, bar);
//   // }
// }

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
  // // Borra la banda del roll (Body); el Header permanece visible
  // display.fillRect(0, BODY_TOP, WIDTH, BODY_H, false);

  // if (_fromGame) {
  //   // Desde Game: el mensaje de agradecimiento centrado en el Body
  //   int16_t y = BODY_TOP + (BODY_H - CHAR_H) / 2;
  //   display.drawText(CARD_THANKS, display.centerX(CARD_THANKS, TEXT_6x8), y, TEXT_6x8, true, false);
  // } else {
  //   // Desde el menú: NAME / VERSION / RELEASE_DATE centradas (3 filas)
  //   const char* lines[3] = { CARD_NAME, CARD_VERSION, CARD_DATE };

  //   int16_t top = BODY_TOP + (BODY_H - 3 * LINE_H) / 2;
  //   for (uint8_t i = 0; i < 3; i++) {
  //     int16_t y = top + (int16_t)(i * LINE_H);
  //     display.drawText(lines[i], display.centerX(lines[i], TEXT_6x8), y, TEXT_6x8, true, false);
  //   }
  // }
}

// ====================================================================================
// Fin
// ====================================================================================