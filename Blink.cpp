#include "Blink.h"

void Blink::start() {
  _timer.start();
  _last = false;
}

bool Blink::isVisible(uint32_t period, uint8_t offPct) const {
  return _timer.blinkOn(period, offPct);
}

bool Blink::changed(uint32_t period, uint8_t offPct) {
  bool now = _timer.blinkOn(period, offPct);
  if (now == _last) return false;
  _last = now;
  return true;
}

// ====================================================================================
// Fin
// ====================================================================================