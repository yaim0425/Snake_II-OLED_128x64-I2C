#include "ScrollerV.h"
#include "Globals.h"

// ========================================================
// Constructor (usa la Display global)
// ========================================================

ScrollerV::ScrollerV()
  : _top(0),
    _heightBand(0),
    _height(0),
    _size(1),
    _fromBelow(true),
    _firstBar(true),
    _y(0),
    _lastY(0),
    _done(true),
    _timer(ANIM_TICK) {
}

// ========================================================
// Inicialización (al entrar en la ventana: sin bloque
// compuesto, sin animación)
// ========================================================

void ScrollerV::begin() {
  _top = 0;
  _heightBand = 0;
  _height = 0;
  _size = 1;
  _fromBelow = true;
  _firstBar = true;
  _y = 0;
  _lastY = 0;
  _done = true;
}

// ========================================================
// Componer el bloque en la franja (ver el .h)
//
// Cada fila se compone en un canvas auxiliar (barra blanca +
// letra negra para el título, letra blanca sobre negro para
// los nombres) y luego se extraen las columnas a la franja.
// ========================================================

void ScrollerV::setBloque(
  const char* const* lineas, uint8_t n,
  int16_t top, int16_t height,
  bool fromBelow, uint8_t size, bool firstBar) {
  if (size < 1) size = 1;
  if (size > 3) size = 3;

  _top = top;
  _heightBand = height;
  _fromBelow = fromBelow;
  _size = size;
  _firstBar = firstBar;

  int16_t lineH = CHAR_H * _size + LINE_GAP;

  // Líneas que caben en la banda (alto máximo de la franja)
  uint8_t maxN = (uint8_t)(STRIP_H / lineH);
  if (n > maxN) n = maxN;

  _height = (int16_t)n * lineH;

  // Banda limpia (todo negro)
  for (uint16_t x = 0; x < WIDTH; x++)
    for (uint8_t y = 0; y < STRIP_H / 8; y++)
      _strip[y][x] = 0;

  for (uint8_t i = 0; i < n; i++) {
    bool bar = _firstBar && (i == 0);
    int16_t y0 = (int16_t)i * lineH;

    GFXcanvas8 canvas(WIDTH, lineH);
    canvas.fillScreen(bar ? SSD1306_WHITE : SSD1306_BLACK);
    canvas.setTextSize(_size);
    canvas.setTextColor(bar ? SSD1306_BLACK : SSD1306_WHITE);
    canvas.setCursor(display.centerX(lineas[i], _size), bar ? 1 : 0);
    canvas.print(lineas[i]);

    for (uint16_t x = 0; x < WIDTH; x++)
      for (int16_t y = 0; y < lineH; y++)
        setStripPixel(x, (uint8_t)(y0 + y), canvas.getPixel(x, y));
  }

  _done = true;
}

// ========================================================
// Inicio de la transición vertical: el bloque arranca
// completamente fuera de la banda y entra desde el lado
// indicado (abajo si fromBelow, arriba si no)
// ========================================================

void ScrollerV::startSlide() {
  _y = _fromBelow ? _heightBand : -_height;
  _lastY = _y + 1;  // fuerza el repintado del primer frame (borra la banda del bloque previo)
  _done = false;
  _timer.start();
}

// ========================================================
// Actualizar: el bloque avanza 1 px por cada ANIM_TICK ms
// (Ticker acumula por tiempo, constante aunque el loop sea
// lento)
//
// Devuelve true mientras la animación sigue su curso (el
// bloque aún está entrando en pantalla) y false cuando
// terminó: el bloque quedó centrado y ya no se mueve. Con la
// animación terminada no se consume el reloj ni se cuenta un
// solo paso, hasta que setBloque()/startSlide() la reinicien.
// ========================================================

bool ScrollerV::update() {
  if (_done) return false;
  _y += _fromBelow ? -(int16_t)_timer.consume() : (int16_t)_timer.consume();
  return true;
}

// ========================================================
// Dibujar: rellenar la banda con negro y volcar encima el
// bloque en su posición actual (recortado a la banda).
//
// El relleno no es un lujo: mientras el bloque entra desde
// el borde solo cubre parte de la banda y el resto
// conservaría lo que hubiera debajo (el bloque anterior).
//
// Se recorre la franja píxel a píxel; solo se pintan los que
// caen dentro de la banda (así los bordes del bloque entran
// y salen sin asomar a medias fuera de ella).
//
// Al llegar al centro se pinta el frame final y la bandera
// _done queda puesta, así las llamadas siguientes no vuelven
// a tocar la banda.
// ========================================================

bool ScrollerV::print() {
  if (_done) return false;
  if (_y == _lastY) return false;

  int16_t target = targetY();
  if (_fromBelow) {
    if (_y <= target) { _y = target; _done = true; }
  } else {
    if (_y >= target) { _y = target; _done = true; }
  }

  // Borra la banda
  display.fillRect(0, _top, WIDTH, (uint16_t)_heightBand, false);

  // Vuelca el bloque (recortado a la banda)
  for (uint16_t x = 0; x < WIDTH; x++)
    for (int16_t y = 0; y < _height; y++) {
      int16_t sy = _y + y;
      if (sy < 0 || sy >= _heightBand) continue;
      if (getStripPixel(x, (uint8_t)y))
        display.drawPixel(x, _top + sy, true);
    }

  _lastY = _y;
  return true;
}

// ========================================================
// Acceso a la franja (_strip)
// ========================================================

bool ScrollerV::getStripPixel(uint16_t x, uint8_t y) const {
  if (x >= WIDTH) return false;
  if (y >= STRIP_H) return false;
  uint8_t rowBytes = y / 8;
  uint8_t bit = y % 8;
  return (_strip[rowBytes][x] & (1 << bit)) != 0;
}

void ScrollerV::setStripPixel(uint16_t x, uint8_t y, bool color) {
  if (x >= WIDTH) return;
  if (y >= STRIP_H) return;
  uint8_t rowBytes = y / 8;
  uint8_t bit = y % 8;
  if (color) {
    _strip[rowBytes][x] |= (int8_t)(1 << bit);
  } else {
    _strip[rowBytes][x] &= (int8_t) ~(1 << bit);
  }
}

// ========================================================
// py (relativo a la banda) para que el bloque quede centrado
// ========================================================

int16_t ScrollerV::targetY() const {
  return (int16_t)((_heightBand - _height) / 2);
}

bool ScrollerV::done() const {
  return _done;
}

// ====================================================================================
// Fin
// ====================================================================================