#ifndef SCROLLER_H
#define SCROLLER_H
#include <Arduino.h>
#include "src/Config.h"
#include "src/devices/Timer.h"

// ========================================================
// Scroller — desplazamiento de texto con cortina
// ========================================================

class Scroller {
public:
  // Constantes
  enum class EndMode : uint8_t {
    STAY,
    EXIT
  };

  // Funciones comunes de las ventanas
  Scroller();
  void begin(int8_t dir, EndMode end);
  bool update();
  void print();
  bool done() const;

  // Funciones locales
  void addText(const char* text, uint8_t size, bool tColor, bool bgColor);
  void setRow(int16_t y);
  void setVertical(int16_t fromLimit, int16_t toLimit);
  void setEndMode(EndMode end);
  void startSlide();

private:
  // Constantes de Config
  static constexpr uint8_t WIDTH = Config::Screen::WIDTH;
  static constexpr uint8_t HEIGHT = Config::Screen::HEIGHT;
  static constexpr uint8_t CHAR_W = Config::Screen::CHAR_W;
  static constexpr uint8_t CHAR_H = Config::Screen::CHAR_H;
  static constexpr uint8_t BODY_TOP = Config::Screen::BODY_TOP;

  // Constantes
  struct Line {
    const char* text;
    uint8_t size;
    bool tColor;
    bool bgColor;
  };

  static constexpr uint8_t OPTION_TOP = 16;
  static constexpr uint8_t ROW_BYTES = HEIGHT / 8;

  // Capacidad y velocidades de animación de la cortina del scroller.
  static constexpr uint8_t MAX_LINES = 12;
  static constexpr uint8_t ANIM_H = 4;
  static constexpr uint8_t ANIM_V = 12;

  // Variables comunes de las ventanas
  bool _done;

  // Variables locales
  uint8_t _strip[ROW_BYTES][WIDTH];
  Line _lines[MAX_LINES];
  uint8_t _lineCount;
  int8_t _dir;
  bool _horizontal;
  EndMode _endMode;
  int16_t _rowY;
  int16_t _from;
  int16_t _to;
  bool _setLimit;
  int16_t _height;
  uint8_t _size;
  int16_t _pos;
  int8_t _step;
  int16_t _lastPosX;
  int16_t _lastPosY;
  Ticker _timerH;
  Ticker _timerV;

  // Funciones locales
  Ticker& ticker() {
    return _horizontal ? _timerH : _timerV;
  }

  void curtain();
  bool getStripPixel(int16_t x, int16_t y) const;
  void setStripPixel(int16_t x, int16_t y, bool color);
  void setStrip();
  int16_t blockX() const;
  int16_t blockY() const;
};

// ========================================================
// Fin
// ========================================================

#endif