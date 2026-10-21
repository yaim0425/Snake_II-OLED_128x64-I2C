#ifndef BUZZER_H
#define BUZZER_H
#include <Arduino.h>
#include "src/Config.h"

// ========================================================
// Buzzer — capa de hardware (sonido)
// ========================================================

class Buzzer {
public:
  // Constantes de Config
  // Pin del zumbador (PWM).
  static constexpr uint8_t PIN = Config::Pin::BUZZER;

  // Funciones comunes de las ventanas
  Buzzer();
  bool begin();
  void update();
  void stop();

  // Funciones locales
  void tone(uint16_t frequency, uint32_t durationMs);
  bool busy() const;

private:
  // Variables locales
  bool _attached;
  bool _busy;
  uint64_t _startMs;
  uint32_t _durationMs;
};

// ========================================================
// Fin
// ========================================================

#endif