#ifndef BLINK_H
#define BLINK_H
#include "Timer.h"

// ========================================================
// Blink — parpadeo anclado, con detección de cambio de estado
// ========================================================

class Blink {
public:
  // Constantes
  // Parpadeo estándar de la interfaz: ciclo de 500 ms con 20 % apagado.
  static constexpr uint32_t PERIOD = 500;
  static constexpr uint8_t OFF     = 20;

  // Funciones
  void start();
  bool isVisible(uint32_t period, uint8_t offPct) const;
  bool changed(uint32_t period, uint8_t offPct);

private:
  // Variables
  bool _last;
  Stopwatch _timer;
};

// ========================================================
// Fin
// ========================================================

#endif