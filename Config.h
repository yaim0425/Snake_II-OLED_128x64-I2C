#pragma once

#include <Arduino.h>

// ========================================================
// Config — constantes compartidas por varias clases o propias
// del hardware/placa. Solo lo que es verdaderamente compartido;
// el resto vive como `static constexpr` dentro de su clase
// (p. ej. ANIM_H en Scroller, NEW_BEST_SIGN_MS en Game).
//
//   - Pin       : pines de la placa (botones, buzzer, I2C).
//   - Screen    : geometría del OLED y sus regiones.
//   - Board     : geometría del tablero de juego (rejilla de
//                 celdas, compartida por Snake/Game/Food).
//   - Difficulty: límites de la dificultad (compartidos por
//                 Menu y Game).
//   - Version   : versión y fecha de release del firmware
//                 (compartidas por las ventanas que las
//                 muestren).
//   - Food      : temporizador de la comida especial.
//
// Al usar constantes con tipo, ámbito (namespace) y constexpr
// no chocan con nombres de librerías ni del core ESP32; por
// eso aquí no hay `#define` para valores (solo serviría para
// lo que necesita el preprocesador, p. ej. #ifdef DEBUG).
// ========================================================

namespace Config {

// ========================================================
// Pines
// ========================================================

namespace Pin {

// Botones (orden del enum Buttons::Button: MOVE_UP..ACTION_LEFT)
constexpr int8_t MOVE_UP = 2;
constexpr int8_t MOVE_RIGHT = 1;
constexpr int8_t MOVE_DOWN = 42;
constexpr int8_t MOVE_LEFT = 41;

constexpr int8_t ACTION_UP = 38;
constexpr int8_t ACTION_RIGHT = 40;
constexpr int8_t ACTION_DOWN = 39;
constexpr int8_t ACTION_LEFT = 47;

constexpr int8_t BUTTONS[8] = {
  MOVE_UP, MOVE_RIGHT, MOVE_DOWN, MOVE_LEFT,
  ACTION_UP, ACTION_RIGHT, ACTION_DOWN, ACTION_LEFT
};

// constexpr int8_t BUTTONS[8] = {
//   2, 1, 42, 41,   // MOVE_UP, MOVE_RIGHT, MOVE_DOWN, MOVE_LEFT
//   38, 40, 39, 47  // ACTION_UP, ACTION_RIGHT, ACTION_DOWN, ACTION_LEFT
// };

constexpr uint8_t BUZZER = 14;   // zumbador
constexpr uint8_t OLED_SDA = 8;  // I2C: datos
constexpr uint8_t OLED_SCL = 9;  // I2C: reloj
}

// ========================================================
// Pantalla OLED (SSD1306)
// ========================================================

namespace Screen {

constexpr uint8_t WIDTH = 128;     // px de ancho
constexpr uint8_t HEIGHT = 64;     // px de alto
constexpr uint8_t CELL = 8;        // px por celda de la rejilla
constexpr uint8_t ADDRESS = 0x3C;  // dirección I2C

// Regiones: Header (0..15) y Body (16..63)
constexpr uint8_t HEADER_TOP = 0;
constexpr uint8_t HEADER_H = 16;

constexpr uint8_t BODY_TOP = 16;
constexpr uint8_t BODY_H = 48;

constexpr uint8_t FOOT_H = 8;
constexpr uint8_t FOOT_TOP = HEIGHT - FOOT_H;  // Punto Y del texto del pie
// constexpr uint8_t FOOT_LINE = HEIGHT - (FOOT_H + 1 + 1);  // línea separadora del pie + 1 px de margen;

// constexpr uint8_t BODY_MIDDLE = BODY_TOP + (BODY_H - FOOT_H - 1) / 2 - 2; // -8  5
// constexpr uint8_t BODY_MIDDLE = BODY_TOP + (BODY_H - FOOT_H - 1) / 2 - 6;
// constexpr uint8_t BODY_MIDDLE = BODY_TOP + (BODY_H - FOOT_H - 1) / 2 + 5;

constexpr uint8_t OPTION_TOP = 16;
constexpr uint8_t OPTION_H = 48;

constexpr uint8_t CHAR_W = 6;
constexpr uint8_t CHAR_H = 8;
}

// ========================================================
// Tablero de juego: rejilla de celdas (compartida por
// Snake/Game/Food)
// ========================================================

namespace Board {
constexpr uint8_t COLS = Screen::WIDTH / Screen::CELL;   // columnas del tablero (16)
constexpr uint8_t ROWS = Screen::BODY_H / Screen::CELL;  // filas del tablero (6)
constexpr uint8_t MAX_LENGTH = COLS * ROWS;              // segmentos máximos (una celda por segmento)
}

// ========================================================
// Dificultad (nivel 1..10), compartida por Menu y Game
// ========================================================



namespace Difficulty {
constexpr uint8_t MIN_LEVEL = 1;
constexpr uint8_t MAX_LEVEL = 10;
constexpr uint8_t DEFAULT_LEVEL = 5;
}

namespace Version {
constexpr char* NAME = "Snake II";
constexpr char* VERSION = "v1.0.0";
constexpr char* RELEASE_DATE = "2026 12 31";
}

namespace Credits {
constexpr char* CREDITS[] = {
  Version::NAME,
  Version::VERSION,
  Version::RELEASE_DATE,
  "",
  "Programmer",
  "OpenCode.AI",
  "YAIM0425",
  "",
  "Reviewer",
  "Claude.AI",
  "ChatGPT.com",
  "YAIM0425",
  "",
  "Director",
  "YAIM0425",
  "",
  "Producer",
  "YAIM0425",
  "",
  "Graphics",
  "YAIM0425",
  "OpenCode.AI",
  "",
  "Sound Effects",
  "OpenCode.AI",
  "",
  "License",
  "Polyform",
  "Noncommercial 1.0.0",
  "Copyright(c) 2026"
};
constexpr uint8_t COUNT = 30;  // derivado: la lista nunca se desalinea
constexpr char* THANKS = "Thanks for playing!";
constexpr uint32_t PAUSE_MS = 4 * 1000;  // pausa con el bloque centrado
constexpr uint32_t PX_MS = 1.5 * 1000;      // ms por px de desplazamiento (~25 px/s)
}

namespace Legend {
// constexpr uint32_t HOLD = 250;    // visible fija antes de parpadear
constexpr uint32_t NEXT = 2 * 1000;   // duración total por rombo (avance lento)
constexpr uint32_t PERIOD = 100;  // período del parpadeo MUY rápido (ms)
constexpr uint8_t OFF = 50;       // % del período en que está oculto
}

namespace Diamond {
constexpr uint8_t SIZE = 3;  // px de un lado del rombo
}

namespace Food {
constexpr uint8_t SPECIAL_TIME = 60;  // segundos iniciales de la comida especial
}

namespace Button {
constexpr uint32_t DELAY = 400;  // mantener para empezar a repetir (ms)
constexpr uint32_t TICK = 100;   // intervalo de repetición mientras se mantiene (ms)
}

namespace Scroller {
constexpr uint8_t ANIMATION_H = 4;   // ms por px de desplazamiento horizontal
constexpr uint8_t ANIMATION_V = 12;  // ms por px de desplazamiento vertical
constexpr uint8_t MAX_LINES = 12;      // líneas máximas por bloque (array fijo)
}

namespace DefaultTimer {
// constexpr uint32_t HOLD = 500;    // mantener sin navegar para parpadear
constexpr uint32_t PERIOD = 500;  // período completo del parpadeo (ms)
constexpr uint8_t OFF = 20;       // % del período en que está oculto
}

namespace Power {
constexpr uint32_t IDLE_TIMEOUT_MS = 3 * 60 * 1000;  // reposo: sin actividad fuera de partida (ms)
constexpr uint32_t WAKE_CHECK_MS = 200;          // reposo: período del wake de verificación (ms)
constexpr uint32_t MIN_SLEEP_MS = 10;            // reposo: bajo este tiempo dormido, el light sleep no persistió (ms)
constexpr uint8_t  SPURIOUS_LIMIT = 3;           // reposo: wakes espurios seguidos hasta mostrar el diagnóstico
constexpr uint32_t DIAG_MS = 2 * 1000;           // reposo: duración del aviso de diagnóstico en pantalla (ms)
}

namespace MenuStrip {
static constexpr int16_t TRIANGLE_Y = Screen::FOOT_TOP - 3;

static constexpr int16_t BODY_H = (Screen::FOOT_TOP - 2) - Screen::BODY_TOP + 1;  // Alto del cuerpo (sin el pie)
static constexpr int16_t Y = (int16_t)(BODY_H / 2);                   // Aux mitad del alto del cuerpo
static constexpr int16_t Z = 2 * Y == BODY_H ? Y : Y - 1;             // Mitad del alto del cuerpo
static constexpr int16_t BODY_MIDDLE = Screen::BODY_TOP + Z;          // Posición mitad del alto del cuerpo

static constexpr int16_t BOX_HEIGHT = 16;
static constexpr int16_t BOX_TOP = BODY_MIDDLE - BOX_HEIGHT / 2;

static constexpr int16_t VALUE_TOP = BODY_MIDDLE + BOX_HEIGHT / 2 + 2;
static constexpr int16_t VALUE_HEIGHT = (Screen::FOOT_TOP - 2) - VALUE_TOP + 1;

static constexpr int16_t DIAMOND_Y = VALUE_TOP + (int16_t)(VALUE_HEIGHT / 2);



// static constexpr int16_t TRIANGLE_Y = Screen::FOOT_TOP - 3;

// static constexpr int16_t BOX_HEIGHT = 16;
// static constexpr int16_t BOX_TOP = Screen::BODY_TOP + 8;

// static constexpr int16_t VALUE_TOP = BOX_TOP + BOX_HEIGHT + 2;
// static constexpr int16_t VALUE_HEIGHT = TRIANGLE_Y - VALUE_TOP;

// static constexpr int16_t DIAMOND_Y = VALUE_TOP + ceil(VALUE_HEIGHT/2);
}
}

// ====================================================================================
// Fin
// ====================================================================================