#ifndef MENU_SOUND_H
#define MENU_SOUND_H
#include "src/Config.h"
#include "src/graphics/Draw.h"
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
  static constexpr uint8_t SIZE = Draw::SIZE;

  static constexpr const char* LABEL[] = {"ON", "OFF"};
  static constexpr uint8_t COUNT = sizeof(LABEL) / sizeof(LABEL[0]);
  static constexpr uint8_t SPACE = WIDTH / (COUNT + 1);

  // Valores para el parpadeo
  static constexpr uint32_t PERIOD = Blink::PERIOD;
  static constexpr uint8_t OFF = Blink::OFF;

  // Variables comunes de las ventanas
  int16_t _startX[COUNT];
  int16_t _endX[COUNT];

  bool _done;
  bool _selected;
  bool _lastSelected;
  Blink _blink;
  bool _clear;

  // Funciones comunes de las ventanas
  void blinkOption();
  void nextOption(bool show);
  void toggleArrows(uint8_t option, bool show);
};

// ========================================================
// Fin
// ========================================================

#endif