#ifndef SNAKE_H
#define SNAKE_H
#include <Arduino.h>
#include "Config.h"
#include "Food.h"
#include "Draw.h"
#include "Sprite.h"

// ========================================================
// Snake — cuerpo, colisiones y representación
// ========================================================

class Snake {
public:
  // Constantes comunes de las ventanas
  static constexpr uint8_t COLS = Config::Board::COLS;
  static constexpr uint8_t ROWS = Config::Board::ROWS;
  static constexpr uint8_t MAX_LENGTH = Config::Board::MAX_LENGTH;

  // Constantes
  enum class Result : uint8_t { MOVED, ATE_FOOT, ATE_SPECIAL, DIED };

  struct Seg {
    uint8_t col;
    uint8_t row;
    Draw::Direction dir;
    Sprite::Part part;
  };

  // Funciones
  Snake();
  void reset();
  void clearPending();
  bool turn(Draw::Direction direction);
  Result step(const Food& food, const Food& specialFood);
  bool occupied(uint8_t cellX, uint8_t cellY) const;
  uint8_t length() const;
  Sprite::Part headPart(const Food& food, const Food& specialFood) const;
  void print(const Food& food, const Food& specialFood);

private:
  // Constantes comunes de las ventanas
  static constexpr uint8_t CELL = Config::Board::CELL;
  static constexpr uint8_t SCALE = Config::Board::SCALE;
  static constexpr uint8_t BODY_TOP = Config::Screen::BODY_TOP;

  // Constantes
  static constexpr uint8_t SIZE = Sprite::SIZE;

  // Posición, longitud y rumbo iniciales (START_DIR = Draw::DIR_RIGHT).
  static constexpr uint8_t START_COL = 1;
  static constexpr uint8_t START_ROW = 2;
  static constexpr uint8_t START_LENGTH = 4;
  static constexpr uint8_t START_DIR = 2;

  // Funciones
  uint8_t slot(uint8_t slotIndex) const;
  Draw::Direction opposite(Draw::Direction direction) const;
  Sprite::Part bodyPartFor(Draw::Direction inDir, Draw::Direction outDir) const;
  Sprite::Part bellyPartFor(Draw::Direction inDir, Draw::Direction outDir) const;
  void drawSprite(Sprite::Part part, uint8_t col, uint8_t row, bool white);

  // Variables
  Seg _body[MAX_LENGTH];
  uint8_t _headIndex;
  uint8_t _tailIndex;
  uint8_t _length;
  Draw::Direction _dir;
  Draw::Direction _nextDir;
  bool _bellyPending;
  bool _tailDropped;
  Seg _droppedSeg;
};

// ========================================================
// Fin
// ========================================================

#endif