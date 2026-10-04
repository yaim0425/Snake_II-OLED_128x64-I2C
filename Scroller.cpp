#include "Scroller.h"
#include "Globals.h"

#include <string.h>

// ========================================================
// Constructor (usa la Display global)
// ========================================================

Scroller::Scroller()
  : _height(16),
    _y(0),
    _toLeft(true),
    _x(0),
    _prevX(0),
    _done(true),
    _timer(ANIM_TICK) {
}

// ========================================================
// Inicialización (al entrar en la ventana: franja centrada,
// sin desplazamiento). El texto centrado todavía no está en
// pantalla, así que la bandera se baja para que print() lo
// vuelque en el primer frame.
// ========================================================

// void Scroller::begin() {
//   _slideX = 0;
//   _done = false;
//   _ticker.start();
// }

// ========================================================
// Componer el texto en la franja (texto centrado) y
// fijar la fila donde se imprimirá
//
// Calcula el límite de caracteres según el tamaño
// (ancho / (6 * size)) y trunca silenciosamente si el
// texto excede el límite. El texto se compone en un
// canvas auxiliar y luego se extraen las columnas.
// ========================================================

void Scroller::setTexto(const char* text, int16_t y, uint8_t size) {
  if (size < 1) size = 1;
  if (size > 3) size = 3;

  _height = 8 * size + size;
  _y = y - size;
  _done = true;

  char buffer[24];
  uint8_t maxChars = STRIP_W / (6 * size);
  strncpy(buffer, text, maxChars);
  buffer[maxChars] = '\0';

  GFXcanvas8 canvas(STRIP_W + CURTAIN_W, _height);
  int16_t textX = (STRIP_W - strlen(buffer) * 6 * size) / 2;
  if (_toLeft) textX += CURTAIN_W;

  canvas.fillScreen(0);
  canvas.setTextSize(size);
  canvas.setTextColor(1);
  canvas.setCursor(textX, size);
  canvas.print(buffer);

  int16_t startX = _toLeft ? 0 : STRIP_W;
  int16_t endX = CURTAIN_W + startX;
  for (uint16_t col = startX; col < endX; col++)
    for (uint8_t y = col % 2; y < _height; y += 2)
      canvas.drawPixel(col, y, 1);

  for (uint16_t col = 0; col < STRIP_H / 8; col++)
    for (uint8_t y = 0; y < STRIP_H; y++)
      _strip[col][y] = 255;

  for (uint16_t col = 0; col < STRIP_W + CURTAIN_W; col++)
    for (uint8_t y = 0; y < _height; y++)
      setStripPixel(col, y, canvas.getPixel(col, y) != 0);
}

// ========================================================
// Inicio de la transición lateral: la franja arranca
// completamente fuera de pantalla y entra desde el lado
// indicado (derecha si rightToLeft, izquierda si no)
// ========================================================

void Scroller::startSlide(bool toLeft) {
  _x = 0;
  _prevX = -1;
  _done = false;
  _toLeft = toLeft;
  _timer.start();
}

// ========================================================
// Actualizar: la franja avanza 1 px por cada ANIM_TICK ms
// (Ticker acumula por tiempo, constante aunque el loop sea lento)
//
// Devuelve true mientras la animación sigue su curso (la
// franja aún está entrando en pantalla) y false cuando
// terminó: la franja quedó centrada y ya no se mueve. Con la
// animación terminada no se consume el reloj ni se cuenta un
// solo paso, hasta que begin()/setTexto()/startSlide() la
// reinicien.
// ========================================================

bool Scroller::update() {
  if (_done) return false;
  if (_timer.consume()) _x ++;
  return true;
}

// ========================================================
// Dibujar: volcar la franja a la pantalla en la fila fijada
// por setTexto()
//
// La banda se rellena primero con el fondo de la franja y
// encima se pinta el texto. El relleno no es un lujo: mientras
// la franja entra desde el borde solo cubre parte del ancho y
// las columnas que aún no cubre conservarían lo que hubiera
// debajo (el texto anterior).
//
// Se recorre la pantalla columna a columna. Para cada columna
// de pantalla se calcula su equivalente dentro de la franja
// (`screenX - _slideX`): si cae fuera, no hay nada que
// dibujar; si cae dentro, se pintan los 8 px de cada byte de
// la franja.
//
// La bandera _settled evita repintar en reposo: se pinta
// siempre el frame en el que la franja llega al centro (si se
// saltara, la pantalla se quedaría con el frame anterior, es
// decir corrida) y en cuanto ese frame está en pantalla la
// bandera queda puesta y las llamadas siguientes no vuelven a
// tocar la banda.
//
// Devuelve true si el texto se puede seguir animando (la
// franja aún entra en pantalla) y false en cuanto la animación
// terminó y el frame final ya está pintado.
// ========================================================

bool Scroller::print() {
  if (_done) return false;
  if (_prevX == _x) return false;

  if (_toLeft) {
    int16_t startX = STRIP_W - _x;
    for (int16_t x = 0; x < _x; x++)
      for (int8_t y = 0; y < _height; y++)
        display.drawPixel(startX + x, _y + y, getStripPixel(x, y));
  }



  // for (uint16_t screenX = 0; screenX < STRIP_W + CURTAIN_W; screenX++) {
  //   int16_t stripCol = screenX - _x;
  //   if (stripCol < 0 || stripCol >= STRIP_W) continue;

  //   for (uint8_t y = 0; y < _height; y++) {
  //     uint8_t py = _y + y;
  //     if (py >= Config::Screen::HEIGHT) break;
  //     display.drawPixel(screenX, py, getStripPixel(stripCol, y));
  //   }
  // }

  _done = STRIP_W + CURTAIN_W == _x;
  _prevX = _x;
  return true;
}

// ========================================================
// Acceso a la franja (_strip)
// ========================================================

bool Scroller::getStripPixel(uint16_t x, uint8_t y) const {
  if (x >= STRIP_W) return false;
  if (y >= STRIP_H) return false;
  uint8_t rowBytes = y / 8;
  uint8_t bit = y % 8;
  return (_strip[rowBytes][x] & (1 << bit)) != 0;
}

void Scroller::setStripPixel(uint16_t x, uint8_t y, bool value) {
  if (x >= STRIP_W) return;
  if (y >= STRIP_H) return;
  uint8_t rowBytes = y / 8;
  uint8_t bit = y % 8;
  if (value) {
    _strip[rowBytes][x] |= (int8_t)(1 << bit);
  } else {
    _strip[rowBytes][x] &= (int8_t) ~(1 << bit);
  }
}

// ====================================================================================
// Fin
// ====================================================================================
