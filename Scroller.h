#ifndef SCROLLER_H
#define SCROLLER_H

#include "Config.h"
#include "Timer.h"

// ========================================================
// Scroller — animación "scroller de 1 bit"
//
// Compone un texto en un array de int8_t (cada byte = 1
// columna de 8 px: bit 0 = fila 0, bit 7 = fila 7) y lo
// desliza lateralmente. Una sola banda por instancia.
//
// La franja es siempre fondo negro y texto blanco (sin
// parámetros de color). El texto se compone centrado en un
// canvas auxiliar y luego se extraen las columnas.
//
// Usa la Display global (Globals.h) para el ancho de la
// franja y el cálculo del límite de caracteres.
// ========================================================

class Scroller {
public:
  // ========================================================
  // Constructor (sin parámetros: usa la Display global)
  // ========================================================

  Scroller();

  // ========================================================
  // Inicialización (al entrar en la ventana)
  // ========================================================

  void begin();

  // ========================================================
  // Componer el texto en la franja (texto centrado) y
  // fijar la fila donde se imprimirá
  //
  // `height` es el alto de la franja en px y `printY` la
  // fila de pantalla donde print() la vuelca (de ahí el
  // nombre: se fija una vez al componer y print() ya no lo
  // necesita). `size` es el factor de escala de la fuente.
  //
  // Calcula el límite de caracteres según el tamaño
  // (ancho / (6 * size)) y trunca silenciosamente si el
  // texto excede el límite. El texto se compone en un
  // canvas auxiliar y luego se extraen las columnas.
  // ========================================================

  void setTexto(const char* text, uint8_t height, int16_t printY, uint8_t size = 2);

  // ========================================================
  // Animación lateral (arranca desde el borde, fuera de
  // pantalla)
  // ========================================================

  // `rightToLeft` = true (1): la franja entra por la derecha
  // y se desplaza hacia la izquierda. false (0): entra por
  // la izquierda y se desplaza hacia la derecha.
  void startSlide(bool rightToLeft);

  // ========================================================
  // Actualizar (avanza la animación lateral: 1 px por
  // ANIM_TICK ms, arrancada por startSlide())
  // ========================================================

  // true = la animación sigue su curso (la franja aún entra
  // en pantalla). false = la animación finalizó (la franja
  // quedó centrada); las llamadas siguientes devuelven
  // false hasta que begin()/setTexto()/startSlide() la
  // reinicien.
  bool update();

  // ========================================================
  // Dibujar (rellena la banda con el fondo de la franja en la
  // fila fijada por setTexto() y vuelca encima el texto).
  //
  // Solo repinta mientras hay algo nuevo: al terminar la
  // animación se pinta el frame final y la bandera _done
  // queda puesta, así que las llamadas siguientes no
  // repintan nada hasta que begin()/setTexto()/startSlide()
  // la bajen.
  // ========================================================

  // true = el texto se puede seguir animando (la franja aún
  // entra en pantalla). false = la animación finalizó y la
  // franja ya está pintada en su sitio.
  bool print();

  // ========================================================
  // Acceso a la franja (_strip)
  // ========================================================

  // Lectura de un byte de la franja (cada byte = 1 columna de 8 px)
  // rowBytes: fila en bytes (0..(STRIP_H/8)-1), col: columna (0..STRIP_W-1)
  int8_t getStripByte(uint8_t rowBytes, uint16_t col) const;

  // Escritura de un byte de la franja (cada byte = 1 columna de 8 px)
  // rowBytes: fila en bytes (0..(STRIP_H/8)-1), col: columna (0..STRIP_W-1)
  void setStripByte(uint8_t rowBytes, uint16_t col, int8_t value);

private:
  // ========================================================
  // Geometría y tiempos
  // ========================================================

  // Ancho de la franja (ancho de pantalla)
  static constexpr uint8_t STRIP_W = Config::Screen::WIDTH;

  // Alto máximo de la franja (texto 18x24)
  static constexpr uint8_t STRIP_H = 24;

  // Avance de 1 px cada ANIM_TICK ms (acumulador por tiempo)
  static constexpr uint32_t ANIM_TICK = 4;

  // ========================================================
  // Estado interno
  // ========================================================

  int8_t _strip[STRIP_H / 8][STRIP_W];  // franja: cada byte = 1 columna de 8 px
  uint8_t _height;                      // alto actual de la franja en px
  int16_t _printY;                      // fila de pantalla donde se vuelca la franja
  bool _rightToLeft;                    // true: entra por la derecha; false: por la izquierda
  int16_t _slideX;                      // borde izquierdo de la franja en pantalla
  bool _done;                           // true: _slideX llegó a 0 y ese frame ya se pintó
  Ticker _ticker;                       // avance de 1 px por ANIM_TICK ms
};

#endif

// ====================================================================================
// Fin
// ====================================================================================
