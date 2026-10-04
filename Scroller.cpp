#include "Scroller.h"
#include "Globals.h"

#include <string.h>

// ========================================================
// Constructor (usa la Display global)
// ========================================================

Scroller::Scroller()
  : _height(16),
    _printY(0),
    _rightToLeft(true),
    _slideX(0),
    _done(false),
    _ticker(ANIM_TICK) {
}

// ========================================================
// Inicialización (al entrar en la ventana: franja centrada,
// sin desplazamiento). El texto centrado todavía no está en
// pantalla, así que la bandera se baja para que print() lo
// vuelque en el primer frame.
// ========================================================

void Scroller::begin() {
  _slideX = 0;
  _done = false;
  _ticker.start();
}

// ========================================================
// Componer el texto en la franja (texto centrado) y
// fijar la fila donde se imprimirá
//
// Calcula el límite de caracteres según el tamaño
// (ancho / (6 * size)) y trunca silenciosamente si el
// texto excede el límite. El texto se compone en un
// canvas auxiliar y luego se extraen las columnas.
// ========================================================

void Scroller::setTexto(const char* text, uint8_t height, int16_t printY, uint8_t size) {
  if (height > STRIP_H) height = STRIP_H;
  if (size < 1) size = 1;
  if (size > 3) size = 3;

  _height = height;
  _printY = printY;

  int16_t w = Config::Screen::WIDTH;
  uint8_t maxChars = w / (6 * size);

  char buffer[64];
  strncpy(buffer, text, maxChars);
  buffer[maxChars] = '\0';

  GFXcanvas8 canvas(w, height);
  int16_t textX = (w - strlen(buffer) * 6 * size) / 2;

  canvas.fillScreen(0);
  canvas.setTextSize(size);
  canvas.setTextColor(1);
  canvas.setCursor(textX, 0);
  canvas.print(buffer);

  uint8_t heightBytes = height / 8;
  for (uint8_t row = 0; row < heightBytes; row++) {
    for (uint16_t col = 0; col < w; col++) {
      uint8_t byte = 0;
      for (uint8_t bit = 0; bit < 8; bit++) {
        uint8_t y = row * 8 + bit;
        if (y < height && canvas.getPixel(col, y) != 0) {
          byte |= (uint8_t)(1 << bit);
        }
      }
      _strip[row][col] = (int8_t)byte;
    }
  }

  begin();
}

// ========================================================
// Inicio de la transición lateral: la franja arranca
// completamente fuera de pantalla y entra desde el lado
// indicado (derecha si rightToLeft, izquierda si no)
// ========================================================

void Scroller::startSlide(bool rightToLeft) {
  _rightToLeft = rightToLeft;
  int16_t w = Config::Screen::WIDTH;
  _slideX = _rightToLeft ? w : -w;
  _done = false;
  _ticker.start();
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
  if (_done || _slideX == 0) return false;

  uint32_t steps = _ticker.consume();
  for (uint32_t i = 0; i < steps; i++) {
    if (_rightToLeft) {
      if (_slideX > 0) _slideX--;
    } else {
      if (_slideX < 0) _slideX++;
    }
  }

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

  uint8_t heightBytes = _height / 8;
  int16_t w = Config::Screen::WIDTH;

  for (uint16_t screenX = 0; screenX < w; screenX++) {
    int16_t stripCol = (int16_t)screenX - _slideX;
    if (stripCol < 0 || stripCol >= (int16_t)STRIP_W) continue;

    for (uint8_t row = 0; row < heightBytes; row++) {
      int8_t byte = _strip[row][stripCol];
      for (uint8_t bit = 0; bit < 8; bit++) {
        uint8_t py = _printY + row * 8 + bit;
        if (py >= display.getHeight()) break;
        bool pixel = byte & (1 << bit);
        display.drawPixel(screenX, py, pixel != 0);
      }
    }
  }

  _done = (_slideX == 0);
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
    _strip[rowBytes][x] &= (int8_t)~(1 << bit);
  }
}

// ====================================================================================
// Fin
// ====================================================================================
