#ifndef BUTTON_REPEAT_H
#define BUTTON_REPEAT_H
#include <Arduino.h>
#include "Timer.h"

// ========================================================
// ButtonRepeat — repetición de un botón al mantenerlo presionado
// ========================================================

class ButtonRepeat {
public:
  // Constantes
  // Retardo antes de la primera repetición y cadencia de las siguientes.
  static constexpr uint8_t DELAY = 400;
  static constexpr uint8_t TICK = 100;

  // Funciones
  explicit ButtonRepeat();
  bool step(uint8_t button);

private:
  // Variables
  uint32_t _tick;
  uint32_t _delay;
  Stopwatch _repeat;
  Stopwatch _repeatTick;
};

// ========================================================
// Fin
// ========================================================

#endif