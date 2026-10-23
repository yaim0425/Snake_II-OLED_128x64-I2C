#include "Playfield.h"
#include "src/Globals.h"

// ========================================================
// Funciones locales
// ========================================================

uint8_t Playfield::cell() {
  return storage.scale();
}

uint8_t Playfield::pixel() {
  return cell() / SIZE;
}

uint8_t Playfield::cols() {
  return (WIDTH - 2 * MARGIN) / cell();
}

uint8_t Playfield::rows() {
  return (BODY_H - 2 * MARGIN) / cell();
}

int16_t Playfield::originX() {
  return (WIDTH - cols() * cell()) / 2;
}

int16_t Playfield::originY() {
  return BODY_TOP + (BODY_H - rows() * cell()) / 2;
}

// ========================================================
// Fin — Funciones locales
// ========================================================
