#include "Food.h"
#include "Game.h"
#include "Globals.h"

#include "Draw.h"

// ========================================================
// Constructor
// ========================================================

Food::Food(uint8_t cols, uint8_t rows, uint8_t top)
  : _cols(cols),
    _rows(rows),
    _top(top),
    _type(Type::NORMAL),
    _x(0),
    _y(0),
    _has(false),
    _specialTime(SPECIAL_TIME) {}

// ========================================================
// Inicialización: no hay alimento
// ========================================================

void Food::begin() {
  _has = false;
  _type = Type::NORMAL;
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
  for (uint8_t y = 0; y < _rows; y++) {
    for (uint8_t x = 0; x < _cols; x++) {
      if (!game.occupied(x, y)) freeCells++;
    }
  }

  if (freeCells == 0) {
    _has = false;  // tablero lleno: no hay dónde poner el alimento
    return false;
  }

  // Segundo recorrido: elegir la pick-ésima celda libre
  uint8_t pick = random(freeCells);
  uint8_t seen = 0;
  bool placed = false;

  for (uint8_t y = 0; y < _rows && !placed; y++) {
    for (uint8_t x = 0; x < _cols && !placed; x++) {
      if (!game.occupied(x, y)) {
        if (seen == pick) {
          _x = x;
          _y = y;
          placed = true;
        }
        seen++;
      }
    }
  }

  _type = type;
  _has = placed;
  return placed;
}

// ========================================================
// Quitar el alimento (no hay)
// ========================================================

void Food::clear() {
  _has = false;
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

uint8_t Food::x() const {
  return _x;
}

uint8_t Food::y() const {
  return _y;
}

uint8_t Food::specialTime() const {
  return _specialTime;
}

void Food::setSpecialTime(uint8_t seconds) {
  _specialTime = seconds;
}

// ========================================================
// Dibujar según el tipo
// ========================================================

void Food::draw() const {
  if (!_has) return;
  if (_type == Type::SPECIAL) drawSpecial();
  else drawNormal();
}

// ========================================================
// Alimento normal: rombo simétrico centrado en la celda
// (como el rombo seleccionado del menú)
// ========================================================

void Food::drawNormal() const {
  int16_t centerX = (int16_t)_x * CELL + CELL / 2;
  int16_t centerY = _top + (int16_t)_y * CELL + CELL / 2;

  Draw::diamond(centerX, centerY);
}

// ========================================================
// Comida especial: sprite SPECIAL_FOOD (8×4 px de 1 bit)
// centrado verticalmente en la celda (la celda mide CELL px,
// el sprite 4)
// ========================================================

void Food::drawSpecial() const {
  int16_t baseX = (int16_t)_x * CELL;
  int16_t baseY = _top + (int16_t)_y * CELL + (CELL - FOOD_HEIGHT) / 2;

  display.drawBitmap(baseX, baseY, FOOD, FOOD_WIDTH, FOOD_HEIGHT, SSD1306_WHITE, SSD1306_BLACK);
}

// ====================================================================================
// Fin
// ====================================================================================
