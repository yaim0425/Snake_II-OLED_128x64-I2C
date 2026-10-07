#include "Blink.h"

void Blink::start(bool state) {
  _timer.start();
  _state = state;
  _pending = false;
}

void Blink::restart() {
  _timer.start();
}

bool Blink::on(uint32_t period, uint8_t offPct) const {
  return _timer.blinkOn(period, offPct);
}

void Blink::update(uint32_t period, uint8_t offPct) {
  _pending = (_state != _timer.blinkOn(period, offPct));
}

bool Blink::changed() const {
  return _pending;
}

bool Blink::state() const {
  return _state;
}

void Blink::toggle() {
  _state = !_state;
  _pending = false;
}

void Blink::set(bool state) {
  _state = state;
  _pending = false;
}

// ====================================================================================
// Fin
// ====================================================================================