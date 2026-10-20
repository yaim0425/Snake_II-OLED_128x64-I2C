#ifndef BOOT_H
#define BOOT_H
#include "Config.h"
#include "Sprite.h"
#include "Timer.h"
#include "Blink.h"

// ========================================================
// Boot — pantalla de arranque (logo y mensaje)
// ========================================================

class Boot {
public:
  // Funciones
  Boot();
  void begin();
  void update();
  void print();
  void firstPrint(bool forceRedraw);
  bool done() const;

private:
  // Variables comunes de las ventanas
  static constexpr int16_t WIDTH = Config::Screen::WIDTH;
  static constexpr int16_t HEADER_TOP = Config::Screen::HEADER_TOP;
  static constexpr uint8_t BODY_H = Config::Screen::BODY_H;
  static constexpr uint8_t BODY_TOP = Config::Screen::BODY_TOP;

  // Constantes
  static constexpr uint32_t PERIOD = Blink::PERIOD;
  static constexpr uint8_t OFF = Blink::OFF;
  static constexpr auto& LOGO = Logo::LOGO;
  static constexpr uint8_t LOGO_W = Logo::LOGO_W;
  static constexpr uint8_t LOGO_H = Logo::LOGO_H;
  static constexpr int8_t MESSAGE_LINES = 2;
  static const char* const MESSAGE[MESSAGE_LINES];

  // Variables
  Blink _blink;
  bool _done;
  bool _clear;

  // Funciones
  void blinkMessage();
  void drawMessage(bool show);
};

// ========================================================
// Fin
// ========================================================

#endif