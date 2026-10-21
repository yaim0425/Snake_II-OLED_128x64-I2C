#ifndef MENU_DIFFICULTY_H
#define MENU_DIFFICULTY_H
#include "src/Config.h"
#include "src/devices/Timer.h"
#include "src/devices/Blink.h"
#include "src/devices/ButtonRepeat.h"

// ========================================================
// MenuDifficulty — selector de dificultad
// ========================================================

class MenuDifficulty {
public:
  // Funciones comunes de las ventanas
  MenuDifficulty();
  void begin();
  void update();
  void print();
  void firstPrint(bool forceRedraw);
  void clear();
  bool done() const;

private:
  // Constantes de Config
  static constexpr int16_t DEFAULT_LEVEL = Config::Difficulty::DEFAULT_LEVEL;
  static constexpr int16_t MAX_LEVEL = Config::Difficulty::MAX_LEVEL;
  static constexpr int16_t MIN_LEVEL = Config::Difficulty::MIN_LEVEL;
  static constexpr int16_t WIDTH = Config::Screen::WIDTH;
  static constexpr int16_t VALUE_HEIGHT = Config::MenuStrip::VALUE_HEIGHT;
  static constexpr int16_t VALUE_TOP = Config::MenuStrip::VALUE_TOP;
  static constexpr int16_t DIAMOND_Y = Config::MenuStrip::DIAMOND_Y;

  // Constantes
  static constexpr uint32_t PERIOD = Blink::PERIOD;
  static constexpr uint8_t OFF = Blink::OFF;

  // Flechas a los lados del nivel (fuente 6x8) y etiqueta entre ellas.
  static constexpr int16_t TRIANGLE_L = (WIDTH - 6 * 2) / 2 - 4;
  static constexpr int16_t TRIANGLE_R = (WIDTH - 6 * 2) / 2 + (6 * 2) + 2;
  static constexpr int16_t LABEL_X = TRIANGLE_L + 4;

  // Variables comunes de las ventanas
  uint8_t _selected;
  uint8_t _lastSelected;
  bool _done;
  Blink _blink;
  bool _clear;
  ButtonRepeat _repeat;

  // Funciones comunes de las ventanas
  void blinkOption();
  void nextOption(bool show);
};

// ========================================================
// Fin
// ========================================================

#endif