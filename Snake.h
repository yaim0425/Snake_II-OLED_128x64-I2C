#ifndef SNAKE_H
#define SNAKE_H

#include <Arduino.h>

#include "Food.h"
#include "Config.h"
#include "Sprite.h"

// ========================================================
// Snake — lógica pura de la serpiente (partes)
//
// Extraída de Game (la ventana del juego): encapsula TODO lo
// que es "la serpiente" sin tocar el hardware:
//   - Buffer circular de segmentos (`Seg body[MAX_LENGTH]`):
//     cada uno guarda su posición, su dirección (`dir`, hacia
//     el segmento siguiente, más cerca de la cabeza) y su
//     sprite persistente (`part`).
//   - Dirección COMMITIDA (`_dir`) y giro pendiente (`_nextDir`):
//     un único giro PENDIENTE, sin cola ni buffer, validado
//     contra la dirección actual (sin reversa directa: 180° se
//     ignora; al girar varias veces entre dos pasos la cabeza no
//     puede volverse sobre la dirección con la que avanzará).
//   - Paso del tablero (`step`): mover la cabeza con wrap en X e
//     Y, colisión con el cuerpo (la cola se libera si no come),
//     comer/crecer y paleta de sprites (BODY/CORNER/BELLY).
//   - Sprites de las partes (cola, cuerpo, curvas, panza) y de la
//     cabeza (boca abierta/cerrada según el alimento adyacente):
//     la parte de cada segmento se deriva de la geometría de sus
//     vecinos y la dirección de la cabeza.
//   - Dibujo propio (`draw`): pinta la serpiente con la Display
//     global y borra la celda de la cola liberada en el último
//     `step()` (repintando sus píxeles en negro), igual que Food
//     dibuja/borra su alimento.
//
// NO depende de Sound ni Food: los efectos y el alimento los
// coordina Game, que llama a `step()` y después a `draw()` (o
// redibuja el tablero entero).
//
// Nota sobre `compactar`: la casilla que la cabeza deja se
// convierte en cuerpo con su sprite persistente, según por qué
// lado entra y sale la tubería (recto BODY_TO_<dir> o esquina
// CORNER_<horizontal>_<vertical>); al comer, la panza (BELLY,
// recta o curva) queda guardada en el segmento y viaja con el
// cuerpo hasta que la cola lo borra.
// ========================================================

class Snake {
public:

  // ========================================================
  // Configuración (geometría del tablero)
  // ========================================================

  static constexpr uint8_t COLS = Config::Board::COLS;               // columnas del tablero (16 celdas de 8 px = 128 px)
  static constexpr uint8_t ROWS = Config::Board::ROWS;               // filas del tablero (6 celdas de 8 px = 48 px, Body)
  static constexpr uint8_t MAX_LENGTH = Config::Board::MAX_LENGTH;   // 96: a lo sumo hay tantas celdas como segmentos

  // ========================================================
  // Direcciones. El orden (UP=1, RIGHT, DOWN, LEFT) coincide
  // con el orden de los sprites por dirección
  // (TAIL_TO_UP/BODY_TO_UP/HEAD_UP_...) para indexarlos con
  // (dir - 1).
  // ========================================================

  enum class Dir : uint8_t {
    NONE = 0, UP = 1, RIGHT, DOWN, LEFT
  };

  // ========================================================
  // Resultado de un paso (para que Game coordine el resto)
  // ========================================================

  enum class Result : uint8_t {
    MOVED,   // la cabeza avanzó sin comer
    ATE_FOOT,     // la cabeza avanzó y comió (creció)
    ATE_SPECIAL,
    DIED     // colisión con el cuerpo: la cabeza NO avanzó
  };

  // ========================================================
  // Segmento del ring buffer
  // ========================================================

  struct Seg {
    uint8_t col;
    uint8_t row;
    Dir dir;                 // dirección hacia el siguiente segmento (más cerca de la cabeza)
    Sprite::Part part;       // sprite persistente del segmento (TAIL, BODY, CORNER o BELLY)
  };

  // ========================================================
  // Constructor (no toca hardware; el tablero se arma en reset())
  // ========================================================

  Snake();

  // ========================================================
  // Reinicio: serpiente inicial horizontal (1,2)..(4,2),
  // cabeza a la derecha
  // ========================================================

  void reset();

  // ========================================================
  // Cancela el giro pendiente (al entrar en la ventana)
  // ========================================================

  void clearPending();

  // ========================================================
  // Giro de la cabeza (MOVE). `direction` queda PENDIENTE
  // (`_nextDir`) si es válido desde la dirección COMMITIDA
  // (`_dir`): para `direction` contraria (180°) se ignora y
  // devuelve `false`. Devuelve `true` si el giro quedó pendiente
  // (Game toca SFX_TURN solo en ese caso).
  // ========================================================

  bool turn(Dir direction);

  // ========================================================
  // Un paso del tablero: aplicar el giro pendiente, mover la
  // cabeza (wrap), detectar colisión, moverse/crecer y derivar
  // los sprites de las partes. `foodX`/`foodY` = celda del
  // alimento (para comer/crecer y para decidir si la cola es
  // bloqueante). Devuelve el resultado para que Game coordine
  // (puntaje, sonido, alimento, GAME OVER).
  // ========================================================

  Result step(Food food, Food specialFood);

  // ========================================================
  // ¿Una celda está ocupada por la serpiente? (lo consulta el
  // alimento para generarse en una celda libre; Food usa
  // Game::occupied, que delega aquí)
  // ========================================================

  bool occupied(uint8_t cellX, uint8_t cellY) const;

  // ========================================================
  // Acceso para dibujar (lo consume Game)
  // ========================================================

  uint8_t length() const;                 // cantidad de segmentos
  const Seg& segment(uint8_t segmentIndex) const; // segmento `segmentIndex` (0 = cola, length()-1 = cabeza)
  Sprite::Part headPart(Food food, Food specialFood) const; // sprite de la cabeza (boca según el alimento adyacente)

  // ========================================================
  // Dibujar: la serpiente se pinta a sí misma (como Food).
  // Pinta todas las partes (la cabeza con la boca según el
  // alimento) y, si el último step() liberó una cola, borra
  // antes esa celda repintando sus píxeles en negro.
  // ========================================================

  void print(Food food, Food specialFood);

private:

  // ========================================================
  // Constantes (renombres de Config y Sprite: el .cpp no
  // referencia esos espacios de nombres nunca)
  // ========================================================

  static constexpr uint8_t CELL         = Config::Board::CELL;     // px por celda del tablero (8)
  // static constexpr uint8_t PART_WIDTH = Sprite::;
  // static constexpr uint8_t PART_HEIGHT = Sprite::HEIGHT;
  // static constexpr uint8_t SIZE = Sprite::SIZE;
  static constexpr uint8_t SIZE         = Sprite::SIZE;             // px del sprite de la serpiente (4)
  // static constexpr uint8_t SIZE         = Sprite::SPRITE_SIZE;             // px del sprite de la serpiente (4)
  static constexpr uint8_t SCALE = Config::Board::SCALE;              // px por lado de cada píxel del sprite (2)
  static constexpr uint8_t BODY_TOP     = Config::Screen::BODY_TOP; // fila superior del tablero (Body)

  static constexpr uint8_t START_COL    = Config::Snake::START_COL;
  static constexpr uint8_t START_ROW    = Config::Snake::START_ROW;
  static constexpr uint8_t START_LENGTH = Config::Snake::START_LENGTH;
  static constexpr uint8_t START_DIR    = Config::Snake::START_DIR;

  // ========================================================
  // Métodos internos
  // ========================================================

  uint8_t slot(uint8_t slotIndex) const;             // índice del ring buffer para el segmento `slotIndex`
  Dir opposite(Dir direction) const;                 // dirección opuesta (RIGHT<->LEFT, UP<->DOWN)
  Sprite::Part bodyPartFor(Dir inDir, Dir outDir) const;  // BODY recto o CORNER al girar
  Sprite::Part bellyPartFor(Dir inDir, Dir outDir) const; // BELLY recto o curvo (al comer)

  // Dibujo de una celda: cada píxel del sprite 4x4 se pinta como un
  // bloque SPRITE_SCALE x SPRITE_SCALE px; `white` = píxel en blanco
  // (dibujar) o en negro (borrar, fondo del Body)
  void drawSprite(Sprite::Part part, uint8_t col, uint8_t row, bool white);

  // ========================================================
  // Estado (ring buffer: los segmentos van de _tailIndex a _headIndex)
  // ========================================================

  Seg _body[MAX_LENGTH];
  uint8_t _headIndex;
  uint8_t _tailIndex;
  uint8_t _length;

  Dir _dir;                // dirección COMMITIDA de la cabeza (la que usará en el
                           // próximo paso); solo cambia en step() al aplicar el giro pendiente
  Dir _nextDir;            // único giro pendiente ("el siguiente"): se valida contra `_dir` (la
                           // cabeza no puede volver sobre la dirección con la que avanzará)
  bool _bellyPending;      // la cabeza está sobre la casilla de la comida: al dejarla se pinta BELLY

  bool _tailDropped;       // el último step() liberó una cola: _droppedSeg espera de ser borrada en draw()
  Seg _droppedSeg;         // cola liberada (celda + sprite con el que estaba pintada)
};

#endif

// ====================================================================================
// Fin
// ====================================================================================