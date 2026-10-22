#ifndef CONFIG_H
#define CONFIG_H
#include <Arduino.h>

namespace Config {

  // ========================================================
  // Constantes
  // ========================================================

  // Pines de la placa (orden del enum Buttons::Button: MOVE_UP..ACTION_LEFT).
  namespace Pin {
    constexpr int8_t MOVE_UP    = 2;
    constexpr int8_t MOVE_RIGHT = 1;
    constexpr int8_t MOVE_DOWN  = 42;
    constexpr int8_t MOVE_LEFT  = 41;

    constexpr int8_t ACTION_UP    = 38;
    constexpr int8_t ACTION_RIGHT = 40;
    constexpr int8_t ACTION_DOWN  = 39;
    constexpr int8_t ACTION_LEFT  = 47;

    constexpr uint8_t BUZZER   = 14;  // zumbador
    constexpr uint8_t OLED_SDA = 8;   // I2C: datos
    constexpr uint8_t OLED_SCL = 9;   // I2C: reloj
  }

  // Identidad del firmware (mostrada en el menú y en los créditos).
  namespace Version {
    constexpr const char* NAME         = "Snake II";
    constexpr const char* VERSION      = "v1.0.0";
    constexpr const char* RELEASE_DATE = "2026 10 21";
  }

  // Niveles de dificultad admitidos (rango persistible).
  namespace Difficulty {
    constexpr uint8_t MIN_LEVEL     = 1;
    constexpr uint8_t MAX_LEVEL     = 10;
    constexpr uint8_t DEFAULT_LEVEL = 5;
  }

  // ========================================================
  // Constantes comunes de las ventanas
  // ========================================================

  // Geometría del OLED y sus regiones: cabecera, cuerpo y pie.
  namespace Screen {
    constexpr uint8_t ADDRESS = 0x3C; // dirección I2C
    constexpr uint8_t SLEEP_MIN = 3; // Minutos para dormir

    // Tamaño de la pantalla
    constexpr uint8_t WIDTH   = 128;  // px de ancho
    constexpr uint8_t HEIGHT  = 64;   // px de alto

    // Tamaño minimo del texto
    constexpr uint8_t CHAR_W = 6;
    constexpr uint8_t CHAR_H = 8;

    // Altura del titulo
    constexpr uint8_t HEADER_TOP = 0;

    // Espacio del cuerpo, sin pie
    constexpr uint8_t BODY_TOP    = 16;
    constexpr uint8_t BODY_H      = 48;
    constexpr uint8_t BODY_MIDDLE = BODY_TOP + BODY_H / 2;

    // Espacio del pie
    constexpr uint8_t FOOT_H   = 8;
    constexpr uint8_t FOOT_TOP = HEIGHT - FOOT_H;
  }

  // Rejilla del tablero de juego (compartida por Snake y Food).
  namespace Board {
    // Valores de referencia
    constexpr uint8_t CELL  = 8;  // px por celda (8x8) -> SCALE = 2
    constexpr uint8_t SCALE = CELL / 4;  // "píxel" de dibujo dentro de la celda (2x2)

    // Tamaño del tablero
    constexpr uint8_t COLS = Screen::WIDTH / CELL;
    constexpr uint8_t ROWS = Screen::BODY_H / CELL;
    constexpr uint8_t MAX_LENGTH = COLS * ROWS;
  }

  // Banda central compartida por los menús: caja de la opción y zona de valores, diamantes y triangulos.
  namespace MenuStrip {
    // Valores auxiliares
    constexpr uint8_t BODY_H      = (Screen::FOOT_TOP - 2) - Screen::BODY_TOP + 1;
    constexpr uint8_t Y           = (uint8_t)(BODY_H / 2);
    constexpr uint8_t Z           = 2 * Y == BODY_H ? Y : Y - 1;
    constexpr uint8_t BODY_MIDDLE = Screen::BODY_TOP + Z;

    // Espacio para las opcines
    constexpr uint8_t BOX_HEIGHT = 16;
    constexpr uint8_t BOX_TOP    = BODY_MIDDLE - BOX_HEIGHT / 2;

    // Espacio para valores, diamantes y triangulos
    constexpr uint8_t VALUE_TOP    = BODY_MIDDLE + BOX_HEIGHT / 2 + 2;
    constexpr uint8_t VALUE_HEIGHT = (Screen::FOOT_TOP - 2) - VALUE_TOP + 1;

    // Altura de referencia para diamantes y triangulos
    constexpr uint8_t DIAMOND_Y  = VALUE_TOP + (uint8_t)(VALUE_HEIGHT / 2);
    constexpr uint8_t TRIANGLE_Y = Screen::FOOT_TOP - 3;
  }

}

#endif