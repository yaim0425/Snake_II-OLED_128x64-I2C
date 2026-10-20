#ifndef MENU_SOUND_H
#define MENU_SOUND_H
#include "Config.h"
#include "Draw.h"
#include "Timer.h"
#include "Blink.h"

// ========================================================
// MenuSound — selector de sonido ON/OFF
// ========================================================

class MenuSound {
public:
  // Funciones
  MenuSound();
  void begin();
  void update();
  void print();
  void firstPrint(bool forceRedraw);
  void clear();
  bool done() const;

private:
  // Variables comunes de las ventanas
  static constexpr int16_t WIDTH = Config::Screen::WIDTH;
  static constexpr int16_t PAD_LEFT_X = Config::Screen::WIDTH * 0.25;
  static constexpr int16_t PAD_RIGHT_X = Config::Screen::WIDTH * 0.75;
  static constexpr uint8_t CHAR_W = Config::Screen::CHAR_W;
  static constexpr int16_t VALUE_HEIGHT = Config::MenuStrip::VALUE_HEIGHT;
  static constexpr int16_t VALUE_TOP = Config::MenuStrip::VALUE_TOP;
  static constexpr int16_t DIAMOND_Y = Config::MenuStrip::DIAMOND_Y;

  // Constantes
  static constexpr uint8_t SIZE = Draw::SIZE;
  static constexpr uint32_t PERIOD = Blink::PERIOD;
  static constexpr uint8_t OFF = Blink::OFF;

  // Funciones
  void blinkOption();
  void nextOption(bool show);

  // Variables
  bool _done;
  bool _selected;
  bool _lastSelected;
  Blink _blink;
  bool _clear;
  int16_t _onStartX;
  int16_t _onEndX;
  int16_t _offStartX;
  int16_t _offEndX;
};

// ========================================================
// Fin
// ========================================================

#endif