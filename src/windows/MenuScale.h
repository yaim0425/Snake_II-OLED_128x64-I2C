#ifndef MENU_SCALE_H
#define MENU_SCALE_H
#include "src/Config.h"
#include "src/graphics/Draw.h"
#include "src/devices/Timer.h"
#include "src/devices/Blink.h"
#include "src/devices/ButtonRepeat.h"

// ========================================================
// MenuScale — selector de escala del tablero
// ========================================================

class MenuScale {
public:
  // Funciones comunes de las ventanas
  MenuScale();
  void begin();
  void update();
  void print();
  void firstPrint();
  void clear();
  bool done() const;
  bool changed() const;

private:
  // Constantes de Config
  static constexpr int16_t WIDTH = Config::Screen::WIDTH;
  static constexpr uint8_t CHAR_W = Config::Screen::CHAR_W;
  static constexpr int16_t VALUE_HEIGHT = Config::MenuStrip::VALUE_HEIGHT;
  static constexpr int16_t VALUE_TOP = Config::MenuStrip::VALUE_TOP;
  static constexpr int16_t DIAMOND_Y = Config::MenuStrip::DIAMOND_Y;

  // Constantes
  static constexpr uint8_t CELL_MIN = Config::Board::CELL_MIN;
  static constexpr uint8_t CELL_MAX = Config::Board::CELL_MAX;
  static constexpr uint8_t CELL_STEP = Config::Board::CELL_STEP;
  static constexpr uint8_t SIZE = Draw::SIZE;

  // Escalas disponibles: 4, 8 y 12 px por celda.
  static constexpr uint8_t COUNT = (CELL_MAX - CELL_MIN) / CELL_STEP + 1;
  static constexpr uint8_t LABEL_LEN = 4;

  // Anclas de las etiquetas en la banda de valores.
  static constexpr int16_t ANCHOR_X[COUNT] = { 22, 64, 106 };

  // Valores para el parpadeo
  static constexpr uint32_t PERIOD = Blink::PERIOD;
  static constexpr uint8_t OFF = Blink::OFF;

  // Variables comunes de las ventanas
  bool _done;
  uint8_t _selected;
  uint8_t _lastSelected;
  Blink _blink;
  bool _clear;
  bool _changed;

  // Variables locales
  int16_t _startX[COUNT];
  int16_t _endX[COUNT];
  char _label[COUNT][LABEL_LEN];
  ButtonRepeat _repeat;

  // Funciones comunes de las ventanas
  void blinkOption();
  void nextOption(bool show);
  void toggleArrows(uint8_t option, bool show);
  static uint8_t cellAt(uint8_t option);
};

// ========================================================
// Fin
// ========================================================

#endif
