#include "Storage.h"

// ========================================================
// Constructor
// ========================================================

Storage::Storage()
  : _bestScore(0),
    _soundEnabled(true),
    _difficulty(Config::Difficulty::DEFAULT_LEVEL) {}

// ========================================================
// Mejor puntaje (récord)
// ========================================================

uint16_t Storage::bestScore() const {
  return _bestScore;
}

void Storage::setBestScore(uint16_t value) {
  _bestScore = value;
}

// ========================================================
// Sonido activo
// ========================================================

bool Storage::soundEnabled() const {
  return _soundEnabled;
}

void Storage::setSoundEnabled(bool enabled) {
  _soundEnabled = enabled;
}

// ========================================================
// Dificultad (clamp a Config::Difficulty::MIN_LEVEL..MAX_LEVEL)
// ========================================================

uint8_t Storage::difficulty() const {
  return _difficulty;
}

void Storage::setDifficulty(uint8_t level) {
  if (level < Config::Difficulty::MIN_LEVEL) level = Config::Difficulty::MIN_LEVEL;
  else if (level > Config::Difficulty::MAX_LEVEL) level = Config::Difficulty::MAX_LEVEL;
  _difficulty = level;
}

// ====================================================================================
// Fin
// ====================================================================================