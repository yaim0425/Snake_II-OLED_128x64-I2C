#ifndef FOOD_H
#define FOOD_H
#include <Arduino.h>
#include "src/Config.h"
#include "src/graphics/Playfield.h"
#include "src/graphics/Sprite.h"

class Game;

// ========================================================
// Food — comida normal y especial del tablero
// ========================================================

class Food {
public:
  // Constantes
  enum class Type : uint8_t { NORMAL = 0, SPECIAL };

  // Funciones comunes de las ventanas
  Food(Type type);
  void clear();
  void print();
  bool occupied(int16_t x, int16_t y) const;

  // Funciones locales
  void lessTime();
  bool spawn(const Game& game);
  void printTime(bool show);
  uint16_t score() const;

private:
  // Constantes de Config
  static constexpr uint8_t CHAR_W = Config::Screen::CHAR_W;
  static constexpr int16_t WIDTH = Config::Screen::WIDTH;

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
  uint16_t _score;

  // Funciones locales
  static uint8_t specialTime();
  void printNormal(bool show) const;
  void printSpecial(bool show) const;
};

// ========================================================
// Fin
// ========================================================

#endif
