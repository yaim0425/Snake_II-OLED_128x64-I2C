#ifndef BUTTONS_H
#define BUTTONS_H
#include <Arduino.h>
#include "Config.h"

// ========================================================
// Buttons — lectura de botones con antirrebote
// ========================================================

class Buttons {
public:
  // Constantes
  enum Button : uint8_t {
    MOVE_UP = 0,
    MOVE_RIGHT,
    MOVE_DOWN,
    MOVE_LEFT,

    ACTION_UP,
    ACTION_RIGHT,
    ACTION_DOWN,
    ACTION_LEFT,
    MAX_BUTTONS
  };

  // Pines por defecto (orden = enum Button).
  static constexpr int8_t PINS[MAX_BUTTONS] = {
    Config::Pin::MOVE_UP, Config::Pin::MOVE_RIGHT, Config::Pin::MOVE_DOWN, Config::Pin::MOVE_LEFT,
    Config::Pin::ACTION_UP, Config::Pin::ACTION_RIGHT, Config::Pin::ACTION_DOWN, Config::Pin::ACTION_LEFT
  };

  // Funciones comunes de las ventanas
  Buttons(const int8_t* pins, uint32_t buttonDelay);
  void begin();

  // Funciones locales
  void read();
  bool heldDown(uint8_t index) const;
  bool pressed(uint8_t index) const;
  bool anyHeld() const;
  bool anyPhysical() const;
  void ignoreUntilRelease();

private:
  // Variables locales
  int8_t _pins[MAX_BUTTONS];
  uint8_t _rawButtons;
  uint8_t _buttons;
  uint8_t _pressed;
  uint8_t _released;
  bool _ignore;
  uint64_t _buttonLast[MAX_BUTTONS];
  uint32_t _buttonDelay;

  // Funciones locales
  static bool isSet(uint8_t states, uint8_t botton);
};

// ========================================================
// Fin
// ========================================================

#endif