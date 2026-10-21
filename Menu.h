#ifndef MENU_H
#define MENU_H
#include "Config.h"
#include "Timer.h"
#include "Blink.h"
#include "ButtonRepeat.h"
#include "Scroller.h"

// ========================================================
// Menu — menú principal
// ========================================================

class Menu {
public:
  // Constantes
  enum Option : uint8_t { OPT_NEW = 0,
                          OPT_CONTINUE,
                          OPT_DIFFICULTY,
                          OPT_SOUND,
                          OPT_CREDITS,
                          OPT_COUNT };

  // Funciones comunes de las ventanas
  Menu();
  void begin(bool showContinue);
  void update();
  void print();
  void firstPrint(bool forceRedraw);
  bool done() const;

  // Funciones locales
  void showOptions();
  int8_t selected() const;

private:
  // Constantes de Config
  static constexpr int16_t WIDTH = Config::Screen::WIDTH;
  static constexpr int16_t FOOT_TOP = Config::Screen::FOOT_TOP;
  static constexpr uint8_t HEADER_TOP = Config::Screen::HEADER_TOP;
  static constexpr int16_t FOOT_H = Config::Screen::FOOT_H;
  static constexpr int16_t TRIANGLE_Y = Config::MenuStrip::TRIANGLE_Y;
  static constexpr int16_t DIAMOND_Y = Config::MenuStrip::DIAMOND_Y;
  static constexpr int16_t BOX_HEIGHT = Config::MenuStrip::BOX_HEIGHT;
  static constexpr int16_t BOX_TOP = Config::MenuStrip::BOX_TOP;
  static constexpr int16_t VALUE_HEIGHT = Config::MenuStrip::VALUE_HEIGHT;
  static constexpr int16_t VALUE_TOP = Config::MenuStrip::VALUE_TOP;
  static constexpr const char* NAME = Config::Version::NAME;
  static constexpr const char* VERSION = Config::Version::VERSION;

  // Constantes
  static constexpr const char* OPTION[OPT_COUNT] = {
    "New",
    "Continue",
    "Difficulty",
    "Sound",
    "Credits"
  };

  // Parpadeo estándar de la interfaz (Blink).
  static constexpr uint32_t PERIOD = Blink::PERIOD;
  static constexpr uint8_t OFF = Blink::OFF;

  // Variables comunes de las ventanas
  bool _isScrolling;
  int8_t _selected;
  int8_t _lastSelected;
  Blink _blink;
  ButtonRepeat _repeat;
  Scroller _scroller;
  bool _done;
  bool _clear;

  // Variables locales
  bool _showContinue;
  int8_t _space;
  bool _confirm;

  // Funciones comunes de las ventanas
  void blinkOption();
  void nextOption();
  void toggleDiamond(int8_t diamond, bool show);

  // Funciones locales
  void action();
  void toggleText(bool show);
  void navigate();
  void toggleTriangle(int8_t triangle, bool show);
};

// ========================================================
// Fin
// ========================================================

#endif