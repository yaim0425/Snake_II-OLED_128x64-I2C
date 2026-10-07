#ifndef BLINK_H
#define BLINK_H

#include <Arduino.h>

#include "Timer.h"

// ========================================================
// Blink — parpadeo con ancla temporal y cambio pendiente
//
// Extrae el trío que cada ventana llevaba por separado:
// Stopwatch + el flag de giro (`state()`) + el "hay que
// repintar" (`changed()`). `update()` recalcula el pendiente
// comparando el flag con la fase del período; `toggle()` lo
// invierte y lo consume; `set()` lo fija (p. ej. al confirmar
// en el Menu). `on()` es la fase cruda para quien repinta
// visible/oculto en cada frame (MenuSound, MenuDifficulty).
// ========================================================

class Blink {
public:

  // Ancla el período (llamar en begin()) y fija el flag de giro
  void start(bool state = false);

  // Reancla el período sin tocar el flag (p. ej. al terminar
  // un vuelo del Scroller)
  void restart();

  // Fase cruda del período: ¿toca visible? (oculto el primer offPct%)
  bool on(uint32_t period, uint8_t offPct) const;

  // Recalcula el cambio pendiente (flag != fase). Una vez por update()
  void update(uint32_t period, uint8_t offPct);

  // ¿Hay cambio pendiente? (lo consume toggle()/set())
  bool changed() const;

  // Flag de giro (cada ventana lo interpreta como su estado de dibujo)
  bool state() const;

  // Invierte el flag y consume el pendiente
  void toggle();

  // Fija el flag y consume el pendiente
  void set(bool state);

private:

  Stopwatch _timer;
  bool _state = false;    // flag de giro (interpretación de cada ventana)
  bool _pending = false;  // cambio de fase pendiente de aplicar
};

#endif

// ====================================================================================
// Fin
// ====================================================================================