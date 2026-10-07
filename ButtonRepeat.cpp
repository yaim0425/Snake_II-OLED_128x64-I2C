#include "ButtonRepeat.h"
#include "Globals.h"

ButtonRepeat::ButtonRepeat(uint32_t delay, uint32_t tick)
  : _delay(delay), _tick(tick), _repeat(), _repeatTick() {}

bool ButtonRepeat::step(uint8_t button) {
  if (buttons.pressed(button)) {
    _repeat.start();
    _repeatTick.start();
    return true;
  }

  if (buttons.hold(button) && _repeat.expired(_delay) && _repeatTick.expired(_tick)) {
    _repeatTick.start();
    return true;
  }

  return false;
}

// ====================================================================================
// Fin
// ====================================================================================