#include "Buzzer.h"
#include "Timer.h"

// ========================================================
// Funciones comunes de las ventanas
// ========================================================

Buzzer::Buzzer(uint8_t pin)
  : _pin(pin),
    _attached(false),
    _busy(false),
    _startMs(0),
    _durationMs(0) {}

bool Buzzer::begin() {
  _attached = ledcAttach(_pin, 2000, 10);
  stop();
  return _attached;
}

void Buzzer::update() {
  if (!_busy) return;

  if (_durationMs == 0) return;

  if (nowMs() - _startMs < _durationMs) return;

  stop();
}

void Buzzer::stop() {
  if (_attached) ledcWriteTone(_pin, 0);

  _startMs = 0;
  _durationMs = 0;
  _busy = false;
}

// ========================================================
// Fin — Funciones comunes de las ventanas
// ========================================================





// ========================================================
// Funciones locales
// ========================================================

void Buzzer::tone(uint16_t frequency, uint32_t durationMs) {
  if (_attached) {
    if (frequency == 0)
      ledcWriteTone(_pin, 0);
    else
      ledcWriteTone(_pin, frequency);
  }

  _startMs = nowMs();
  _durationMs = durationMs;
  _busy = true;
}

bool Buzzer::busy() const {
  return _busy;
}

// ========================================================
// Fin — Funciones locales
// ========================================================
