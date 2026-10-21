#include "Scroller.h"
#include <string.h>
#include "src/Globals.h"
#include "Draw.h"

// ========================================================
// Funciones comunes de las ventanas
// ========================================================

Scroller::Scroller()
  : _done(true),
    _lineCount(0),
    _dir(0),
    _horizontal(false),
    _endMode(EndMode::STAY),
    _rowY(0),
    _from(-1),
    _to(-1),
    _setLimit(false),
    _height(0),
    _size(1),
    _pos(0),
    _step(1),
    _lastPosX(0),
    _lastPosY(0),
    _timerH(ANIM_H),
    _timerV(ANIM_V) {}

void Scroller::begin(int8_t dir, EndMode end) {
  _dir = dir;
  _horizontal = (dir == Draw::DIR_LEFT || dir == Draw::DIR_RIGHT);
  setEndMode(end);
  _lineCount = 0;
  _setLimit = false;
  _done = true;
}

bool Scroller::update() {
  if (_done) return false;

  uint8_t steps = ticker().consume();

  if (steps == 0) return true;

  for (uint8_t i = 0; i < steps; i++) {
    if (_pos == _to) break;

    _pos += _step;

    if (_pos == _to) break;
  }

  return true;
}

void Scroller::print() {
  if (_done) return;

  int16_t posX = blockX();
  int16_t posY = blockY();

  if (posX == _lastPosX && posY == _lastPosY) return;

  if (posY != _lastPosY) {
    int16_t bandTop, bandBot;

    if (posY < _lastPosY) {
      bandTop = posY + _height;
      bandBot = _lastPosY + _height;
    } else {
      bandTop = _lastPosY;
      bandBot = posY;
    }

    if (bandTop < 0) bandTop = 0;

    if (bandBot > HEIGHT) bandBot = HEIGHT;

    if (!_horizontal && bandTop < BODY_TOP) bandTop = BODY_TOP;

    if (bandBot > bandTop) display.fillRect(0, bandTop, WIDTH, (uint16_t)(bandBot - bandTop), false);
  }

  for (int16_t x = 0; x < WIDTH; x++) {
    int16_t screenX = posX + x;

    if (screenX < 0 || screenX >= WIDTH) continue;

    for (int16_t y = 0; y < _height; y++) {
      int16_t screenY = posY + y;

      if (screenY < 0 || screenY >= HEIGHT) continue;

      if (!_horizontal && screenY < BODY_TOP) continue;

      display.drawPixel(screenX, screenY, getStripPixel(x, y));
    }
  }

  curtain();
  _lastPosX = posX;
  _lastPosY = posY;
  _done = _pos == _to;
  return;
}

bool Scroller::done() const {
  return _done;
}

// ========================================================
// Fin — Funciones comunes de las ventanas
// ========================================================





// ========================================================
// Funciones locales
// ========================================================

void Scroller::addText(const char* text, uint8_t size, bool tColor, bool bgColor) {
  if (_lineCount >= MAX_LINES) return;

  if (size < 1) size = 1;

  if (size > 3) size = 3;

  _lines[_lineCount].text = text;
  _lines[_lineCount].size = size;
  _lines[_lineCount].tColor = tColor;
  _lines[_lineCount].bgColor = bgColor;
  _lineCount++;
}

void Scroller::setRow(int16_t y) {
  _rowY = y;
  _setLimit = true;
}

void Scroller::setVertical(int16_t fromLimit, int16_t toLimit) {
  _to = toLimit;
  _from = fromLimit;
  _setLimit = true;
}

void Scroller::setEndMode(EndMode end) {
  _endMode = end == EndMode::EXIT && _horizontal ? EndMode::STAY : end;
}

void Scroller::startSlide() {
  if (_lineCount == 0 && _endMode != EndMode::EXIT) {
    _done = true;
    return;
  }

  if (_lineCount) {
    _height = 0;

    for (uint8_t i = 0; i < _lineCount; i++) {
      uint8_t size = _lines[i].size;
      _height += size * CHAR_H + 2;
    }

    if (_height > HEIGHT) _height = HEIGHT;

    _size = _lines[0].size;
  }

  setStrip();

  if (_horizontal) {
    if (!_setLimit) _rowY = OPTION_TOP;

    const int16_t curtainW = (CHAR_H / 2 * _size) + (CHAR_W * _size);

    if (_dir == Draw::DIR_LEFT) {
      _from = WIDTH + curtainW;
      _to = 0;
    } else {
      _from = -WIDTH - curtainW;
      _to = 0;
    }
  } else if (!_setLimit) {
    _from = HEIGHT;
    _to = -_height;
  } else if (_endMode == EndMode::EXIT) {
    _from = _pos;
    _to -= _height;
  }

  _setLimit = false;
  _pos = _from;
  _step = _to > _from ? 1 : -1;

  if (_from == _to) return;

  _done = false;
  _lastPosX = blockX();
  _lastPosY = blockY();
  ticker().start();
  _lineCount = 0;
}

void Scroller::curtain() {
  if (!_horizontal) return;

  int8_t dir = _dir == Draw::DIR_LEFT ? -1 : 1;
  int16_t xBack = _dir == Draw::DIR_LEFT ? blockX() : blockX() + WIDTH;
  int16_t centerX;
  int16_t centerY;

  const int16_t size = (CHAR_H / 2) * _size;
  const int16_t height = _rowY + (CHAR_H / 2) * _size;
  const int16_t xFront = xBack + dir * CHAR_W * _size;

  centerX = xFront;
  centerY = height;
  display.fillTriangle(centerX + dir * size, centerY, centerX, centerY, centerX, centerY - size, false);

  centerX = xFront;
  centerY = height + 1;
  display.fillTriangle(centerX + dir * size, centerY, centerX, centerY, centerX, centerY + size, false);
  centerX = xBack;

  if (dir == -1) centerX -= CHAR_W * _size;

  centerY = _rowY;
  display.fillRect(centerX, centerY, CHAR_W * _size, CHAR_H * _size + 2, false);

  centerX = xBack;
  centerY = height;
  display.fillTriangle(centerX + dir * size, centerY, centerX, centerY, centerX, centerY - size, true);

  centerX = xBack;
  centerY = height + 1;
  display.fillTriangle(centerX + dir * size, centerY, centerX, centerY, centerX, centerY + size, true);
}

bool Scroller::getStripPixel(int16_t x, int16_t y) const {
  if (x >= WIDTH) return false;
  if (y >= HEIGHT) return false;

  uint8_t rowBytes = y / 8;
  uint8_t bit = y % 8;
  return (_strip[rowBytes][x] & (1 << bit)) != 0;
}

void Scroller::setStripPixel(int16_t x, int16_t y, bool color) {
  if (x >= WIDTH) return;
  if (y >= HEIGHT) return;

  uint8_t rowBytes = y / 8;
  uint8_t bit = y % 8;

  if (color)
    _strip[rowBytes][x] |= (uint8_t)(1 << bit);
  else
    _strip[rowBytes][x] &= (uint8_t) ~(1 << bit);
}

void Scroller::setStrip() {
  if (_endMode == EndMode::EXIT) return;

  for (uint8_t y = 0; y < ROW_BYTES; y++)
    for (int16_t x = 0; x < WIDTH; x++) _strip[y][x] = 0;
  int16_t yStart = 0;

  for (uint8_t i = 0; i < _lineCount; i++) {
    const char* text = _lines[i].text;
    uint8_t size = _lines[i].size;
    uint8_t textH = size * CHAR_H;
    int16_t lineH = textH + 2;
    bool tColor = _lines[i].tColor ? SSD1306_WHITE : SSD1306_BLACK;
    bool bgColor = _lines[i].bgColor ? SSD1306_WHITE : SSD1306_BLACK;

    for (int16_t y = 0; y < lineH; y++)
      for (int16_t x = 0; x < WIDTH; x++) setStripPixel(x, yStart + y, _lines[i].bgColor);
    int16_t textW = strlen(text) * CHAR_W * size;
    int16_t xStart = display.centerX(text, size);

    if (xStart < 0) xStart = 0;

    GFXcanvas8 canvas(textW, textH);
    canvas.fillScreen(bgColor);
    canvas.setTextSize(size);
    canvas.setTextColor(tColor, bgColor);
    canvas.setCursor(0, 0);
    canvas.print(text);

    for (int16_t y = 0; y < textH; y++)
      for (int16_t x = 0; x < textW; x++) setStripPixel(xStart + x, yStart + 1 + y, canvas.getPixel(x, y));
    yStart += lineH;
  }
}

int16_t Scroller::blockX() const {
  return _horizontal ? _pos : 0;
}

int16_t Scroller::blockY() const {
  return _horizontal ? _rowY : _pos;
}

// ========================================================
// Fin — Funciones locales
// ========================================================
