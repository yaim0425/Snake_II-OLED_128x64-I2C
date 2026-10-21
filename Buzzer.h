#ifndef BUZZER_H
#define BUZZER_H
#include <Arduino.h>
#include "Config.h"

// ========================================================
// Buzzer — capa de hardware (sonido)
// ========================================================

class Buzzer {
public:
  // Constantes de Config
  // Pin por defecto del zumbador (PWM); sobrescribir en la composición si cambia el montaje.
  static constexpr uint8_t PIN = Config::Pin::BUZZER;

  // Funciones comunes de las ventanas
  Buzzer(uint8_t pin);
  bool begin();
  void update();
  void stop();

  // Funciones locales
  void tone(uint16_t frequency, uint32_t durationMs);
  bool busy() const;

private:
  // Variables locales
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