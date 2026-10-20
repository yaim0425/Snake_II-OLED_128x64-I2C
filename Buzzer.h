#ifndef BUZZER_H
#define BUZZER_H
#include <Arduino.h>
#include "Config.h"

// ========================================================
// Buzzer — capa de hardware (sonido)
// ========================================================

class Buzzer {
public:
  // Constantes
  // Pin por defecto del zumbador (PWM); sobrescribir en la composición si cambia el montaje.
  static constexpr uint8_t PIN = Config::Pin::BUZZER;

  // Funciones
  Buzzer(uint8_t pin);
  bool begin();
  void update();
  void tone(uint16_t frequency, uint32_t durationMs);
  void stop();
  bool busy() const;

private:
  // Variables
  uint8_t _pin;
  bool _attached;
  bool _busy;
  uint64_t _startMs;
  uint32_t _durationMs;
};

// ========================================================
// Fin
// ========================================================

#endif