#ifndef DRAW_H
#define DRAW_H

#include <Arduino.h>

#include "Config.h"

// ========================================================
// Draw — primitivas de dibujo reutilizables (SIN estado)
//
// Funciones libres que dibujan formas compuestas sobre el
// `display` global a partir de un centro. No guardan estado:
// cualquier ventana las llama directo, sin instanciar nada.
// Encapsulan lo que antes eran los `toggleTriangle()` /
// `toggleDiamond()` duplicados en Menu y Legend.
// ========================================================

namespace Draw {
    enum Direction : uint8_t {
        UP = 1,
        RIGHT,
        DOWN,
        LEFT
    };
// Triángulo sólido (o borrado si show = false) apuntando en `dir`
// (1 = arriba, 2 = derecha, 3 = abajo, 4 = izquierda), centrado en
// (centerX, centerY). Lado Config::Diamond::SIZE.
void triangle(int8_t dir, int16_t centerX, int16_t centerY, bool show = true);

// Rombo sólido (o borrado si show = false): dos triángulos (arriba
// + abajo) centrados en (centerX, centerY). Lado Config::Diamond::SIZE.
void diamond(int16_t centerX, int16_t centerY, bool show = true);

}

#endif

// ====================================================================================
// Fin
// ====================================================================================