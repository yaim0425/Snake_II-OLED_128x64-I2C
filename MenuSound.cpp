#include "MenuSound.h"
#include "Globals.h"
#include "Draw.h"
#include <string.h>

// ========================================================
// Funciones comunes de las ventanas
// ========================================================

MenuSound::MenuSound()
  : _done(false),
    _selected(true),
    _lastSelected(true),
    _blink(),
    _clear(true),
    _onStartX(0),
    _onEndX(0),
    _offStartX(0),
    _offEndX(0) {}

void MenuSound::begin() {
  _clear = true;
  _done = false;
  _selected = storage.soundEnabled();
  _lastSelected = _selected;
  const char* text;
  int16_t width;
  text = "ON";
  width = strlen(text) * CHAR_W * TEXT_6x8;
  _onStartX = PAD_RIGHT_X - width / 2;
  _onEndX = _onStartX + width;
  text = "OFF";
  width = strlen(text) * CHAR_W * TEXT_6x8;
  _offStartX = PAD_LEFT_X - width / 2;
  _offEndX = _offStartX + width;
  _blink.start();
}

void MenuSound::update() {
  if (buttons.pressed(Buttons::MOVE_LEFT) && _selected) {
    _selected = false;
    sound.play(Sound::SFX_CLICK, false);
  }

  if (buttons.pressed(Buttons::MOVE_RIGHT) && !_selected) {
    _selected = true;
    sound.play(Sound::SFX_CLICK, false);
  }

  if (buttons.pressed(Buttons::ACTION_RIGHT)) {
    storage.setSoundEnabled(_selected);

    if (_selected) sound.play(Sound::SFX_CONFIRM, false);

    _done = true;
  } else if (buttons.pressed(Buttons::ACTION_UP)) {
    sound.play(Sound::SFX_BACK, false);
    _done = true;
  }
}

void MenuSound::print() {
  firstPrint(false);
  nextOption(false);
  blinkOption();
}

void MenuSound::firstPrint(bool forceRedraw) {
  if (!_clear && !forceRedraw) return;

  clear();
  _clear = false;
  display.drawText("ON", _onStartX, DIAMOND_Y - 3, TEXT_6x8, true, false);
  display.drawText("OFF", _offStartX, DIAMOND_Y - 3, TEXT_6x8, true, false);
  nextOption(true);
}

void MenuSound::clear() {
  display.fillRect(0, VALUE_TOP, WIDTH, VALUE_HEIGHT, false);
}

bool MenuSound::done() const {
  return _done;
}

void MenuSound::blinkOption() {
  if (!_blink.changed(PERIOD, OFF)) return;

  bool visible = _blink.isVisible(PERIOD, OFF);

  if (_selected) {
    Draw::triangle(Draw::DIR_RIGHT, _onStartX - (SIZE + 3), DIAMOND_Y, visible);
    Draw::triangle(Draw::DIR_LEFT, _onEndX + (SIZE + 1), DIAMOND_Y, visible);
  } else {
    Draw::triangle(Draw::DIR_RIGHT, _offStartX - (SIZE + 3), DIAMOND_Y, visible);
    Draw::triangle(Draw::DIR_LEFT, _offEndX + (SIZE + 0), DIAMOND_Y, visible);
  }
}

void MenuSound::nextOption(bool show) {
  if (_lastSelected == _selected && !show) return;

  Draw::triangle(Draw::DIR_RIGHT, _onStartX - (SIZE + 3), DIAMOND_Y, _selected);
  Draw::triangle(Draw::DIR_LEFT, _onEndX + (SIZE + 1), DIAMOND_Y, _selected);
  Draw::triangle(Draw::DIR_RIGHT, _offStartX - (SIZE + 3), DIAMOND_Y, !_selected);
  Draw::triangle(Draw::DIR_LEFT, _offEndX + (SIZE + 0), DIAMOND_Y, !_selected);
  _lastSelected = _selected;
  _blink.start();
}

// ========================================================
// Fin
// ========================================================
