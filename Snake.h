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
  // Constantes de Config
  static constexpr uint8_t COLS = Config::Board::COLS;
  static constexpr uint8_t ROWS = Config::Board::ROWS;
  static constexpr uint8_t MAX_LENGTH = Config::Board::MAX_LENGTH;

  // Constantes
  enum class Result : uint8_t {
    MOVED,
    ATE_FOOT,
    ATE_SPECIAL,
    DIED };

  struct Seg {
    int16_t col;
    int16_t row;
    Draw::Direction dir;
    Sprite::Part part;
  };

  // Funciones comunes de las ventanas
  Snake();
  Result step(const Food& food, const Food& specialFood);
  bool occupied(int16_t cellX, int16_t cellY) const;
  void print(const Food& food, const Food& specialFood);

  // Funciones locales
  void reset();
  void clearPending();
  bool turn(Draw::Direction direction);
  uint8_t length() const;
  Sprite::Part headPart(const Food& food, const Food& specialFood) const;

private:
  // Constantes de Config
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

  // Variables comunes de las ventanas
  Seg _body[MAX_LENGTH];
  uint8_t _headIndex;
  uint8_t _tailIndex;
  uint8_t _length;
  Draw::Direction _dir;
  Draw::Direction _nextDir;
  bool _bellyPending;
  bool _tailDropped;
  Seg _droppedSeg;

  // Funciones locales
  uint8_t slot(uint8_t slotIndex) const;
  Draw::Direction opposite(Draw::Direction direction) const;
  Sprite::Part bodyPartFor(Draw::Direction inDir, Draw::Direction outDir) const;
  Sprite::Part bellyPartFor(Draw::Direction inDir, Draw::Direction outDir) const;
  void drawSprite(Sprite::Part part, int16_t col, int16_t row, bool white);
};

// ========================================================
// Fin
// ========================================================

#endif