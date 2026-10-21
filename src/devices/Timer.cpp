#include "Timer.h"
#include <esp_timer.h>

// Tiempo del sistema en milisegundos.
uint64_t nowMs() {
  return (uint64_t)esp_timer_get_time() / 1000ULL;
}

// ========================================================
// Funciones comunes de las ventanas
// ========================================================

void Stopwatch::start() {
  _start = nowMs();
}

Ticker::Ticker(uint16_t period)
  : _period(period) {}

void Ticker::start() {
  _last = nowMs();
  _accum = 0;
}

// ========================================================
// Fin — Funciones comunes de las ventanas
// ========================================================





// ========================================================
// Funciones locales
// ========================================================

uint64_t Stopwatch::elapsed() const {
  return nowMs() - _start;
}

bool Stopwatch::expired(uint16_t ms) const {
  return elapsed() >= ms;
}

bool Stopwatch::blinkOn(uint16_t period, uint8_t offPct) const {
  return (elapsed() % period) >= (uint64_t)period * offPct / 100;
}

uint8_t Ticker::consume() {
  uint64_t now = nowMs();
  _accum += now - _last;
  _last = now;
  uint8_t steps = (uint8_t)(_accum / _period);
  _accum %= _period;
  return steps;
}

// ========================================================
// Fin — Funciones locales
// ========================================================
