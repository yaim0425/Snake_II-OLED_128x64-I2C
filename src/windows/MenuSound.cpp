#include "MenuSound.h"
#include "src/Globals.h"
#include "src/graphics/Draw.h"
#include <string.h>

// ========================================================
// Funciones comunes de las ventanas
// ========================================================

MenuSound::MenuSound()
  : _done(false),
    _selected(true),
    _lastSelected(true),
    _blink(),
    _clear(true) {

}

void MenuSound::begin() {
  _clear = true;
  _done = false;
  _selected = storage.soundEnabled();
  _lastSelected = _selected;

  for (uint8_t i = 0; i < COUNT; i++) {
    const int16_t w = strlen(LABEL[i]) * CHAR_W * TEXT_6x8;
    _startX[i] = SPACE - w / 2;
    _endX[i] = _startX[i] + w;
  }
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
  firstPrint();
  nextOption(false);
  blinkOption();
}

void MenuSound::firstPrint() {
  if (!_clear) return;

  clear();
  _clear = false;
  for (uint8_t i = 0; i < COUNT; i++)
    display.drawText(LABEL[i], _startX[i], DIAMOND_Y - 3, TEXT_6x8, true, false);
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

  toggleArrows(_selected, _blink.isVisible(PERIOD, OFF));
}

void MenuSound::nextOption(bool show) {
  if (_lastSelected == _selected && !show) return;

  toggleArrows(_lastSelected, false);
  toggleArrows(_selected, true);
  _lastSelected = _selected;
  _blink.start();
}

void MenuSound::toggleArrows(uint8_t option, bool show) {
  Draw::triangle(Draw::DIR_RIGHT, _startX[option] - (SIZE + 3), DIAMOND_Y, show);
  Draw::triangle(Draw::DIR_LEFT, _endX[option] + (SIZE + 1), DIAMOND_Y, show);
}

// ========================================================
// Fin — Funciones comunes de las ventanas
// ========================================================
