#ifndef FOOD_H
#define FOOD_H

#include <Arduino.h>

#include "Config.h"
#include "Sprite.h"
#include "timer.h"

// ========================================================
// Food — alimento del tablero (normal y especial)
//
// Encapsula el estado y la lógica del alimento de la
// serpiente, extraídos de Game:
//   - NORMAL: la comida común, que se dibuja como rombo
//     simétrico centrado en la celda (igual que el rombo
//     seleccionado del menú).
//   - SPECIAL: la comida especial, que se dibuja con el
//     sprite de 8×4 px (Sprite::SPECIAL_FOOD) y tiene un
//     temporizador (`specialTime`, los segundos restantes
//     que el Header muestra a la derecha). Por ahora el
//     temporizador solo es layout (valor fijo).
//
// El alimento se coloca en una celda libre al azar. La
// ocupación de las celdas la responde el tablero
// (Game::occupied): `spawn` recibe la referencia a la
// partida, así Food no conoce el tablero de antemano.
// ========================================================

class Game;  // solo para consultarle qué celdas ocupa la serpiente

class Food {
public:

  // ========================================================
  // Tipo de alimento
  // ========================================================

  enum class Type : uint8_t {
    NORMAL = 0,   // comida común (rombo centrado en la celda)
    SPECIAL       // comida especial (sprite SPECIAL_FOOD + temporizador)
  };

  // ========================================================
  // Temporizador de la comida especial
  // ========================================================

  // Segundos iniciales de la comida especial (por ahora valor
  // fijo: solo el layout del Header)
  static constexpr uint8_t SPECIAL_TIME = Config::Food::SPECIAL_TIME;

  // ========================================================
  // Constructor (recibe la geometría del tablero: COLS x ROWS
  // celdas de `Config::Screen::CELL` px a partir de la fila
  // `top`; dibuja con la Display global — Globals.h)
  // ========================================================

  Food();

  // ========================================================
  // Inicialización: no hay alimento
  // ========================================================

  void begin();
  void lessTime();

  // ========================================================
  // Colocar un alimento de un tipo en una celda libre al
  // azar. La ocupación la responde el tablero (`game.occupied`).
  // Devuelve false si no hay celdas libres (tablero lleno),
  // en cuyo caso queda sin alimento (partida ganada).
  // ========================================================

  bool spawn(Type type, const Game& game);

  // ========================================================
  // Quitar el alimento (no hay)
  // ========================================================

  void clear();

  // ========================================================
  // Acceso
  // ========================================================

  bool has() const;                     // ¿hay alimento en el tablero?
  Type type() const;                    // tipo del alimento actual
  // uint8_t x() const;                    // columna (0..COLS-1)
  // uint8_t y() const;                    // fila (0..ROWS-1)
  // uint8_t specialTime() const;          // segundos restantes de la comida especial
  // void setSpecialTime(uint8_t seconds); // actualiza el temporizador de la especial

  // ========================================================
  // Dibujar según el tipo (rombo NORMAL / sprite SPECIAL)
  // ========================================================

  void print();
  // void clear() const;
  void printTime(bool show = false);
  bool occupied(uint8_t x, uint8_t y) const;
  uint8_t score() const;

private:

  // ========================================================
  // Constantes (renombres de Config y Sprite: el .cpp no
  // referencia esos espacios de nombres nunca)
  // ========================================================

  static constexpr uint8_t CELL        = Config::Board::CELL;      // px por celda del tablero
  static constexpr auto&   SPECIAL_FOOD        = Special::SPECIAL_FOOD;      // bitmap del sprite (formato drawBitmap)
  static constexpr uint8_t SPECIAL_FOOD_W  = Special::SPECIAL_FOOD_W;    // ancho del sprite (px)
  static constexpr uint8_t SPECIAL_FOOD_H = Special::SPECIAL_FOOD_H;    // alto del sprite (px)
  static constexpr uint8_t SCALE = Config::Board::SCALE;

  static constexpr uint8_t CHAR_W = Config::Screen::CHAR_W;
  static constexpr int16_t WIDTH = Config::Screen::WIDTH;

  static constexpr int8_t COLS = Config::Board::COLS;
  static constexpr int8_t ROWS = Config::Board::ROWS;

  static constexpr int8_t BODY_TOP = Config::Screen::BODY_TOP;
  // static constexpr int8_t SPECIAL_TIME = Config::Food::SPECIAL_TIME;

  // ========================================================
  // Estado
  // ========================================================

  uint8_t _cols;      // columnas del tablero
  uint8_t _rows;      // filas del tablero
  uint8_t _top;       // fila superior del tablero (Body)

  Type _type;
  uint8_t _x;
  uint8_t _y;
  bool _has;
  uint8_t _time;

  // Ticker _timer;
  uint8_t _lastTime;
  uint8_t _score;

  // ========================================================
  // Helpers de dibujo
  // ========================================================

  void printNormal(bool show = true) const;   // rombo simétrico centrado en la celda
  void printSpecial(bool show = true) const;  // sprite SPECIAL_FOOD (8×4 px) centrado
};

#endif

// ====================================================================================
// Fin
// ====================================================================================