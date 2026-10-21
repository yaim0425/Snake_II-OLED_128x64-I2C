#ifndef DRAW_H
#define DRAW_H
#include <Arduino.h>

// ========================================================
// Draw — figuras: triángulo y rombo
// ========================================================

namespace Draw {
  // Constantes
  enum Direction : uint8_t { DIR_NONE = 0, DIR_UP, DIR_RIGHT, DIR_DOWN, DIR_LEFT };

  // Media anchura del rombo, en píxeles.
  static constexpr uint8_t SIZE = 3;

  // Funciones locales
  void triangle(int8_t dir, int16_t centerX, int16_t centerY, bool show);
  void diamond(int16_t centerX, int16_t centerY, bool show);
}

// ========================================================
// Fin
// ========================================================

#endif