#include "Food.h"
#include "Game.h"
#include "Globals.h"

#include "Draw.h"

// ========================================================
// Constructor
// ========================================================

Food::Food()
  : _type(Type::NORMAL),
    _x(0),
    _y(0),
    _has(false),
    _time(0) {}

// ========================================================
// Inicialización: no hay alimento
// ========================================================

void Food::begin() {
  _has = false;
  _type = Type::NORMAL;
}

void Food::lessTime() {
  if (_type != Type::SPECIAL) return;
  if (!_time) return;
  // int8_t step = _timer.consume();
  // if (step) _time -= step;
  _time--;
}

// ========================================================
// Colocar un alimento en una celda libre al azar.
//
// Se cuentan las celdas libres (las que `game` no reporta
// ocupadas) y se elige la pick-ésima, sin necesidad de un
// buffer: basta con recorrer la rejilla dos veces. Si no hay
// ninguna celda libre devuelve false y queda sin alimento.
// ========================================================

bool Food::spawn(Type type, const Game& game) {
  // Primer recorrido: contar las celdas libres
  uint8_t freeCells = 0;
  int8_t n = type == Type::NORMAL ? 0 : 1;

  for (uint8_t y = 0; y < ROWS; y++)
    for (uint8_t x = 0; x < COLS - n; x++)
      if (!game.occupied(x, y))
        if (type == Type::NORMAL || (type == Type::SPECIAL && !game.occupied(x + 1, y)))
          freeCells++;

  if (freeCells == 0) {
    _has = false;  // tablero lleno: no hay dónde poner el alimento
    return false;
  }

  // Segundo recorrido: elegir la pick-ésima celda libre
  uint8_t pick = random(freeCells);
  uint8_t seen = 0;
  bool placed = false;

  for (uint8_t y = 0; y < ROWS && !placed; y++)
    for (uint8_t x = 0; x < COLS - n && !placed; x++)
      if (!game.occupied(x, y))
        if (type == Type::NORMAL || (type == Type::SPECIAL && !game.occupied(x + 1, y))) {
          if (seen == pick) {
            _x = x;
            _y = y;
            placed = true;
          }
          seen++;
        }

  if (placed) _score = storage.difficulty();

  if (placed && type == Type::SPECIAL) {
    _time = SPECIAL_TIME;
    _score *= 10;
  }

  _type = type;
  _has = placed;
  _lastTime = 0;
  return placed;
}

// ========================================================
// Quitar el alimento (no hay)
// ========================================================

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

// ========================================================
// Acceso
// ========================================================

bool Food::has() const {
  return _has;
}

Food::Type Food::type() const {
  return _type;
}

bool Food::occupied(uint8_t x, uint8_t y) const {
  if (!_has) return false;
  if (_type == Type::NORMAL)
    return _x == x && _y == y;
  if (_type == Type::SPECIAL)
    return (_x == x || _x + 1 == x) && _y == y;
  return false;
}

// uint8_t Food::x() const {
//   return _x;
// }

// uint8_t Food::y() const {
//   return _y;
// }

// uint8_t Food::specialTime() const {
//   return _specialTime;
// }

// void Food::setSpecialTime(uint8_t seconds) {
//   _specialTime = seconds;
// }

// ========================================================
// Dibujar según el tipo
// ========================================================

void Food::print() {
  if (!_has) return;
  if (_type == Type::SPECIAL) {
    printSpecial();
    printTime(true);
  } else printNormal();
}

// ========================================================
// Alimento normal: rombo simétrico centrado en la celda
// (como el rombo seleccionado del menú)
// ========================================================

void Food::printNormal(bool show) const {
  int16_t centerX = _x * CELL + CELL / 2;
  int16_t centerY = _y * CELL + CELL / 2 + BODY_TOP;
  Draw::diamond(centerX, centerY, show);
}

// ========================================================
// Comida especial: sprite SPECIAL_FOOD (8×4 px de 1 bit)
// centrado verticalmente en la celda (la celda mide CELL px,
// el sprite 4)
// ========================================================

void Food::printSpecial(bool show) const {
  int16_t baseX = _x * CELL;
  int16_t baseY = _y * CELL + BODY_TOP;  // + (CELL - SPECIAL_FOOD_H) / 2;

  for (int8_t y = 0; y < SPECIAL_FOOD_H; y++)
    for (int8_t x = 0; x < SPECIAL_FOOD_W; x++)
      display.fillRect(baseX + x * SCALE, baseY + y * SCALE, SCALE, SCALE, show && Special::specialPixel(x, y));

  // display.drawBitmap(
  //   baseX, baseY,
  //   FOOD, FOOD_WIDTH, FOOD_HEIGHT,
  //   SSD1306_WHITE, SSD1306_BLACK);
}

uint8_t Food::score() const {
  return _score;
}

void Food::printTime(bool show) {
  if (_lastTime == _time && !show) return;

  char time[3];
  if (_time == 0) {
    clear();
    time[0] = ' ';
    time[1] = ' ';
    time[2] = '\0';
  } else
    snprintf(time, sizeof(time), " %u", (unsigned)_time);

  // if (_food.specialTime() == -1)
  // display.drawText(" ", WIDTH - 1, 0, TEXT_12x16);
  // else
  int16_t w = strlen(time) * CHAR_W * TEXT_12x16;
  display.drawText(time, WIDTH - w, 1, TEXT_12x16);

  _lastTime = _time;
}

// ====================================================================================
// Fin
// ====================================================================================
