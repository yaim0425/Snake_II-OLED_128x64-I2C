#ifndef FOOD_H
#define FOOD_H
#include <Arduino.h>
#include "src/Config.h"
#include "src/graphics/Sprite.h"

class Game;

// ========================================================
// Food — comida normal y especial del tablero
// ========================================================

class Food {
public:
  // Constantes
  enum class Type : uint8_t { NORMAL = 0, SPECIAL };

  // Vida útil de la especial, en ticks del tablero.
  static constexpr uint8_t SPECIAL_TIME = Config::Board::COLS + Config::Board::ROWS;

  // Funciones comunes de las ventanas
  Food(Type type);
  void clear();
  void print();
  bool occupied(int16_t x, int16_t y) const;

  // Funciones locales
  void lessTime();
  bool spawn(const Game& game);
  void printTime(bool show);
  uint8_t score() const;

private:
  // Constantes de Config
  static constexpr uint8_t CELL = Config::Board::CELL;
  static constexpr uint8_t SCALE = Config::Board::SCALE;
  static constexpr int8_t COLS = Config::Board::COLS;
  static constexpr int8_t ROWS = Config::Board::ROWS;
  static constexpr uint8_t CHAR_W = Config::Screen::CHAR_W;
  static constexpr int16_t WIDTH = Config::Screen::WIDTH;
  static constexpr int16_t BOARD_LEFT = Config::Board::LEFT;
  static constexpr int16_t BOARD_TOP = Config::Board::TOP;

  // Constantes
  static constexpr uint8_t SIZE = Sprite::SIZE;
  static constexpr auto& SPECIAL_FOOD = Special::SPECIAL_FOOD;
  static constexpr uint8_t SPECIAL_FOOD_W = Special::SPECIAL_FOOD_W;
  static constexpr uint8_t SPECIAL_FOOD_H = Special::SPECIAL_FOOD_H;

  // Variables locales
  Type _type;
  int16_t _x;
  int16_t _y;
  bool _has;
  uint8_t _time;
  uint8_t _lastTime;
  uint8_t _score;

  // Funciones locales
  void printNormal(bool show) const;
  void printSpecial(bool show) const;
};

// ========================================================
// Fin
// ========================================================

#endif