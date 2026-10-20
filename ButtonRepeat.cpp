#include "ButtonRepeat.h"
#include "Globals.h"

ButtonRepeat::ButtonRepeat()
  : _repeat(),
    _repeatTick() {}

bool ButtonRepeat::step(uint8_t button) {
  if (buttons.pressed(button)) {
    _repeat.start();
    _repeatTick.start();
    return true;
  }

  if (buttons.heldDown(button) && _repeat.expired(DELAY) && _repeatTick.expired(TICK)) {
    _repeatTick.start();
    return true;
  }

  return false;
}