#ifndef GAME_H
#define GAME_H
#include "src/Config.h"
#include "src/devices/Timer.h"
#include "src/devices/Blink.h"
#include "Food.h"
#include "Snake.h"

// ========================================================
// Game — partida: movimiento, puntuación y overlays
// ========================================================

class Game {
public:
  // Constantes
  enum class State : uint8_t {
    START,
    PLAY,
    PAUSE,
    GAME_OVER
  };

  // Funciones comunes de las ventanas
  Game();
  void begin(bool newGame);
  void update();
  void print();
  void firstPrint(bool forceRedraw);
  bool done() const;
  bool occupied(int16_t x, int16_t y) const;

  // Funciones locales
  bool showContinue() const;
  bool isGameOver() const;
  State state() const;
  bool win() const;
  void speedFor(uint8_t level);

private:
  // Constantes de Config
  static constexpr int16_t BODY_MIDDLE = Config::Screen::BODY_MIDDLE;
  static constexpr uint8_t CHAR_H = Config::Screen::CHAR_H;
  static constexpr uint8_t CHAR_W = Config::Screen::CHAR_W;
  static constexpr int16_t HEIGHT = Config::Screen::HEIGHT;
  static constexpr int16_t WIDTH = Config::Screen::WIDTH;

  // Constantes
  // Conteo de salida (3-2-1) y ritmo de su parpadeo.
  static constexpr int8_t COUNT = 3;
  static constexpr int16_t PERIOD = 1000;
  static constexpr int8_t OFF = 25;

  // Comidas normales requeridas antes de soltar una especial.
  static constexpr int8_t FOOD_COUNT = 10;

  // Variables comunes de las ventanas
  bool _done;
  bool _clear;
  Blink _blink;

  // Variables locales
  State _state;
  bool _win;
  uint16_t _score;
  Ticker _timer;
  int8_t _countdown;
  bool _blinkChanged;
  bool _moveSnake;
  bool _ateFood;
  bool _ateSpecialFood;
  Snake _snake;
  Food _food;
  Food _specialFood;
  int8_t _countFood;

  // Funciones comunes de las ventanas
  void step();

  // Funciones locales
  void startPlay();
  void die();
  void drawOverlay(const char* title, bool fullWidth, bool show);
  void spawnFood(Food& food);
  void printScore();
};

// ========================================================
// Fin
// ========================================================

#endif