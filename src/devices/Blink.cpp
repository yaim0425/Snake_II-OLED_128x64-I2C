#include "Blink.h"

// ========================================================
// Funciones comunes de las ventanas
// ========================================================

void Blink::start() {
  _timer.start();
  _last = false;
}

// ========================================================
// Fin — Funciones comunes de las ventanas
// ========================================================





// ========================================================
// Funciones locales
// ========================================================

bool Blink::isVisible(uint32_t period, uint8_t offPct) const {
  return _timer.blinkOn(period, offPct);
}

bool Blink::changed(uint32_t period, uint8_t offPct) {
  bool now = _timer.blinkOn(period, offPct);
  if (now == _last) return false;
  _last = now;
  return true;
}

// ========================================================
// Fin — Funciones locales
// ========================================================
