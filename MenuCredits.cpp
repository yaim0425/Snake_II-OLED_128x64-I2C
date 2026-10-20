#include "MenuCredits.h"
#include "Globals.h"
#include "Draw.h"

MenuCredits::MenuCredits()
  : _fromGame(false),
    _winner(false),
    _clear(true),
    _done(true),
    _blockH(0),
    _rest(0),
    _stay(true),
    _line(0),
    _scroller(),
    _isScrolling(false),
    _card(false),
    _pause() {}

void MenuCredits::begin(bool fromGame, bool winner) {
  _fromGame = fromGame;
  _winner = winner;
  buttons.ignoreUntilRelease();
  _clear = true;
  _done = false;
  _stay = true;
  _card = false;
  _line = 0;
  startStay();
  sound.stop();
  sound.play(fromGame && winner ? Sound::SFX_DRAGON_BALL : Sound::SFX_PIRATES, true);
}

void MenuCredits::update() {
  if (_done) return;

  bool exit = false;

  if (buttons.anyHeld()) sound.stop();

  if (buttons.pressed(Buttons::MOVE_UP) || buttons.pressed(Buttons::MOVE_RIGHT) ||
    buttons.pressed(Buttons::MOVE_DOWN) || buttons.pressed(Buttons::MOVE_LEFT)) {
    sound.play(Sound::SFX_CLICK, false);
    exit = true;
  } else if (buttons.pressed(Buttons::ACTION_UP)) {
    sound.play(Sound::SFX_BACK, false);
    exit = true;
  } else if (buttons.pressed(Buttons::ACTION_RIGHT)) {
    sound.play(Sound::SFX_CONFIRM, false);
    exit = true;
  } else if (buttons.pressed(Buttons::ACTION_DOWN) || buttons.pressed(Buttons::ACTION_LEFT)) {
    sound.play(Sound::SFX_CLICK, false);
    exit = true;
  }

  if (exit) {
    _done = true;
    return;
  }

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

  // La tarjeta final se queda centrada: no hay pausa ni salida.
  if (_card) return;

  if (_stay) {
    if (!_pause.expired(PAUSE_MS)) return;

    _isScrolling = true;
    _stay = false;
    startExit();
  }
}

void MenuCredits::print() {
  firstPrint(false);
  drawRoll();
}

void MenuCredits::firstPrint(bool forceRedraw) {
  if (!_clear && !forceRedraw) return;

  display.clear();
  _clear = false;
  const char* title;

  if (!_fromGame)
    title = TITLE_CREDITS;
  else if (_winner)
    title = TITLE_WINNER;
  else
    title = TITLE_GAME_OVER;
  display.drawText(title, display.centerX(title, TEXT_12x16), HEADER_TOP, TEXT_12x16, true, false);
}

bool MenuCredits::done() const {
  return _done;
}

bool MenuCredits::card() const {
  return _card;
}

void MenuCredits::startStay() {
  if (_line >= COUNT) {
    startCard();
    return;
  }

  _scroller.begin(Draw::DIR_UP, Scroller::EndMode::STAY);
  int8_t i;

  for (i = 0; _line + i < COUNT; i++) {
    const char* text = CREDITS[_line + i];

    if (text[0] == '\0') break;

    _scroller.addText(text, TEXT_6x8, i != 0, i == 0);
  }

  _line += i + 1;
  startBlock((uint8_t)i);
}

void MenuCredits::startExit() {
  _scroller.setEndMode(Scroller::EndMode::EXIT);
  _scroller.setVertical(_rest, BODY_TOP);
  _scroller.startSlide();
}

void MenuCredits::startCard() {
  // El roll terminó: la tarjeta final sube como un bloque y se queda centrada.
  _card = true;
  sound.stop();
  _scroller.begin(Draw::DIR_UP, Scroller::EndMode::STAY);

  if (_fromGame) {
    _scroller.addText(CARD_THANKS, TEXT_6x8, false, true);
    startBlock(1);
    return;
  }

  _scroller.addText(CARD_NAME, TEXT_6x8, false, true);
  _scroller.addText(CARD_VERSION, TEXT_6x8, true, false);
  _scroller.addText(CARD_DATE, TEXT_6x8, true, false);
  startBlock(3);
}

void MenuCredits::startBlock(uint8_t count) {
  _blockH = count * LINE_H * TEXT_6x8;
  _rest = BODY_TOP + (BODY_H - _blockH) / 2;
  _scroller.setVertical(HEIGHT - 1, _rest);
  _scroller.startSlide();
  _isScrolling = true;
  _stay = true;
}

void MenuCredits::drawRoll() {
  _scroller.print();
}