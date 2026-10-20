#include "Buttons.h"
#include "Timer.h"

Buttons::Buttons(const int8_t* pins, uint32_t buttonDelay)
  : _buttonDelay(buttonDelay) {
  _rawButtons = 0;
  _buttons = 0;
  _pressed = 0;
  _released = 0;
  _ignore = false;

  for (uint8_t i = 0; i < MAX_BUTTONS; i++) {
    _pins[i] = pins[i];
    _buttonLast[i] = 0;
  }
}

void Buttons::begin() {
  for (uint8_t i = 0; i < MAX_BUTTONS; i++) {
    pinMode(_pins[i], INPUT_PULLDOWN);
    _buttonLast[i] = nowMs();
  }

  _buttons = 0;

  for (uint8_t i = 0; i < MAX_BUTTONS; i++) {
    if (digitalRead(_pins[i]) == HIGH) _buttons |= (uint8_t)(1 << i);
  }

  _rawButtons = _buttons;
}

void Buttons::read() {
  uint64_t now = nowMs();
  _pressed = 0;
  _released = 0;
  uint8_t raw = 0;

  for (uint8_t i = 0; i < MAX_BUTTONS; i++) {
    if (digitalRead(_pins[i]) == HIGH) raw |= (uint8_t)(1 << i);
  }

  uint8_t changed = raw ^ _rawButtons;
  _rawButtons = raw;

  for (uint8_t i = 0; i < MAX_BUTTONS; i++) {
    uint8_t bit = (uint8_t)(1 << i);

    if (changed & bit) _buttonLast[i] = now;

    if ((now - _buttonLast[i]) < _buttonDelay) continue;

    bool state = (raw & bit) != 0;
    bool cur = (_buttons & bit) != 0;

    if (cur == state) continue;

    if (state)
      _buttons |= bit;
    else
      _buttons &= (uint8_t)~bit;

    if (state && !cur)
      _pressed |= bit;
    else
      _released |= bit;
  }

  const uint8_t moves = _buttons & MOVE_MASK;

  if (moves & (moves - 1)) {
    _buttons &= (uint8_t)~MOVE_MASK;
    _pressed &= (uint8_t)~MOVE_MASK;
    _released &= (uint8_t)~MOVE_MASK;
  }

  if (_ignore) {
    _buttons = 0;
    _pressed = 0;
    _released = 0;

    if (raw == 0) _ignore = false;
  }
}

bool Buttons::isSet(uint8_t states, uint8_t botton) {
  return (states & (1 << botton)) != 0;
}

bool Buttons::heldDown(uint8_t index) const {
  return isSet(_buttons, index);
}

bool Buttons::pressed(uint8_t index) const {
  return isSet(_pressed, index);
}

bool Buttons::anyHeld() const {
  return _buttons != 0;
}

bool Buttons::anyPhysical() const {
  return _rawButtons != 0;
}

void Buttons::ignoreUntilRelease() {
  _ignore = true;
  _buttons = 0;
  _pressed = 0;
  _released = 0;
}