#ifndef PLAYFIELD_H
#define PLAYFIELD_H
#include <Arduino.h>
#include "src/Config.h"
#include "src/graphics/Sprite.h"

// ========================================================
// Playfield — geometría viva del área jugable
// ========================================================

namespace Playfield {
  // Constantes de Config
  static constexpr int16_t WIDTH = Config::Screen::WIDTH;

  static constexpr int16_t BODY_TOP = Config::Screen::BODY_TOP;
  static constexpr int16_t BODY_H = Config::Screen::BODY_H;

  static constexpr uint8_t CELL_MIN = Config::Board::CELL_MIN;
  static constexpr uint8_t CELL_MAX = Config::Board::CELL_MAX;
  static constexpr uint8_t CELL_STEP = Config::Board::CELL_STEP;
  static constexpr uint8_t MARGIN = Config::Board::MARGIN;

  // Constantes
  static constexpr uint8_t SIZE = Sprite::SIZE;

  // Funciones locales
  uint8_t cell();
  uint8_t pixel();
  uint8_t cols();
  uint8_t rows();
  int16_t originX();
  int16_t originY();
}

// ========================================================
// Fin
// ========================================================

#endif
