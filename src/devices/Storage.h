#ifndef STORAGE_H
#define STORAGE_H
#include "src/Config.h"

// ========================================================
// Storage — estado compartido: récord, sonido y dificultad
// ========================================================

class Storage {
public:
  // Constantes de Config
  // Niveles de dificultad admitidos (rango persistible).
  static constexpr uint8_t MIN_LEVEL = Config::Difficulty::MIN_LEVEL;
  static constexpr uint8_t MAX_LEVEL = Config::Difficulty::MAX_LEVEL;
  static constexpr uint8_t DEFAULT_LEVEL = Config::Difficulty::DEFAULT_LEVEL;

  // Funciones comunes de las ventanas
  Storage();

  // Funciones locales
  uint16_t bestScore() const;
  void setBestScore(uint16_t value);
  bool soundEnabled() const;
  void setSoundEnabled(bool enabled);
  uint8_t difficulty() const;
  void setDifficulty(uint8_t level);

private:
  // Variables locales
  uint16_t _bestScore;
  bool _soundEnabled;
  uint8_t _difficulty;
};

// ========================================================
// Fin
// ========================================================

#endif