#ifndef SNAKE_H
#define SNAKE_H
#include <Arduino.h>
#include "src/Config.h"
#include "Food.h"
#include "src/graphics/Draw.h"
#include "src/graphics/Playfield.h"
#include "src/graphics/Sprite.h"

// ========================================================
// Snake — cuerpo, colisiones y representación
// ========================================================

class Snake {
public:
  // Constantes de Config
  static constexpr uint16_t MAX_LENGTH = Config::Board::MAX_LENGTH;

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
  uint16_t length() const;
  Sprite::Part headPart(const Food& food, const Food& specialFood) const;

private:
  // Constantes
  static constexpr uint8_t SIZE = Sprite::SIZE;

  // Posición, longitud y rumbo iniciales (START_DIR = Draw::DIR_RIGHT).
  static constexpr uint8_t START_COL = 1;
  static constexpr uint8_t START_ROW = 2;
  static constexpr uint8_t START_LENGTH = 4;
  static constexpr uint8_t START_DIR = 2;

  // Variables locales
  Seg _body[MAX_LENGTH];
  uint16_t _headIndex;
  uint16_t _tailIndex;
  uint16_t _length;
  Draw::Direction _dir;
  Draw::Direction _nextDir;
  bool _bellyPending;
  bool _tailDropped;
  Seg _droppedSeg;

  // Funciones locales
  uint16_t slot(uint16_t slotIndex) const;
  void advance(int16_t col, int16_t row, Draw::Direction dir,
               int16_t& nextCol, int16_t& nextRow) const;
  Draw::Direction opposite(Draw::Direction direction) const;
  Sprite::Part bodyPartFor(Draw::Direction inDir, Draw::Direction outDir) const;
  Sprite::Part bellyPartFor(Draw::Direction inDir, Draw::Direction outDir) const;
  void drawSprite(Sprite::Part part, int16_t col, int16_t row, bool white);
};

// ========================================================
// Fin
// ========================================================

#endif
