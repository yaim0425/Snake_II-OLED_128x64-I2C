#ifndef SCROLLER_H
#define SCROLLER_H

#include <Arduino.h>

#include "Config.h"
#include "Timer.h"

// ========================================================
// Scroller — animación "scroller de 1 bit" en 4 direcciones
//
// Reemplaza a ScrollerH y ScrollerV con una sola clase que se
// desliza en cualquiera de las 4 direcciones de Draw::Direction:
//
//   DIR_LEFT  : entra por la derecha y se mueve hacia la izquierda.
//   DIR_RIGHT : entra por la izquierda y se mueve hacia la derecha.
//   DIR_UP    : entra por abajo y sube.
//   DIR_DOWN  : entra por arriba y baja.
//
// El bloque se compone como un mapa de bits de 1 bit por píxel en
// `_strip` (cada byte = 1 columna de 8 px: bit 0 = fila 0, bit 7 =
// fila 7). `_strip` cubre toda la pantalla ([HEIGHT/8][WIDTH]) y el
// bloque vive en su esquina superior izquierda.
//
// Flujo de uso:
//   begin(dir);                         // fija orientación, reinicia
//   addText("Hola", TEXT_6x8, true, false);    // apila líneas (hasta MAX_LINES)
//   addText("Mundo", TEXT_6x8, false, true);
//   setHorizontal(0, WIDTH, y);         // o setVertical(up, down)
//   pause(-1, PAUSE_MS);                // opcional (solo vertical)
//   startSlide();                       // rasteriza a _strip y arranca
//   ...
//   if (update()) print();              // anima
//   ...
//   if (done()) ...                     // terminó de deslizar
//
// Las líneas pasan a `_strip` en `startSlide()`, que después las
// "libera" (resetea `_lineCount`): addText guarda punteros, no copia
// el texto, así que las cadenas deben seguir vivas hasta startSlide().
//
// El reposo (centro de la banda) es la parada por defecto de pause();
// sin pausa configurada el bloque entra, cruza y desaparece en el
// límite opuesto, y ahí done() pasa a true.
// ========================================================

class Scroller {
public:
  // ========================================================
  // Constructor (usa la Display global — Globals.h, en el .cpp)
  // ========================================================

  Scroller();

  // ========================================================
  // Inicialización (al entrar en la ventana)
  //
  // `dir` es un valor de Draw::Direction: fija la orientación y
  // reinicia todo el estado (sin bloque, sin animación, _done = true).
  // ========================================================

  void begin(int8_t dir);

  // ========================================================
  // Componer el bloque pendiente (una llamada por línea)
  //
  // Guarda la línea (texto, colores y factor de escala) hasta
  // startSlide(). `tColor` es el color de letra y `bgColor` el de
  // fondo; cada línea lleva además 1 px de fondo arriba y abajo, así
  // el alto de la línea es size*CHAR_H + 2. El fondo se compone a
  // todo lo ancho de la pantalla y el texto va centrado en ella.
  //
  // `size` (1..3) se recorta silenciosamente. Si el bloque ya tiene
  // MAX_LINES líneas, la nueva se ignora.
  // ========================================================

  void addText(
    const char* text, uint8_t size,
    bool tColor, bool bgColor);

  // ========================================================
  // Geometría para el modo horizontal (DIR_LEFT / DIR_RIGHT)
  //
  // `y` es la fila de pantalla a la que se muestra el bloque.
  // `leftLimit`/`rightLimit` son las columnas donde aparece y
  // desaparece (coordenadas de pantalla, pueden quedar fuera).
  // El reposo queda centrado entre ambos límites.
  // ========================================================

  void setHorizontal(int16_t leftLimit, int16_t rightLimit, int16_t y);

  // ========================================================
  // Geometría para el modo vertical (DIR_UP / DIR_DOWN)
  //
  // `upLimit`/`downLimit` son las filas donde aparece y desaparece
  // (coordenadas de pantalla, pueden quedar fuera). En vertical el
  // bloque se centra horizontalmente en la pantalla y el reposo
  // queda centrado entre ambos límites.
  // ========================================================

  void setVertical(int16_t upLimit, int16_t downLimit);

  // ========================================================
  // Arranque del slide
  //
  // Calcula las dimensiones del bloque, lo rasteriza a `_strip`,
  // resuelve la posición de reposo/parada, fija la posición inicial
  // (el límite de aparición) y arranca el reloj. Después "libera"
  // las líneas pendientes (_lineCount = 0).
  // ========================================================

  void startSlide();

  // ========================================================
  // Pausa de la animación (solo vertical)
  //
  // Detiene el bloque al llegar a la fila absoluta `y` de pantalla.
  // Con `y < 0` (por defecto) la parada es con el bloque centrado
  // entre los límites.
  //
  // `pause != 0`: reanuda sola a los `pause` ms.
  // `pause == 0`: queda detenida hasta resume().
  // ========================================================

  void pause(uint16_t pause = 0, int16_t y = -1);

  // ========================================================
  // Reanudar tras una parada con pause == 0 (solo vertical)
  // ========================================================

  void resume();

  // ========================================================
  // Actualizar (avanza el bloque 1 px por ANIM_TICK ms)
  //
  // true mientras la animación sigue; false si ya terminó (la
  // llamada no consume reloj).
  // ========================================================

  bool update();

  // ========================================================
  // Dibujar (borra el rectángulo barrido por el bloque y lo vuelca
  // en su posición actual; en horizontal, además, la cortina)
  //
  // Solo repinta mientras la posición cambie. Devuelve true si el
  // bloque sigue animándose y false si ya terminó (done) o si no
  // hubo cambio de posición.
  // ========================================================

  bool print();

  // ========================================================
  // Estado: true si el bloque llegó al límite de desaparición
  // ========================================================

  bool done() const;

private:
  // ========================================================
  // Una línea pendiente (addText hasta startSlide)
  // ========================================================

  struct Line {
    const char* text;
    uint8_t size;
    bool tColor;
    bool bgColor;
  };

  // ========================================================
  // Métodos internos
  // ========================================================

  void curtain();                                // adorno de entrada (solo horizontal)
  int16_t timeCurtain() const;                   // ancho del adorno (solo horizontal)
  bool getStripPixel(uint16_t x, uint8_t y) const;
  void setStripPixel(uint16_t x, uint8_t y, bool color);

  void clearStrip();                             // borra _strip entero
  void compose();                                // rasteriza _lines a _strip

  int16_t centerY() const;                     // origen del bloque centrado en la banda
  int16_t startPos() const;                      // origen en el límite de aparición
  int16_t endPos() const;                        // origen en el límite de desaparición
  int16_t blockX() const;                        // x de pantalla del bloque (según _pos)
  int16_t blockY() const;                        // y de pantalla del bloque (según _pos)

  Ticker& ticker() { return _horizontal ? _timerH : _timerV; }

  // ========================================================
  // Geometría y tiempos
  // ========================================================

  static constexpr uint8_t WIDTH = Config::Screen::WIDTH;
  static constexpr uint8_t HEIGHT = Config::Screen::HEIGHT;
  static constexpr uint8_t CHAR_W = Config::Screen::CHAR_W;
  static constexpr uint8_t CHAR_H = Config::Screen::CHAR_H;

  static constexpr uint8_t ROW_BYTES = HEIGHT / 8;  // filas de 8 px en _strip
  static constexpr uint8_t MAX_LINES = Config::Scroller::MAX_LINES;

  static constexpr uint32_t ANIM_H = Config::Scroller::ANIMATION_H;
  static constexpr uint32_t ANIM_V = Config::Scroller::ANIMATION_V;

  // ========================================================
  // Estado interno
  // ========================================================

  uint8_t _strip[ROW_BYTES][WIDTH];  // franja: cada byte = 1 columna de 8 px

  Line _lines[MAX_LINES];            // líneas pendientes (hasta startSlide)
  uint8_t _lineCount;

  int8_t _dir;                       // Draw::Direction
  bool _horizontal;                  // true: DIR_LEFT / DIR_RIGHT

  int16_t _limitA;                   // leftLimit / upLimit
  int16_t _limitB;                   // rightLimit / downLimit
  int16_t _y;                        // fila del bloque (solo horizontal)

  int16_t _width;                    // ancho del bloque en px
  int16_t _height;                   // alto del bloque en px
  uint8_t _size;                     // size de la primera línea (cortina)

  int16_t _pos;                      // origen actual (x si horizontal, y si vertical)
  int16_t _end;                      // origen en el límite de desaparición
  int8_t _step;                      // +1 / -1 según el sentido del slide
  int16_t _lastPosX;                   // x del bloque en el frame anterior
  int16_t _lastPosY;                   // y del bloque en el frame anterior

  bool _pauseSet;                    // hay una parada programada
  bool _pauseDefault;                // la parada usa el centro de la banda
  int16_t _pauseY;                   // fila (origen) donde parar
  uint16_t _pauseMs;                 // duración de la parada (0 = hasta resume())
  bool _waiting;                     // detenido en la parada
  bool _pauseDone;                   // la parada de este slide ya ocurrió
  Stopwatch _pauseSw;                // cronómetro de la parada

  bool _done;                        // true: llegó al límite de desaparición

  Ticker _timerH;                    // avance horizontal (1 px / ANIM_H ms)
  Ticker _timerV;                    // avance vertical (1 px / ANIM_V ms)
};

#endif

// ====================================================================================
// Fin
// ====================================================================================
