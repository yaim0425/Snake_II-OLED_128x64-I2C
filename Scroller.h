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
// Dos usos (los únicos del firmware):
//
//   1. Horizontal (Menu / Legend): el texto entra con su cortina,
//      se desliza y se DETIENE con el texto completo en pantalla
//      (la cortina ya oculta). Modo EndMode::STAY.
//   2. Vertical (MenuCredits): el bloque sube desde fuera de la
//      banda y se detiene en el reposo (STAY); tras la pausa
//      externa del caller se lanza un segundo vuelo EndMode::EXIT
//      que continúa desde el reposo hasta el límite de salida,
//      donde desaparece. El dibujo vertical se recorta a la banda
//      del Body ([BODY_TOP, HEIGHT)): el bloque nunca pisa el
//      Header.
//
// No hay pausas internas: la detención en STAY llega hasta
// `_to` y pone `_done`; el timing lo programa el caller.
//
// Flujo de uso:
//   begin(dir);                              // fija orientación y modo
//   addText("Hola", TEXT_6x8, true, false);  // apila líneas (hasta MAX_LINES)
//   addText("Mundo", TEXT_6x8, false, true);
//   setRow(y);                               // horizontal: fila del bloque
//   // o setVertical(to, from);               // vertical: destino y origen
//   startSlide();                            // rasteriza a _strip y arranca
//   ...
//   if (update()) print();                   // anima
//   ...
//   if (done()) ...                          // llegó a _to (reposo o salida)
//
// Las líneas pasan a `_strip` en `startSlide()`, que después las
// "libera" (resetea `_lineCount`): addText guarda punteros, no copia
// el texto, así que las cadenas deben seguir vivas hasta startSlide().
// ========================================================

class Scroller {
public:
  // ========================================================
  // Comportamiento al llegar a `_to`
  //
  // STAY: el bloque se detiene en `_to` y queda quieto (`_done`).
  // EXIT: el bloque sale por `_to` (en vertical, desaparece al
  //       cruzar la banda; `_to = BODY_TOP - alto del bloque`).
  //
  // EXIT solo aplica a vertical; en horizontal se fuerza STAY.
  // ========================================================

  enum class EndMode : uint8_t { STAY, EXIT };

  // ========================================================
  // Constructor (usa la Display global — Globals.h, en el .cpp)
  // ========================================================

  Scroller();

  // ========================================================
  // Inicialización (al entrar en la ventana)
  //
  // `dir` es un valor de Draw::Direction: fija la orientación y
  // reinicia todo el estado (sin bloque, sin animación, _done = true).
  // `end` decide si al llegar al destino el bloque reposa (STAY) o
  // desaparece (EXIT). Si `EXIT` se combina con horizontal, se
  // fuerza STAY (la horizontal solo reposa).
  // ========================================================

  void begin(int8_t dir, EndMode end = EndMode::STAY);

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
  // Geometría (horizontal: DIR_LEFT / DIR_RIGHT)
  //
  // `y` es la fila de pantalla a la que se muestra el bloque. El
  // Scroller fija los límites: aparece desde el borde con la cortina
  // fuera (`WIDTH ± telón`) y reposa en `_to = 0`, donde el texto
  // queda completo en pantalla y la cortina oculta.
  // ========================================================

  void setRow(int16_t y);

  // ========================================================
  // Geometría (vertical: DIR_UP / DIR_DOWN)
  //
  // `toLimit` es el destino del bloque (el reposo en STAY, o el
  // límite de salida en EXIT); `fromLimit` el origen donde aparece.
  // Ambos son coordenadas de pantalla del borde superior del bloque
  // (deben poder quedar fuera: int16_t). Sin setVertical el vuelo
  // por defecto es la travesía completa (desde HEIGHT hasta -alto).
  // ========================================================

  void setVertical(int16_t toLimit, int16_t fromLimit);

  // ========================================================
  // Arranque del slide
  //
  // Calcula las dimensiones del bloque, lo rasteriza a `_strip`,
  // resuelve los límites (los explícitos o los por defecto de la
  // dirección), fija la posición inicial (`_from`) y arranca el
  // reloj. Después "libera" las líneas pendientes (_lineCount = 0).
  // ========================================================

  void startSlide();

  // ========================================================
  // Actualizar (avanza el bloque 1 px por ANIM_TICK ms)
  //
  // true mientras la animación sigue; false si ya terminó (la
  // llamada no consume reloj). El bloque se mueve desde `_from`
  // hasta `_to` y se detiene ahí (STAY reposa, EXIT desaparece).
  // ========================================================

  bool update();

  // ========================================================
  // Dibujar el bloque en su posición actual
  //
  // Solo repinta mientras la posición cambie. Devuelve true si el
  // bloque sigue animándose y false si ya terminó (done) o si no
  // hubo cambio de posición. En vertical recorta las filas a la
  // banda del Body ([BODY_TOP, HEIGHT)): el Header queda intacto.
  // ========================================================

  bool print();

  // ========================================================
  // Estado: true cuando el bloque llegó a `_to`
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
  static constexpr uint8_t BODY_TOP = Config::Screen::BODY_TOP;  // banda del roll (clip vertical)

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
  EndMode _endMode;                  // STAY (reposa en _to) / EXIT (desaparece en _to)

  int16_t _rowY;                     // fila del bloque (solo horizontal)
  int16_t _from;                     // origen del bloque al arrancar
  int16_t _to;                       // destino: límite de reposo (STAY) o de salida (EXIT)
  bool _geomSet;                     // setRow/setVertical fijó la geometría explícita

  int16_t _width;                    // ancho del bloque en px (siempre WIDTH)
  int16_t _height;                   // alto del bloque en px
  uint8_t _size;                     // size de la primera línea (cortina)

  int16_t _pos;                      // origen actual (x si horizontal, y si vertical)
  int8_t _step;                      // +1 / -1 según el sentido del slide
  int16_t _lastPosX;                 // x del bloque en el frame anterior
  int16_t _lastPosY;                 // y del bloque en el frame anterior

  bool _done;                        // true: llegó a _to

  Ticker _timerH;                    // avance horizontal (1 px / ANIM_H ms)
  Ticker _timerV;                    // avance vertical (1 px / ANIM_V ms)
};

#endif

// ====================================================================================
// Fin
// ====================================================================================