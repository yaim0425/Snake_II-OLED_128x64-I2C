#include "Scroller.h"
#include "Globals.h"

#include <string.h>

// ========================================================
// Constructor (usa la Display global)
// ========================================================

Scroller::Scroller()
  : _height(16),
    _size(2),
    _y(0),
    _toLeft(true),
    _isStopNow(false),
    _x(0),
    _lastX(0),
    _done(true),
    _timer(ANIM_TICK) {
}

// ========================================================
// Inicialización (al entrar en la ventana: franja centrada,
// sin desplazamiento). El texto centrado todavía no está en
// pantalla, así que la bandera se baja para que print() lo
// vuelque en el primer frame.
// ========================================================

void Scroller::begin() {
  _height = 16;
  _size = 2;
  _y = 0;
  _toLeft = true;
  _x = 0;
  _lastX = 0;
  _done = true;
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

void Scroller::setTexto(const char* text, int16_t y, bool toLeft, uint8_t size) {
  if (size < 1) size = 1;
  if (size > 3) size = 3;

  _y = y;
  _size = size;
  _done = true;
  _toLeft = toLeft;
  _height = 8 * size;

  uint8_t maxChars = floor(WIDTH / (6 * size));

  char buffer[24];
  strncpy(buffer, text, maxChars);
  buffer[maxChars] = '\0';

  GFXcanvas8 canvas(WIDTH, _height);
  int16_t textX = (WIDTH - strlen(buffer) * 6 * size) / 2;
  // if (_toLeft) textX += CURTAIN_W;

  canvas.fillScreen(SSD1306_WHITE);
  canvas.setTextSize(size);
  canvas.setTextColor(SSD1306_BLACK);
  canvas.setCursor(textX, 0);
  canvas.print(buffer);

  // int16_t startX = _toLeft ? 0 : WIDTH;
  // int16_t endX = _toLeft ? CURTAIN_W : BUFFER_W;

  // for (int16_t x = startX; x < endX; x++)
  //   for (int8_t y = x % 2; y < _height; y += 2)
  //     canvas.drawPixel(x, y, SSD1306_BLACK);

  for (int16_t x = 0; x < WIDTH; x++)
    for (int16_t y = 0; y < STRIP_H / 8; y++)
      _strip[y][x] = 255;

  for (uint16_t x = 0; x < WIDTH; x++)
    for (uint8_t y = 0; y < _height; y++)
      setStripPixel(x, y, canvas.getPixel(x, y));
}

// ========================================================
// Inicio de la transición lateral: la franja arranca
// completamente fuera de pantalla y entra desde el lado
// indicado (derecha si rightToLeft, izquierda si no)
// ========================================================

void Scroller::startSlide() {
  _x = 0;
  _lastX = 0;
  _done = false;
  _isStopNow = false;
  _timer.start();
  
  int16_t up = _y - 1;
  if (up >= 0 && up < Config::Screen::BODY_H)
    display.fillRect(0, up, WIDTH, 1, true);

  int16_t down = _y + _height;
  if (down >= 0 && down < Config::Screen::BODY_H)
    display.fillRect(0, down, WIDTH, 1, true);
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
  _x += _timer.consume();
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
  if (_lastX == _x) return false;

  if (_x > WIDTH) _x = WIDTH;

  if (_toLeft) {
    int16_t startX = WIDTH - _x;
    for (int16_t x = 0; x < _x; x++) {
      int16_t screenX = startX + x;
      if (screenX < 0 || screenX >= WIDTH) continue;
      for (int8_t y = 0; y < _height; y++)
        display.drawPixel(screenX, _y + y, getStripPixel(x, y));
    }
  } else {
    int16_t startX = WIDTH - _x;
    for (int16_t x = startX; x < WIDTH; x++) {
      int16_t screenX = x - startX;
      if (screenX >= WIDTH) continue;
      for (int8_t y = 0; y < _height; y++)
        display.drawPixel(screenX, _y + y, getStripPixel(x, y));
    }
  }

  // if (_toLeft) {
  //   int16_t startX = STRIP_W - _x;
  //   for (int16_t x = 0; x < _x; x++)
  //     if (startX + x >= 0)
  //       for (int8_t y = 0; y < _height; y++)
  //         display.drawPixel(startX + x, _y + y, getStripPixel(x, y));
  // } else {
  //   int16_t startX = BUFFER_W - _x;
  //   for (int16_t x = startX; x < BUFFER_W; x++)
  //     // if (STRIP_W > x)
  //       for (int8_t y = 0; y < _height; y++)
  //         display.drawPixel(x - startX, _y + y, getStripPixel(x, y));
  // }

  // int16_t w = Config::Screen::WIDTH;
  // for (uint16_t screenX = 0; screenX < w; screenX++) {
  //   int16_t stripCol = (int16_t)screenX - _x;
  //   if (stripCol < 0 || stripCol >= (int16_t)STRIP_W) continue;

  //   for (uint8_t y = 0; y < _height; y++) {
  //     uint8_t py = _y + y;
  //     if (py >= display.getHeight()) break;
  //     bool pixel = getStripPixel((uint16_t)stripCol, y);
  //     if (pixel) {
  //       display.drawPixel(screenX, py, true);
  //     }
  //   }
  // }

  _done = _x == WIDTH;
  _lastX = _x;
  return true;
}

// ========================================================
// Acceso a la franja (_strip)
// ========================================================

bool Scroller::getStripPixel(uint16_t x, uint8_t y) const {
  if (x >= WIDTH) return false;
  if (y >= STRIP_H) return false;
  uint8_t rowBytes = y / 8;
  uint8_t bit = y % 8;
  return (_strip[rowBytes][x] & (1 << bit)) != 0;
}

void Scroller::setStripPixel(uint16_t x, uint8_t y, bool color) {
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

void Scroller::curtain() {
  int8_t dir = _toLeft ? -1 : 1;
  int8_t timeCurtain = 6 * _size + 4 * _size - 1;

  int16_t centerX;
  int16_t centerY;
  int16_t size;

  size = 4 * _size - 1;

  centerX = _x + dir * (6 * _size - 1);
  centerY = _y + 4 * _size - 1;
  display.fillTriangle(
    centerX + dir * size, centerY,
    centerX, centerY,
    centerX, centerY - size,
    false
  );

  centerX = _x + dir * (6 * _size - 1);
  centerY = _y + 4 * _size + 0;
  display.fillTriangle(
    centerX + dir * size, centerY,
    centerX, centerY,
    centerX, centerY + size,
    false
  );

  int8_t width = 6 * _size - 1;
  int8_t height = 8 * _size;
  
  centerX = _x;
  if (dir == -1) centerX -= width;
  centerY = _y;
  display.fillRect(
    centerX, centerY,
    width, height,
    false);

  // size = 4 * _size - 1;
  // centerX = _x + 6 * _size - 1;
  // centerY = _y + 4 * _size + 0;
  // display.fillRect(centerX, centerY, size, 2, false);

  // int16_t N = width - 2 * _size - 1;

  size = 4 * _size - 2;

  centerX = _x;
  centerY = _y + 4 * _size - 1;
  display.fillTriangle(
    centerX + dir * size, centerY,
    centerX, centerY,
    centerX, centerY - size,
    true
  );

  centerX = _x;
  centerY = _y + 4 * _size + 0;
  display.fillTriangle(
    centerX + dir * size, centerY,
    centerX, centerY,
    centerX, centerY + size,
    true
  );
}

bool Scroller::done() const {
  return _done;
}

// ====================================================================================
// Fin
// ====================================================================================
