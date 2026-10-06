#include "Settings.h"

// ========================================================
// Constructor
// ========================================================

Settings::Settings()
  : _bestScore(0),
    _soundEnabled(true),
    _difficulty(Config::Difficulty::DEFAULT_LEVEL) {}

// ========================================================
// Mejor puntaje (récord)
// ========================================================

uint16_t Settings::bestScore() const {
  return _bestScore;
}

void Settings::setBestScore(uint16_t value) {
  _bestScore = value;
}

// ========================================================
// Sonido activo
// ========================================================

bool Settings::soundEnabled() const {
  return _soundEnabled;
}

void Settings::setSoundEnabled(bool enabled) {
  _soundEnabled = enabled;
}

// ========================================================
// Dificultad (clamp a Config::Difficulty::MIN_LEVEL..MAX_LEVEL)
// ========================================================

uint8_t Settings::difficulty() const {
  return _difficulty;
}

void Settings::setDifficulty(uint8_t level) {
  if (level < Config::Difficulty::MIN_LEVEL) level = Config::Difficulty::MIN_LEVEL;
  else if (level > Config::Difficulty::MAX_LEVEL) level = Config::Difficulty::MAX_LEVEL;
  _difficulty = level;
}

// ====================================================================================
// Fin
// ====================================================================================