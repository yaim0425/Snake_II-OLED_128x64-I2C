#include "Scroller.h"

#include <string.h>

#include "Globals.h"
#include "Draw.h"

// ====================================================================================
// Scroller — scroller de 1 bit en 4 direcciones (ver el .h)
// ====================================================================================

// ====================================================================================
// Constructor (usa la Display global)
// ====================================================================================

Scroller::Scroller()
  : _lineCount(0),
    _dir(Draw::DIR_LEFT),
    _horizontal(true),
    _limitA(0),
    _limitB(0),
    _y(0),
    _width(0),
    _height(0),
    _size(TEXT_6x8),
    _pos(0),
    _end(0),
    _step(0),
    _lastPosX(0),
    _lastPosY(0),
    _pauseSet(false),
    _pauseDefault(true),
    _pauseY(0),
    _pauseMs(0),
    _waiting(false),
    _pauseDone(false),
    _pauseSw(),
    _done(true),
    _timerH(ANIM_H),
    _timerV(ANIM_V) {
  clearStrip();
}

// ====================================================================================
// Inicialización: fija la orientación y reinicia todo el estado
// ====================================================================================

void Scroller::begin(int8_t dir) {
  _dir = dir;
  _horizontal = (dir == Draw::DIR_LEFT || dir == Draw::DIR_RIGHT);

  _lineCount = 0;

  _limitA = 0;
  _limitB = 0;
  _y = 0;

  _width = 0;
  _height = 0;
  _size = TEXT_6x8;

  _pos = 0;
  _end = 0;
  _step = 0;
  _lastPosX = 0;
  _lastPosY = 0;

  _pauseSet = false;
  _pauseDefault = true;
  _pauseY = 0;
  _pauseMs = 0;
  _waiting = false;
  _pauseDone = false;

  _done = true;
}

// ====================================================================================
// addText: apila una línea pendiente (no dibuja; eso lo hace startSlide)
// ====================================================================================

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

// ====================================================================================
// Geometría
// ====================================================================================

void Scroller::setHorizontal(int16_t leftLimit, int16_t rightLimit, int16_t y) {
  _limitA = leftLimit;
  _limitB = rightLimit;
  _y = y;
}

void Scroller::setVertical(int16_t upLimit, int16_t downLimit) {
  _limitA = upLimit;
  _limitB = downLimit;
}

// ====================================================================================
// startSlide: mide y rasteriza el bloque, resuelve la parada y arranca el slide
// ====================================================================================

void Scroller::startSlide() {
  // Dimensiones del bloque: alto = suma de (size*CHAR_H + 2) por línea;
  // ancho = mayor ancho de línea (líneas centradas entre sí).
  _height = 0;
  for (uint8_t i = 0; i < _lineCount; i++) {
    uint8_t size = _lines[i].size;
    _height += size * CHAR_H + 2;
  }
  if (_height > HEIGHT) _height = HEIGHT;

  // El bloque ocupa el ancho completo de la pantalla (cada línea se
  // compone con su fondo a todo lo ancho y el texto centrado).
  _width = WIDTH;

  _size = _lineCount ? _lines[0].size : 1;

  compose();

  // Parada con el centro de la banda (resuelta ahora que hay dimensiones)
  if (_pauseSet && _pauseDefault) _pauseY = centerY();

  // Posición inicial (límite de aparición) y final (desaparición)
  _pos = startPos();
  _end = endPos();
  _step = _end > _pos ? 1 : (_end < _pos ? -1 : 0);

  _done = false;
  _waiting = false;
  _pauseDone = false;

  _lastPosX = blockX();
  _lastPosY = blockY();

  ticker().start();
  _pauseSw.start();

  // Libera las líneas pendientes (ya están en _strip)
  _lineCount = 0;
}

// ====================================================================================
// pause / resume
// ====================================================================================

void Scroller::pause(uint16_t pause, int16_t y) {
  _pauseSet = true;
  _pauseDefault = (y < 0);
  _pauseY = _pauseDefault ? centerY() : y;
  _pauseMs = pause;
  _waiting = false;
  _pauseDone = false;
}

void Scroller::resume() {
  _waiting = false;
}

// ====================================================================================
// Actualizar: avanza 1 px por paso del ticker; atiende la parada
// ====================================================================================

bool Scroller::update() {
  if (_done) return false;

  // Detenido en la parada: solo el reloj (o resume()) la levanta.
  // Se drena el reloj de avance para que la pausa no acumule pasos
  // (si no, al reanudar el bloque saltaría de golpe).
  if (_waiting) {
    ticker().consume();
    if (_pauseMs == 0) return true;
    if (!_pauseSw.expired(_pauseMs)) return true;
    _waiting = false;
  }

  uint8_t steps = ticker().consume();
  if (steps == 0) return true;

  for (uint8_t i = 0; i < steps; i++) {
    if (_pos == _end) break;

    _pos += _step;

    // ¿Pasa por la parada programada?
    if (_pauseSet && !_pauseDone) {
      if ((_step > 0 && _pos >= _pauseY) || (_step < 0 && _pos <= _pauseY)) {
        _pos = _pauseY;
        _pauseDone = true;
        _waiting = true;
        _pauseSw.start();
        break;
      }
    }

    if (_pos == _end) break;
  }

  return true;
}

// ====================================================================================
// Dibujar: borra el rectángulo barrido y vuelca el bloque actual
// ====================================================================================

bool Scroller::print() {
  if (_done) return false;

  int16_t posX = blockX();
  int16_t posY = blockY();

  if (posX == _lastPosX && posY == _lastPosY) return false;

  // Borra la unión del rectángulo anterior y el actual (así la franja
  // que avanza va borrando la vieja). En horizontal se ensancha para
  // cubrir también la huella de la cortina, que se dibuja fuera del
  // bloque.
  // if (_horizontal) {
  //   // int16_t cw = (int16_t)(timeCurtain() + 1);
  //   // int16_t ux0 = ((ox < _lastOX) ? ox : _lastOX) - cw;
  //   // int16_t ux1 = ((ox + _width > _lastOX + _width) ? (int16_t)(ox + _width) : (int16_t)(_lastOX + _width)) + cw;
  //   // int16_t eh = (int16_t)(8 * _size);
  //   // if (eh < _height) eh = _height;
  //   // display.fillRect(ux0, oy, (uint16_t)(ux1 - ux0), (uint16_t)eh, false);
  // } else {
  //   // int16_t ux0 = (ox < _lastOX) ? ox : _lastOX;
  //   // int16_t uy0 = (oy < _lastOY) ? oy : _lastOY;
  //   // int16_t ux1 = (ox + _width > _lastOX + _width) ? (int16_t)(ox + _width) : (int16_t)(_lastOX + _width);
  //   // int16_t uy1 = (oy + _height > _lastOY + _height) ? (int16_t)(oy + _height) : (int16_t)(_lastOY + _height);
  //   // display.fillRect(ux0, uy0, (uint16_t)(ux1 - ux0), (uint16_t)(uy1 - uy0), false);
  // }

  // Vuelca el bloque actual (solo los píxeles encendidos; el resto
  // queda como lo dejó el borrado de arriba)
  for (uint16_t x = 0; x < _width; x++)
    for (uint16_t y = 0; y < _height; y++)
      if(
        (posX + x >= 0 || posX + x < WIDTH) &&
        (posY + y >= 0 || posY + y < HEIGHT)
      )
      display.drawPixel(posX + x, posY + y, getStripPixel(x, y));

  if (_horizontal) curtain();

  _lastPosX = posX;
  _lastPosY = posY;

  if (_pos == _end) _done = true;

  return true;
}

// ====================================================================================
// Estado
// ====================================================================================

bool Scroller::done() const {
  return _done;
}

// ====================================================================================
// Rasterizado del bloque (addText -> _strip)
// ====================================================================================

void Scroller::clearStrip() {
  for (uint8_t y = 0; y < ROW_BYTES; y++)
    for (uint16_t x = 0; x < WIDTH; x++)
      _strip[y][x] = 0;
}

void Scroller::compose() {
  clearStrip();

  int16_t yStart = 0;
  for (uint8_t i = 0; i < _lineCount; i++) {
    const char* text = _lines[i].text;
    uint8_t size = _lines[i].size;
    uint8_t textH = size * CHAR_H;
    int16_t lineH = (int16_t)textH + 2;  // 1 px de fondo arriba y abajo

    bool tColor = _lines[i].tColor ? SSD1306_WHITE : SSD1306_BLACK;
    bool bgColor = _lines[i].bgColor ? SSD1306_WHITE : SSD1306_BLACK;

    // Fondo de la línea a todo lo ancho (incluye los 1 px de arriba y abajo)
    for (int16_t y = 0; y < lineH; y++)
      for (uint16_t x = 0; x < WIDTH; x++)
        setStripPixel(x, (uint8_t)(yStart + y), _lines[i].bgColor);

    // Texto centrado en la pantalla (canvas auxiliar -> _strip)
    int16_t textW = strlen(text) * CHAR_W * size;
    if (textW > 0) {
      int16_t xStart = display.centerX(text, size);
      if (xStart < 0) xStart = 0;

      GFXcanvas8 canvas(textW, textH);
      canvas.fillScreen(bgColor);
      canvas.setTextSize(size);
      canvas.setTextColor(tColor, bgColor);
      canvas.setCursor(0, 0);
      canvas.print(text);

      for (int16_t y = 0; y < textH; y++)
        for (int16_t x = 0; x < textW; x++)
          setStripPixel((uint16_t)(xStart + x), (uint8_t)(yStart + 1 + y), canvas.getPixel(x, y));
    }

    yStart += lineH;
  }
}

// ====================================================================================
// Acceso a la franja (_strip)
// ====================================================================================

bool Scroller::getStripPixel(uint16_t x, uint8_t y) const {
  if (x >= WIDTH) return false;
  if (y >= HEIGHT) return false;
  uint8_t rowBytes = y / 8;
  uint8_t bit = y % 8;
  return (_strip[rowBytes][x] & (1 << bit)) != 0;
}

void Scroller::setStripPixel(uint16_t x, uint8_t y, bool color) {
  if (x >= WIDTH) return;
  if (y >= HEIGHT) return;
  uint8_t rowBytes = y / 8;
  uint8_t bit = y % 8;
  if (color) {
    _strip[rowBytes][x] |= (uint8_t)(1 << bit);
  } else {
    _strip[rowBytes][x] &= (uint8_t) ~(1 << bit);
  }
}

// ====================================================================================
// Posiciones (origen del bloque: x si horizontal, y si vertical)
// ====================================================================================

// Bloque centrado entre los límites (resto por defecto de la parada)
int16_t Scroller::centerY() const {
  return (_limitA + _limitB - _height) / 2;
}

// Origen en el límite de aparición (bloque entrando, fuera de la banda)
int16_t Scroller::startPos() const {
  switch (_dir) {
    case Draw::DIR_LEFT: return _limitB + timeCurtain();   // entra desde la derecha
    case Draw::DIR_RIGHT: return _limitA - timeCurtain();  // entra desde la izquierda
    case Draw::DIR_UP: return _limitB;     // entra desde abajo
    case Draw::DIR_DOWN: return _limitA;   // entra desde arriba
  }
  return _limitA;
}

// Origen en el límite de desaparición (bloque saliendo, fuera de la banda)
int16_t Scroller::endPos() const {
  switch (_dir) {
    case Draw::DIR_LEFT: return _limitA;
    case Draw::DIR_RIGHT: return _limitB;
    case Draw::DIR_UP: return _limitA;
    case Draw::DIR_DOWN: return _limitB;
  }
  return _limitB;
}

int16_t Scroller::blockX() const {
  return _horizontal ? _pos : (WIDTH - _width) / 2;
}

int16_t Scroller::blockY() const {
  return _horizontal ? _y : _pos;
}

// ====================================================================================
// Cortina (adorno de entrada, solo horizontal; portado de ScrollerH)
// ====================================================================================

int16_t Scroller::timeCurtain() const {
  return (CHAR_H / 2 * _size) + (CHAR_W * _size);
}

void Scroller::curtain() {
  int8_t dir = _dir == Draw::DIR_LEFT ? -1 : 1;
  int16_t xBack = _dir == Draw::DIR_LEFT ? blockX() : blockX() + _width;

  int16_t centerX;
  int16_t centerY;
  // int16_t size;
  // int16_t _y = _y;

  const int16_t size = (CHAR_H / 2) * _size;
  const int16_t height = _y + (CHAR_H / 2) * _size;
  const int16_t xFront = xBack + dir * CHAR_W * _size;

  // display.fillRect(0, _y, WIDTH, _height, true);
  // for(int8_t x = 0; x < WIDTH; x++)
  //   display.drawPixel(x, Config::Screen::FOOT_TOP - 1,x % 2);

  centerX = xFront;
  centerY = height;
  display.fillTriangle(
    centerX + dir * size, centerY,
    centerX, centerY,
    centerX, centerY - size,
    false);

  centerX = xFront;
  centerY = height + 1;
  display.fillTriangle(
    centerX + dir * size, centerY,
    centerX, centerY,
    centerX, centerY + size,
    false);

  centerX = xBack;
  if (dir == -1) centerX -= CHAR_W * _size;
  centerY = _y;
  display.fillRect(
    centerX, centerY,
    CHAR_W * _size, CHAR_H * _size + 2,
    false);

  centerX = xBack;
  centerY = height;
  display.fillTriangle(
    centerX + dir * size, centerY,
    centerX, centerY,
    centerX, centerY - size,
    true);

  centerX = xBack;
  centerY = height + 1;
  display.fillTriangle(
    centerX + dir * size, centerY,
    centerX, centerY,
    centerX, centerY + size,
    true);
}

// ====================================================================================
// Fin
// ====================================================================================
