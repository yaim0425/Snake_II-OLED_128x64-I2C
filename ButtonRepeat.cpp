#include "ButtonRepeat.h"
#include "Globals.h"

// ========================================================
// Funciones comunes de las ventanas
// ========================================================

ButtonRepeat::ButtonRepeat()
  : _repeat(),
    _repeatTick() {}

bool ButtonRepeat::step(uint8_t button) {
  if (buttons.pressed(button)) {
    _repeatTick.start();
    _repeat.start();
    return true;
  }

  if (buttons.heldDown(button) && _repeat.expired(DELAY) && _repeatTick.expired(TICK)) {
    _repeatTick.start();
    return true;
  }

  return false;
}

// ========================================================
// Fin — Funciones comunes de las ventanas
// ========================================================
