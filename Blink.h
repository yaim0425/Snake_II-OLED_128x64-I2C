#ifndef BLINK_H
#define BLINK_H

#include <Arduino.h>

#include "Timer.h"

// ========================================================
// Blink — parpadeo anclado con detección de flanco
//
// Ancla un período (start) y expone dos lecturas:
//   - isVisible(period, offPct): fase actual (true = visible).
//     Para quien repinta cada frame (MenuDifficulty, MenuSound).
//   - changed(period, offPct): true solo cuando la fase cambió
//     respecto de la última consulta. Para quien redibuja solo
//     en el flanco (Boot, Legend, Menu).
//
// El recuerdo de la última fase (`_last`) vive aquí: las
// ventanas ya no llevan su propio flag de giro.
// ========================================================

class Blink {
public:

  // Ancla el período (llamar en begin())
  void start();

  // Fase actual del período: ¿toca visible? (oculto el primer offPct%)
  bool isVisible(uint32_t period, uint8_t offPct) const;

  // ¿Cambió la fase desde la última consulta? (una vez por flanco)
  bool changed(uint32_t period, uint8_t offPct);

private:

  Stopwatch _timer;
  bool _last = false;  // última fase consultada (para el flanco)
};

#endif

// ====================================================================================
// Fin
// ====================================================================================