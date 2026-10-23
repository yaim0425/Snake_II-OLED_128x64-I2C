#include "MenuScale.h"
#include "src/Globals.h"
#include "src/graphics/Draw.h"
#include <stdio.h>
#include <string.h>

// ========================================================
// Funciones comunes de las ventanas
// ========================================================

MenuScale::MenuScale()
  : _done(false),
    _selected(0),
    _lastSelected(0),
    
    _blink(),
    _clear(true),
    _changed(false),
    _repeat() {

  for (uint8_t i = 0; i < COUNT; i++) {
    _startX[i] = 0;
    _endX[i] = 0;
    _label[i][0] = '\0';
  }
}

void MenuScale::begin() {
  _clear = true;
  _done = false;
  _changed = false;

  for (uint8_t i = 0; i < COUNT; i++) {
    const uint8_t cell = cellAt(i);
    snprintf(_label[i], LABEL_LEN, "%u", (unsigned)cell);
    const int16_t w = strlen(_label[i]) * CHAR_W * TEXT_6x8;
    _startX[i] = ANCHOR_X[i] - w / 2;
    _endX[i] = _startX[i] + w;
  }

  _selected = 0;
  const uint8_t current = storage.scale();
  for (uint8_t i = 0; i < COUNT; i++)
    if (cellAt(i) == current) _selected = i;

  _lastSelected = _selected;
  _blink.start();
}

void MenuScale::update() {
  if (_repeat.step(Buttons::MOVE_RIGHT) && _selected < COUNT - 1) {
    _selected++;
    sound.play(Sound::SFX_CLICK, false);
  }

  if (_repeat.step(Buttons::MOVE_LEFT) && _selected > 0) {
    _selected--;
    sound.play(Sound::SFX_CLICK, false);
  }

  if (buttons.pressed(Buttons::ACTION_RIGHT)) {
    const uint8_t cell = cellAt(_selected);
    if (cell != storage.scale()) _changed = true;
    storage.setScale(cell);
    sound.play(Sound::SFX_CONFIRM, false);
    _done = true;
  } else if (buttons.pressed(Buttons::ACTION_UP)) {
    sound.play(Sound::SFX_BACK, false);
    _done = true;
  }
}

void MenuScale::print() {
  firstPrint();
  nextOption(false);
  blinkOption();
}

void MenuScale::firstPrint() {
  if (!_clear) return;

  clear();
  _clear = false;
  for (uint8_t i = 0; i < COUNT; i++)
    display.drawText(_label[i], _startX[i], DIAMOND_Y - 3, TEXT_6x8, true, false);
  nextOption(true);
}

void MenuScale::clear() {
  display.fillRect(0, VALUE_TOP, WIDTH, VALUE_HEIGHT, false);
}

bool MenuScale::done() const {
  return _done;
}

bool MenuScale::changed() const {
  return _changed;
}

// ========================================================
// Fin — Funciones comunes de las ventanas
// ========================================================



// ========================================================
// Funciones locales
// ========================================================

void MenuScale::blinkOption() {
  if (!_blink.changed(PERIOD, OFF)) return;

  toggleArrows(_selected, _blink.isVisible(PERIOD, OFF));
}

void MenuScale::nextOption(bool show) {
  if (_lastSelected == _selected && !show) return;

  toggleArrows(_lastSelected, false);
  toggleArrows(_selected, true);
  _lastSelected = _selected;
  _blink.start();
}

void MenuScale::toggleArrows(uint8_t option, bool show) {
  Draw::triangle(Draw::DIR_RIGHT, _startX[option] - (SIZE + 3), DIAMOND_Y, show);
  Draw::triangle(Draw::DIR_LEFT, _endX[option] + (SIZE + 1), DIAMOND_Y, show);
}

uint8_t MenuScale::cellAt(uint8_t option) {
  return CELL_MIN + option * CELL_STEP;
}

// ========================================================
// Fin — Funciones locales
// ========================================================
