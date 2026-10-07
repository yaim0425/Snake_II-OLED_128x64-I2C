#ifndef BUTTON_REPEAT_H
#define BUTTON_REPEAT_H

#include <Arduino.h>

#include "Config.h"
#include "Timer.h"

// ========================================================
// ButtonRepeat — repetición de un botón al mantenerlo
//
// Primer paso inmediato (evento pressed) y, manteniéndolo, un
// paso cada `tick` ms tras `delay` ms de mantención. Encapsula
// los dos Stopwatch que antes vivían sueltos en Menu y
// MenuDifficulty (donde además habían divergido: hold vs state).
// Los tiempos por defecto salen de Config::Button.
// ========================================================

class ButtonRepeat {
public:

  explicit ButtonRepeat(uint32_t delay = Config::Button::DELAY,
                        uint32_t tick  = Config::Button::TICK);

  // ¿Toca aplicar un paso del botón en este frame?
  bool step(uint8_t button);

private:

  uint32_t _delay;
  uint32_t _tick;
  Stopwatch _repeat;      // inicio de la mantención (retardo)
  Stopwatch _repeatTick;  // último paso de la repetición (cadencia)
};

#endif

// ====================================================================================
// Fin
// ====================================================================================