#ifndef SCROLLER_V_H
#define SCROLLER_V_H

#include "Config.h"
#include "Timer.h"

// ========================================================
// ScrollerV — animación "scroller de 1 bit" vertical
//
// Compone un bloque de varias líneas en un array de int8_t
// (cada byte = 1 columna de 8 px: bit 0 = fila 0, bit 7 =
// fila 7) y lo desliza verticalmente dentro de una banda.
// Una sola banda por instancia.
//
// La banda es siempre fondo negro y texto blanco, con la
// opción de una barra blanca de ancho completo en la primera
// fila (título: letra negra sobre la barra). Cada fila se
// compone centrada en un canvas auxiliar y luego se extraen
// las columnas. Un bloque = `n` filas de `LINE_H` px.
//
// Dirección configurable por slide:
//   - fromBelow = true: entra desde abajo de la banda y se
//     desplaza hacia arriba hasta centrarse (roll de créditos).
//   - fromBelow = false: entra desde arriba y baja hasta
//     centrarse.
//
// Usa la Display global (Globals.h) para centrar el texto y
// volcar la franja a pantalla.
// ========================================================

class ScrollerV {
public:
  // ========================================================
  // Constructor (sin parámetros: usa la Display global)
  // ========================================================

  ScrollerV();

  // ========================================================
  // Inicialización (al entrar en la ventana)
  // ========================================================

  void begin();

  // ========================================================
  // Componer el bloque en la franja y fijar la banda donde
  // se imprimirá.
  //
  // `lineas` son las filas del bloque (la primera es el
  // título si `firstBar` = true), `n` la cantidad de filas.
  // `top` y `height` definen la banda de la pantalla dentro
  // de la cual se desliza el bloque; `fromBelow` la dirección
  // del slide; `size` el factor de escala de la fuente.
  //
  // La fila de título se pinta como barra blanca de ancho
  // completo con letra negra; las demás como texto blanco
  // sobre negro. El texto se compone en un canvas auxiliar y
  // luego se extraen las columnas.
  //
  // `n` se recorta silenciosamente si el bloque no cabe en la
  // banda (alto máximo Config::ScrollerV::MAX_H).
  // ========================================================

  void setBloque(
    const char* const* lineas, uint8_t n,
    int16_t top, int16_t height,
    bool fromBelow = true, uint8_t size = 1, bool firstBar = true);

  // ========================================================
  // Animación vertical (arranca desde el borde de la banda,
  // fuera de pantalla, en la dirección indicada)
  // ========================================================

  void startSlide();

  // ========================================================
  // Actualizar (avanza la animación vertical: 1 px por
  // ANIM_TICK ms, arrancada por startSlide())
  // ========================================================

  // true = la animación sigue su curso (el bloque aún entra
  // en pantalla). false = la animación finalizó (el bloque
  // quedó centrado); las llamadas siguientes devuelven false
  // hasta que setBloque()/startSlide() la reinicien.
  bool update();

  // ========================================================
  // Dibujar (rellena la banda con negro y vuelca encima el
  // bloque, recortado a la banda).
  //
  // Al terminar la animación se pinta el frame final y la
  // bandera _done queda puesta, así que las llamadas
  // siguientes no repintan nada hasta que setBloque()/
  // startSlide() la bajen.
  // ========================================================

  bool print();
  bool done() const;
private:
  // ========================================================
  // Acceso a la franja (_strip)
  // ========================================================

  // Lectura de un bit/píxel dentro de la franja (1 bit por píxel)
  // x: columna (0..WIDTH-1), y: fila (0..STRIP_H-1)
  bool getStripPixel(uint16_t x, uint8_t y) const;

  // Escritura de un bit/píxel dentro de la franja (1 bit por píxel)
  // x: columna (0..WIDTH-1), y: fila (0..STRIP_H-1), value: true = set
  void setStripPixel(uint16_t x, uint8_t y, bool color);

  // py (relativo a la banda) para que el bloque quede centrado
  int16_t targetY() const;

  // ========================================================
  // Geometría y tiempos
  // ========================================================

  // Ancho de la franja (ancho de pantalla)
  static constexpr uint8_t WIDTH = Config::Screen::WIDTH;

  // Alto máximo de la franja (alto del Body)
  static constexpr uint8_t STRIP_H = Config::ScrollerV::MAX_H;

  // Avance de 1 px cada ANIM_TICK ms (acumulador por tiempo)
  static constexpr uint32_t ANIM_TICK = Config::ScrollerV::ANIMATION;

  static constexpr int8_t CHAR_H = Config::Screen::CHAR_H;

  // Aire extra sobre/alto de fila (la letra mide CHAR_H*size px)
  static constexpr int8_t LINE_GAP = 2;

  // ========================================================
  // Estado interno
  // ========================================================

  uint8_t _strip[STRIP_H / 8][WIDTH];  // franja: cada byte = 1 columna de 8 px
  int16_t _top;          // fila de pantalla donde empieza la banda
  int16_t _heightBand;   // alto de la banda en px (fija en setBloque)
  int16_t _height;       // alto actual del bloque en px
  int8_t _size;          // factor de escala de la fuente
  bool _fromBelow;       // true: entra desde abajo y sube; false: al revés
  bool _firstBar;        // true: la primera fila es barra blanca con título
  int16_t _y;            // borde superior del bloque (relativo a la banda)
  int16_t _lastY;        // borde superior en el último frame pintado
  bool _done;            // true: el bloque llegó al centro y ese frame ya se pintó
  Ticker _timer;         // avance de 1 px por ANIM_TICK ms
};

#endif

// ====================================================================================
// Fin
// ====================================================================================