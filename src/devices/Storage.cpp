#include "Storage.h"

// ========================================================
// Funciones comunes de las ventanas
// ========================================================

Storage::Storage()
  : _bestScore(0),
    _soundEnabled(true),
    _difficulty(DEFAULT_LEVEL),
    _scale(DEFAULT_CELL) {}

// ========================================================
// Fin — Funciones comunes de las ventanas
// ========================================================





// ========================================================
// Funciones locales
// ========================================================

uint32_t Storage::bestScore() const {
  return _bestScore;
}

void Storage::setBestScore(uint32_t value) {
  _bestScore = value;
}

bool Storage::soundEnabled() const {
  return _soundEnabled;
}

void Storage::setSoundEnabled(bool enabled) {
  _soundEnabled = enabled;
}

uint8_t Storage::difficulty() const {
  return _difficulty;
}

void Storage::setDifficulty(uint8_t level) {
  if (level < MIN_LEVEL)
    level = MIN_LEVEL;
  else if (level > MAX_LEVEL)
    level = MAX_LEVEL;
  _difficulty = level;
}

uint8_t Storage::scale() const {
  return _scale;
}

void Storage::setScale(uint8_t cell) {
  if (cell < MIN_CELL)
    cell = MIN_CELL;
  else if (cell > MAX_CELL)
    cell = MAX_CELL;
  _scale = (cell / CELL_STEP) * CELL_STEP;
}

// ========================================================
// Fin — Funciones locales
// ========================================================
