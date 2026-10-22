#include "Food.h"
#include "Game.h"
#include "src/Globals.h"
#include <stdio.h>
#include <string.h>

// ========================================================
// Funciones comunes de las ventanas
// ========================================================

Food::Food(Type type)
  : _type(type),
    _x(0),
    _y(0),
    _has(false),
    _time(0),
    _lastTime(0),
    _score(0) {}

void Food::clear() {
  if (!_has) return;

  _has = false;

  if (_type == Type::NORMAL) printNormal(false);

  if (_type == Type::SPECIAL) {
    _time = 0;
    printTime(false);
    printSpecial(false);
  }
}

void Food::print() {
  if (!_has) return;

  if (_type == Type::SPECIAL) {
    printSpecial(true);
    printTime(true);
  } else
    printNormal(true);
}

bool Food::occupied(int16_t x, int16_t y) const {
  if (!_has) return false;

  if (_type == Type::NORMAL) return _x == x && _y == y;

  if (_type == Type::SPECIAL) return (_x == x || _x + 1 == x) && _y == y;

  return false;
}

// ========================================================
// Fin — Funciones comunes de las ventanas
// ========================================================





// ========================================================
// Funciones locales
// ========================================================

void Food::lessTime() {
  if (_type != Type::SPECIAL) return;

  if (!_time) return;

  _time--;
}

bool Food::spawn(const Game& game) {
  uint8_t freeCells = 0;
  int8_t n = _type == Type::NORMAL ? 0 : 1;

  for (int16_t y = 0; y < ROWS; y++)
    for (int16_t x = 0; x < COLS - n; x++)
      if (!game.occupied(x, y))
        if (_type == Type::NORMAL || (_type == Type::SPECIAL && !game.occupied(x + 1, y))) freeCells++;

  if (freeCells == 0) {
    _has = false;
    return false;
  }

  uint8_t pick = random(freeCells);
  uint8_t seen = 0;
  bool placed = false;

  for (int16_t y = 0; y < ROWS && !placed; y++)
    for (int16_t x = 0; x < COLS - n && !placed; x++)
      if (!game.occupied(x, y))
        if (_type == Type::NORMAL || (_type == Type::SPECIAL && !game.occupied(x + 1, y))) {
          if (seen == pick) {
            _x = x;
            _y = y;
            placed = true;
          }

          seen++;
        }

  if (placed) _score = storage.difficulty();

  if (placed && _type == Type::SPECIAL) {
    _time = SPECIAL_TIME;
    _score *= 10;
  }

  _has = placed;
  _lastTime = 0;
  return placed;
}

void Food::printTime(bool show) {
  if (_lastTime == _time && !show) return;

  char time[4];

  if (_time == 0) {
    clear();
    time[0] = ' ';
    time[1] = ' ';
    time[2] = '\0';
  } else
    snprintf(time, sizeof(time), " %u", (unsigned)_time);
  int16_t w = strlen(time) * CHAR_W * TEXT_12x16;
  display.drawText(time, WIDTH - w, 1, TEXT_12x16, true, false);
  _lastTime = _time;
}

uint8_t Food::score() const {
  return _score;
}

void Food::printNormal(bool show) const {
  int16_t baseX = _x * CELL + SCALE;
  int16_t baseY = _y * CELL + BODY_TOP + SCALE;

  for (uint8_t y = 0; y < SIZE; y++)
    for (uint8_t x = 0; x < SIZE; x++)
      if (Sprite::pixel(Sprite::FOOD, x, y))
        display.fillRect(baseX + x * SCALE, baseY + y * SCALE, SCALE, SCALE, show);
}

void Food::printSpecial(bool show) const {
  int16_t baseX = _x * CELL;
  int16_t baseY = _y * CELL + BODY_TOP;

  for (int8_t y = 0; y < SPECIAL_FOOD_H; y++)
    for (int8_t x = 0; x < SPECIAL_FOOD_W; x++)
      display.fillRect(baseX + x * SCALE, baseY + y * SCALE, SCALE, SCALE, show && Special::specialPixel(0, x, y));
}

// ========================================================
// Fin — Funciones locales
// ========================================================
