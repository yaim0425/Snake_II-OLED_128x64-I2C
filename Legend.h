#ifndef LEGEND_H
#define LEGEND_H
#include <Arduino.h>
#include "Config.h"
#include "Timer.h"
#include "Blink.h"
#include "Scroller.h"

// ========================================================
// Legend — leyenda de botones (pantalla de ayuda)
// ========================================================

class Legend {
public:
  // Funciones
  Legend();
  void begin();
  void update();
  void print();
  void firstPrint(bool forceRedraw);
  bool done() const;

private:
  // Funciones
  void diamondCenter(int8_t diamond, int16_t& centerX, int16_t& centerY);
  void toggleDiamond(int8_t diamond, bool show);
  void blinkDiamond();
  void nextBtn();

  // Variables comunes de las ventanas
  static constexpr int16_t WIDTH = Config::Screen::WIDTH;
  static constexpr int16_t PAD_LEFT_X = Config::Screen::WIDTH * 0.25;
  static constexpr int16_t PAD_RIGHT_X = Config::Screen::WIDTH * 0.75;
  static constexpr int16_t TEXT_Y = Config::Screen::HEADER_TOP + 4;
  static constexpr int16_t FOOT_TOP = Config::Screen::FOOT_TOP;
  static constexpr int16_t FOOT_H = Config::Screen::FOOT_H;
  static constexpr int16_t PAD_Y =
    Config::Screen::BODY_TOP + (Config::Screen::BODY_H - Config::Screen::FOOT_H - 1) / 2 - 2;

  // Constantes
  static constexpr int16_t PAD_RADIO = 10;
  static const char* const BTN_FUNC[4];

  // Tiempo por botón antes de pasar al siguiente y parpadeo del rombo.
  static constexpr uint32_t NEXT = 2 * 1000;
  static constexpr uint32_t PERIOD = 100;
  static constexpr uint8_t OFF = 50;

  // Variables
  uint8_t _btn;
  uint8_t _lastBtn;
  Scroller _scroller;
  Ticker _ticker;
  Blink _blink;
  bool _lastScroll;
  bool _done;
  bool _clear;
};

// ========================================================
// Fin
// ========================================================

#endif