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

  // Escalas de celda admitidas (px por celda).
  static constexpr uint8_t MIN_CELL = Config::Board::CELL_MIN;
  static constexpr uint8_t MAX_CELL = Config::Board::CELL_MAX;
  static constexpr uint8_t DEFAULT_CELL = Config::Board::CELL_DEFAULT;
  static constexpr uint8_t CELL_STEP = Config::Board::CELL_STEP;

  // Funciones comunes de las ventanas
  Storage();

  // Funciones locales
  uint32_t bestScore() const;
  void setBestScore(uint32_t value);

  bool soundEnabled() const;
  void setSoundEnabled(bool enabled);

  uint8_t difficulty() const;
  void setDifficulty(uint8_t level);

  uint8_t scale() const;
  void setScale(uint8_t cell);

private:
  // Variables locales
  uint32_t _bestScore;
  bool _soundEnabled;
  uint8_t _difficulty;
  uint8_t _scale;
};

// ========================================================
// Fin
// ========================================================

#endif