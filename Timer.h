#ifndef TIMER_H
#define TIMER_H
#include <Arduino.h>

// Tiempo del sistema en milisegundos.
uint64_t nowMs();

// ========================================================
// Stopwatch — cronómetro: ¿pasaron X ms desde start()?
// ========================================================

class Stopwatch {
public:
  // Funciones
  void start();
  uint64_t elapsed() const;
  bool expired(uint16_t ms) const;
  bool blinkOn(uint16_t period, uint8_t offPct) const;

private:
  // Variables
  uint64_t _start = 0;
};

// ========================================================
// Fin
// ========================================================





// ========================================================
// Ticker — acumulador de pasos periódicos de `period` ms
// ========================================================

class Ticker {
public:
  // Funciones
  explicit Ticker(uint16_t period);
  void start();
  uint8_t consume();

private:
  // Variables
  uint16_t _period;
  uint64_t _last = 0;
  uint16_t _accum = 0;
};

// ========================================================
// Fin
// ========================================================

#endif