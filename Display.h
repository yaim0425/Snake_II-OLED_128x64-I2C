#ifndef DISPLAY_H
#define DISPLAY_H
#include <Adafruit_SSD1306.h>
#include "Config.h"

enum TextSize { TEXT_6x8 = 1, TEXT_12x16 = 2 };

// ========================================================
// Display — Pantalla OLED: bus I2C y regiones
// ========================================================

class Display {
public:
  // Constantes comunes de las ventanas
  static constexpr uint8_t WIDTH = Config::Screen::WIDTH;
  static constexpr uint8_t HEIGHT = Config::Screen::HEIGHT;

  static constexpr uint8_t CHAR_W = Config::Screen::CHAR_W;
  static constexpr uint8_t CHAR_H = Config::Screen::CHAR_H;

  static constexpr uint8_t HEADER_TOP = Config::Screen::HEADER_TOP;

  static constexpr uint8_t BODY_TOP = Config::Screen::BODY_TOP;
  static constexpr uint8_t BODY_H = Config::Screen::BODY_H;
  static constexpr uint8_t BODY_MIDDLE = Config::Screen::BODY_MIDDLE;

  static constexpr uint8_t FOOT_H = Config::Screen::FOOT_H;
  static constexpr uint8_t FOOT_TOP = Config::Screen::FOOT_TOP;

  static constexpr uint32_t CLOCK_HZ = 400 * 1000;

  // Funciones
  Display(uint8_t sda, uint8_t scl, uint8_t address);

  void begin();
  void clear();
  void show();
  void power(bool on);
  void drawPixel(uint8_t x, uint8_t y, bool white);
  void fillRect(
    uint8_t x, uint8_t y,
    uint16_t width, uint16_t height,
    bool white);
  void fillTriangle(
    uint8_t x0, uint8_t y0,
    uint8_t x1, uint8_t y1,
    uint8_t x2, uint8_t y2,
    bool white);
  void drawText(
    const char* text, uint8_t x, uint8_t y, uint8_t size,
    bool tColor, bool bgColor);
  void drawBitmap(
    uint8_t x, uint8_t y, const uint8_t* bitmap,
    uint8_t w, uint8_t h, bool color, bool bgColor);

  uint8_t centerX(const char* text, uint8_t size) const;

private:
  // Variables
  Adafruit_SSD1306* _screen;
  uint8_t _sda;
  uint8_t _scl;
  uint8_t _address;
};

// ========================================================
// Fin
// ========================================================

#endif