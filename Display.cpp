#include "Display.h"
#include <string.h>

Display::Display(uint8_t sda, uint8_t scl, uint8_t address)
  : _screen(nullptr),
    _sda(sda),
    _scl(scl),
    _address(address) {}

void Display::begin() {
  if (_screen != nullptr) return;

  Wire.begin(_sda, _scl);
  _screen = new Adafruit_SSD1306(WIDTH, HEIGHT, &Wire, _address, -1);

  if (!_screen->begin(SSD1306_SWITCHCAPVCC, _address)) {
    Serial.println("");
    Serial.println("ERROR: No se encontró la pantalla");

    for (;;);
  }

  Wire.setClock(CLOCK_HZ);
  _screen->setRotation(0);
  _screen->clearDisplay();
  _screen->display();
}

void Display::clear() {
  _screen->clearDisplay();
}

void Display::show() {
  _screen->display();
}

void Display::power(bool on) {
  if (on)
    _screen->ssd1306_command(SSD1306_DISPLAYON);
  else
    _screen->ssd1306_command(SSD1306_DISPLAYOFF);
}

void Display::drawPixel(uint8_t x, uint8_t y, bool white) {
  _screen->drawPixel(x, y, white);
}

void Display::fillRect(uint8_t x, uint8_t y, uint16_t width, uint16_t height, bool white) {
  _screen->fillRect(x, y, width, height, white);
}

void Display::fillTriangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, bool white) {
  _screen->fillTriangle(x0, y0, x1, y1, x2, y2, white);
}

void Display::drawText(const char* text, uint8_t x, uint8_t y, uint8_t size, bool tColor, bool bgColor) {
  _screen->setTextSize(size);
  _screen->setTextColor(tColor, bgColor);
  _screen->setCursor(x, y);
  _screen->print(text);
}

void Display::drawBitmap(uint8_t x, uint8_t y, const uint8_t* bitmap, uint8_t width, uint8_t height, bool tColor, bool bgColor) {
  _screen->drawBitmap(x, y, bitmap, width, height, tColor, bgColor);
}

uint8_t Display::centerX(const char* text, uint8_t size) const {
  return (WIDTH - strlen(text) * CHAR_W * size) / 2;
}