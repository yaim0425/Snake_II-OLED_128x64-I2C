#include "Draw.h"
#include "Globals.h"

namespace Draw {

void triangle(int8_t dir, int16_t centerX, int16_t centerY, bool show) {
  const int16_t size = Config::Diamond::SIZE;

  switch (dir) {
    case DIR_UP:  // Arriba (↑)
      display.fillTriangle(
        centerX - size, centerY,
        centerX, centerY - size,
        centerX + size, centerY,
        show);
      break;

    case DIR_RIGHT:  // Derecha (→)
      display.fillTriangle(
        centerX, centerY - size,
        centerX + size, centerY,
        centerX, centerY + size,
        show);
      break;

    case DIR_DOWN:  // Abajo (↓)
      display.fillTriangle(
        centerX - size, centerY,
        centerX, centerY + size,
        centerX + size, centerY,
        show);
      break;

    case DIR_LEFT:  // Izquierda (←)
      display.fillTriangle(
        centerX, centerY - size,
        centerX - size, centerY,
        centerX, centerY + size,
        show);
      break;
  }
}

void diamond(int16_t centerX, int16_t centerY, bool show) {
  triangle(DIR_UP, centerX, centerY, show);
  triangle(DIR_DOWN, centerX, centerY, show);
}

}

// ====================================================================================
// Fin
// ====================================================================================