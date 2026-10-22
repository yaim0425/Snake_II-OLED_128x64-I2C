#ifndef MENU_SOUND_H
#define MENU_SOUND_H
#include "src/Config.h"
#include "src/graphics/Draw.h"
#include "src/devices/Timer.h"
#include "src/devices/Blink.h"

// ========================================================
// MenuSound — selector de sonido ON/OFF
// ========================================================

class MenuSound {
public:
  // Funciones comunes de las ventanas
  MenuSound();
  void begin();
  void update();
  void print();
  void firstPrint();
  void clear();
  bool done() const;

private:
  // Constantes de Config
  static constexpr int16_t WIDTH = Config::Screen::WIDTH;
  static constexpr uint8_t CHAR_W = Config::Screen::CHAR_W;
  static constexpr int16_t VALUE_HEIGHT = Config::MenuStrip::VALUE_HEIGHT;
  static constexpr int16_t VALUE_TOP = Config::MenuStrip::VALUE_TOP;
  static constexpr int16_t DIAMOND_Y = Config::MenuStrip::DIAMOND_Y;

  // Constantes
  static constexpr int16_t PAD_LEFT_X = WIDTH * 0.25;
  static constexpr int16_t PAD_RIGHT_X = WIDTH * 0.75;
  static constexpr uint8_t SIZE = Draw::SIZE;

  // Valores para el parpadeo
  static constexpr uint32_t PERIOD = Blink::PERIOD;
  static constexpr uint8_t OFF = Blink::OFF;

  // Variables comunes de las ventanas
  bool _done;
  bool _selected;
  bool _lastSelected;
  Blink _blink;
  bool _clear;

  // Variables locales
  int16_t _onStartX;
  int16_t _onEndX;
  int16_t _offStartX;
  int16_t _offEndX;

  // Funciones comunes de las ventanas
  void blinkOption();
  void nextOption(bool show);
};

// ========================================================
// Fin
// ========================================================

#endif