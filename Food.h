#ifndef FOOD_H
#define FOOD_H
#include <Arduino.h>
#include "Config.h"
#include "Sprite.h"

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

  // Funciones
  Food(Type type);
  void lessTime();
  bool spawn(const Game& game);
  void clear();
  void print();
  void printTime(bool show);
  bool occupied(uint8_t x, uint8_t y) const;
  uint8_t score() const;

private:
  // Constantes
  static constexpr auto& SPECIAL_FOOD = Special::SPECIAL_FOOD;
  static constexpr uint8_t SPECIAL_FOOD_W = Special::SPECIAL_FOOD_W;
  static constexpr uint8_t SPECIAL_FOOD_H = Special::SPECIAL_FOOD_H;

  // Variables comunes de las ventanas
  static constexpr uint8_t CELL = Config::Board::CELL;
  static constexpr uint8_t SCALE = Config::Board::SCALE;
  static constexpr int8_t COLS = Config::Board::COLS;
  static constexpr int8_t ROWS = Config::Board::ROWS;
  static constexpr uint8_t CHAR_W = Config::Screen::CHAR_W;
  static constexpr int16_t WIDTH = Config::Screen::WIDTH;
  static constexpr int8_t BODY_TOP = Config::Screen::BODY_TOP;

  // Funciones
  void printNormal(bool show) const;
  void printSpecial(bool show) const;

  // Variables
  Type _type;
  uint8_t _x;
  uint8_t _y;
  bool _has;
  uint8_t _time;
  uint8_t _lastTime;
  uint8_t _score;
};

// ========================================================
// Fin
// ========================================================

#endif