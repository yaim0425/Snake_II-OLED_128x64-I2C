#include "Storage.h"

Storage::Storage()
  : _bestScore(0),
    _soundEnabled(true),
    _difficulty(DEFAULT_LEVEL) {}

uint16_t Storage::bestScore() const {
  return _bestScore;
}

void Storage::setBestScore(uint16_t value) {
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