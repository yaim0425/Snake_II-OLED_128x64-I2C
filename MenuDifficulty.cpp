#include "MenuDifficulty.h"
#include "Globals.h"
#include "Draw.h"
#include <stdio.h>

MenuDifficulty::MenuDifficulty()
  : _selected(DEFAULT_LEVEL),
    _lastSelected(DEFAULT_LEVEL),
    _done(false),
    _blink(),
    _clear(true),
    _repeat() {}

void MenuDifficulty::begin() {
  _clear = true;
  _done = false;
  _selected = storage.difficulty();
  _lastSelected = _selected;
  _blink.start();
}

void MenuDifficulty::update() {
  if (_repeat.step(Buttons::MOVE_RIGHT) && _selected < MAX_LEVEL) {
    _selected++;
    sound.play(Sound::SFX_CLICK, false);
  }

  if (_repeat.step(Buttons::MOVE_LEFT) && _selected > MIN_LEVEL) {
    _selected--;
    sound.play(Sound::SFX_CLICK, false);
  }

  if (buttons.pressed(Buttons::ACTION_RIGHT)) {
    storage.setDifficulty(_selected);
    sound.play(Sound::SFX_CONFIRM, false);
    _done = true;
  } else if (buttons.pressed(Buttons::ACTION_UP)) {
    sound.play(Sound::SFX_BACK, false);
    _done = true;
  }
}

void MenuDifficulty::print() {
  firstPrint(false);
  nextOption(false);
  blinkOption();
}

void MenuDifficulty::clear() {
  display.fillRect(0, VALUE_TOP, WIDTH, VALUE_HEIGHT, false);
}

void MenuDifficulty::nextOption(bool show) {
  if (_lastSelected == _selected && !show) return;

  char buf[8];

  if (_selected < 10)
    snprintf(buf, sizeof(buf), " %u", (unsigned)_selected);
  else
    snprintf(buf, sizeof(buf), "%u", (unsigned)_selected);
  display.drawText(buf, LABEL_X, DIAMOND_Y - 3, TEXT_6x8, true, false);
  const bool leftHeld = buttons.heldDown(Buttons::MOVE_LEFT) && _selected > MIN_LEVEL;
  const bool rightHeld = buttons.heldDown(Buttons::MOVE_RIGHT) && _selected < MAX_LEVEL;
  Draw::triangle(Draw::DIR_LEFT, TRIANGLE_L, DIAMOND_Y, _selected > MIN_LEVEL && !rightHeld);
  Draw::triangle(Draw::DIR_RIGHT, TRIANGLE_R, DIAMOND_Y, _selected < MAX_LEVEL && !leftHeld);
  _lastSelected = _selected;
  _blink.start();
}

void MenuDifficulty::blinkOption() {
  if (!_blink.changed(PERIOD, OFF)) return;

  const bool visible = _blink.isVisible(PERIOD, OFF);
  const bool leftHeld = buttons.heldDown(Buttons::MOVE_LEFT) && _selected > MIN_LEVEL;
  const bool rightHeld = buttons.heldDown(Buttons::MOVE_RIGHT) && _selected < MAX_LEVEL;

  if (_selected > MIN_LEVEL && !rightHeld) Draw::triangle(Draw::DIR_LEFT, TRIANGLE_L, DIAMOND_Y, leftHeld || visible);

  if (_selected < MAX_LEVEL && !leftHeld) Draw::triangle(Draw::DIR_RIGHT, TRIANGLE_R, DIAMOND_Y, rightHeld || visible);
}

void MenuDifficulty::firstPrint(bool forceRedraw) {
  if (!_clear && !forceRedraw) return;

  clear();
  _clear = false;
  nextOption(true);
}

bool MenuDifficulty::done() const {
  return _done;
}