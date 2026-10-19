#include "Snake.h"
#include "Food.h"
#include "Globals.h"

// ========================================================
// Constructor (el tablero se arma en reset(); el dibujo usa la
// Display global, como Food)
// ========================================================

Snake::Snake()
  : _headIndex(0),
    _tailIndex(0),
    _length(0),
    _dir(Dir::RIGHT),
    _nextDir(Dir::NONE),
    _bellyPending(false),
    _tailDropped(false),
    _droppedSeg{ 0, 0, Dir::NONE, Sprite::EMPTY } {}

// ========================================================
// Reinicio: serpiente inicial horizontal (START_COL,START_ROW)
// de START_LENGTH segmentos, cabeza en START_DIR
// ========================================================

void Snake::reset() {
  _length = START_LENGTH;
  _tailIndex = 0;
  _headIndex = START_LENGTH - 1;
  _dir = (Dir)START_DIR;
  _nextDir = Dir::NONE;
  _bellyPending = false;  // ninguna casilla pendiente de panza
  _tailDropped = false;   // nada que borrar al arrancar

  // Cada segmento guarda su dirección y su sprite persistente
  _body[0] = { START_COL + 1, START_ROW, Dir::RIGHT, Sprite::TAIL_TO_RIGHT };
  _body[1] = { START_COL + 2, START_ROW, Dir::RIGHT, Sprite::BODY_TO_RIGHT };
  _body[2] = { START_COL + 3, START_ROW, Dir::RIGHT, Sprite::BODY_TO_RIGHT };
  _body[3] = { START_COL + 4, START_ROW, Dir::RIGHT, Sprite::HEAD_RIGHT_CLOSE };
}

// ========================================================
// Cancelar el giro pendiente (al entrar en la ventana)
// ========================================================

void Snake::clearPending() {
  _nextDir = Dir::NONE;
}

// ========================================================
// Giro de la cabeza (MOVE), sin reversa directa
//
// Solo hay "estado actual" (`_dir`) y "siguiente" (`_nextDir`):
// un único giro pendiente, sin cola ni buffer. Un MOVE se
// evalúa SIEMPRE desde la dirección actual de la cabeza
// (`_dir`, la COMMITIDA, la que usará en el próximo paso): desde
// ella solo hay 3 posibilidades —seguir, girar a la izquierda o
// girar a la derecha— y la contraria (180°) se ignora. Si llega
// un MOVE válido, queda como PENDIENTE (`_nextDir`, el último
// válido pisa al anterior) y se aplica recién en el próximo
// `step()`. Así no se produce un GAME OVER espurio por una
// reversa falsa del último MOVE contra la dirección con la que
// la cabeza avanzará realmente.
// ========================================================

bool Snake::turn(Dir direction) {
  if (direction == Dir::NONE || direction == _dir || direction == _nextDir) return false;

  // Prohibir la reversa directa contra la dirección COMMITIDA (_dir),
  // no contra un giro pendiente intermedio (la cabeza no puede volver
  // sobre sí misma respecto a la dirección con la que avanzará)
  // bool invalid = false;
  // invalid = invalid || (direction == Dir::UP && _dir == Dir::DOWN);
  // invalid = invalid || (direction == Dir::DOWN && _dir == Dir::UP);
  // invalid = invalid || (direction == Dir::LEFT && _dir == Dir::RIGHT);
  // invalid = invalid || (direction == Dir::RIGHT && _dir == Dir::LEFT);
  if (_dir == opposite(direction)) return false;

  // Giro aceptado: queda pendiente
  _nextDir = direction;
  return true;
}

// ========================================================
// Un paso del tablero: mover la cabeza (wrap), detectar
// colisión con el propio cuerpo y comer/crecer. `fx`/`fy`
// indican dónde está el alimento (para saber si este paso
// cae sobre él y, por lo tanto, si la cola es bloqueante).
// ========================================================

Snake::Result Snake::step(Food food, Food specialFood) {
  // La cola liberada en el paso anterior ya se pintó (contrato:
  // Game llama a draw() después de cada step())
  _tailDropped = false;

  // Dirección real de este paso: el giro pendiente del último MOVE
  // válido (ya validado contra la dirección COMMITIDA en turn()).
  // Se trabaja con una copia local `stepDir`: `_dir` (la dirección
  // COMMITIDA) SOLO se actualiza si el destino resulta legal, así al
  // colisionar la cabeza conserva su orientación real de movimiento
  // y no aparece dibujada "volteada" hacia el choque (headPart()
  // dibuja la cabeza según `_dir`).
  Dir stepDir = _dir;
  if (_nextDir != Dir::NONE) {
    stepDir = _nextDir;
    _nextDir = Dir::NONE;
  }

  const Seg& headSegment = _body[_headIndex];
  uint8_t nextX = headSegment.col;
  uint8_t nextY = headSegment.row;

  switch (stepDir) {
    case Dir::UP: nextY = nextY == 0 ? ROWS - 1 : nextY - 1; break;
    case Dir::RIGHT: nextX = (nextX + 1) % COLS; break;
    case Dir::DOWN: nextY = nextY == ROWS - 1 ? 0 : nextY + 1; break;
    case Dir::LEFT: nextX = nextX == 0 ? COLS - 1 : nextX - 1; break;
    default: break;
  }

  bool eat = food.occupied(nextX, nextY) || specialFood.occupied(nextX, nextY);

  // Colisión con el cuerpo. Al comer la cola NO se mueve (es bloqueante);
  // sin comer, la celda de la cola se libera y es legal pisarla.
  uint8_t skip = eat ? 0xFF : _tailIndex;
  for (uint8_t segmentIndex = 0; segmentIndex < _length; segmentIndex++) {
    uint8_t slotIndex = slot(segmentIndex);
    if (slotIndex == skip) continue;
    if (_body[slotIndex].col == nextX && _body[slotIndex].row == nextY)
      return Result::DIED;  // la cabeza NO avanza (para GAME OVER lo decide Game)
  }

  // El destino es legal: la cabeza "commitea" la nueva dirección tras
  // el giro (si no se superó la validación, `_dir` quedó intacto y la
  // cabeza sigue apuntando hacia donde realmente viajaba).
  _dir = stepDir;

  // El cuerpo NO se mueve: la casilla que la cabeza deja pasa a ser
  // cuerpo nuevo con su sprite persistente (BODY recto, CORNER si
  // giró, BELLY si era la casilla de la comida recién comida).
  const Seg& oldHead = _body[_headIndex];
  Dir inDir = oldHead.dir;  // con qué dirección llegó la cabeza a esta casilla
  Dir outDir = stepDir;     // con qué dirección se va hacia la nueva casilla

  // La cabeza avanza a la nueva casilla (se agrega la nueva parte)
  uint8_t newHeadIndex = (_headIndex + 1) % MAX_LENGTH;
  _body[newHeadIndex] = { nextX, nextY, stepDir, Sprite::HEAD_RIGHT_CLOSE };
  _headIndex = newHeadIndex;

  // La casilla que dejó la cabeza se convierte en cuerpo (parte persistente)
  Seg& newBody = _body[(_headIndex + MAX_LENGTH - 1) % MAX_LENGTH];

  if (_bellyPending) {
    // Estaba sobre la comida (recién comida): al dejarla se pinta la panza
    newBody.part = bellyPartFor(inDir, outDir);
    _bellyPending = false;
  } else
    newBody.part = bodyPartFor(inDir, outDir);
  newBody.dir = outDir;

  if (!eat) {
    // Se elimina la última parte (cola): se recuerda su celda y su
    // sprite para que draw() borre su celda de la pantalla. El
    // segmento que queda último pasa a ser cola y recibe su sprite
    // de cola apuntando como su dir.
    _droppedSeg = _body[_tailIndex];
    _tailDropped = true;

    _tailIndex = (_tailIndex + 1) % MAX_LENGTH;
    Seg& tail = _body[_tailIndex];
    tail.part = (Sprite::Part)(Sprite::TAIL_TO_UP + ((uint8_t)tail.dir - 1));
    return Result::MOVED;
  }

  // Comió: crece (la cola NO avanza este paso); la panza queda
  // pendiente para pintarse cuando la cabeza deje la casilla.
  _length++;
  _bellyPending = true;

  if(specialFood.occupied(nextX, nextY))
    return Result::ATE_SPECIAL;

    if(food.occupied(nextX, nextY))
  return Result::ATE_FOOT;
}

// ========================================================
// ¿Una celda está ocupada por la serpiente? (lo consulta el
// alimento para generarse en una celda libre)
// ========================================================

bool Snake::occupied(uint8_t cellX, uint8_t cellY) const {
  for (uint8_t segmentIndex = 0; segmentIndex < _length; segmentIndex++) {
    const Seg& currentSegment = _body[slot(segmentIndex)];
    if (currentSegment.col == cellX && currentSegment.row == cellY) return true;
  }
  return false;
}

// ========================================================
// Acceso para dibujar (lo consume Game)
// ========================================================

uint8_t Snake::length() const {
  return _length;
}

const Snake::Seg& Snake::segment(uint8_t segmentIndex) const {
  return _body[slot(segmentIndex)];
}

// ========================================================
// Sprite de la cabeza. La boca se abre una casilla antes de
// llegar a la comida (el alimento está en la próxima celda
// según _dir) y se cierra al colisionar con ella (cuando la
// cabeza está sobre el alimento, ya no apunta a ninguna).
// El orden del enum Dir (UP=1..LEFT=4) coincide con el orden
// de los sprites por dirección (HEAD_UP_..): se indexan con
// (dir - 1).
// ========================================================

Sprite::Part Snake::headPart(Food food, Food specialFood) const {
  uint8_t offset = (uint8_t)_dir - 1;

  // Próxima celda según la dirección actual
  const Seg& head = _body[_headIndex];
  uint8_t nextX = head.col;
  uint8_t nextY = head.row;
  switch (_dir) {
    case Dir::UP: nextY = nextY == 0 ? ROWS - 1 : nextY - 1; break;
    case Dir::RIGHT: nextX = (nextX + 1) % COLS; break;
    case Dir::DOWN: nextY = nextY == ROWS - 1 ? 0 : nextY + 1; break;
    case Dir::LEFT: nextX = nextX == 0 ? COLS - 1 : nextX - 1; break;
    default: break;
  }

  bool aboutToEat = food.occupied(nextX, nextY) || specialFood.occupied(nextX, nextY);
  uint8_t basePart = aboutToEat ? Sprite::HEAD_UP_OPEN : Sprite::HEAD_UP_CLOSE;
  return (Sprite::Part)(basePart + offset);
}

// ========================================================
// Dibujar: la serpiente se pinta a sí misma con la Display
// global (como Food). Primero borra la cola liberada en el
// último step() (si la hubo) y luego pinta todas las partes
// (la cabeza con la boca según el alimento).
// ========================================================

void Snake::print(Food food, Food specialFood) {
  // La cola liberada se borra repintando sus píxeles en negro (el
  // fondo del Body ya es negro)
  if (_tailDropped) {
    drawSprite(_droppedSeg.part, _droppedSeg.col, _droppedSeg.row, false);
    _tailDropped = false;
  }

  // Cuerpo y cabeza (de la cola a la cabeza: la cabeza queda encima)
  for (uint8_t i = 0; i < _length; i++) {
    const Seg& seg = _body[slot(i)];
    Sprite::Part part = i == _length - 1 ? headPart(food, specialFood) : seg.part;
    drawSprite(part, seg.col, seg.row, true);
  }
}

// ========================================================
// Dibujar una celda: cada píxel del sprite 4x4 se pinta como un
// bloque SPRITE_SCALE x SPRITE_SCALE px (2x2: completa la celda
// de CELL px = 8x8). `white` = píxel en blanco (dibujar) o en
// negro (borrar)
// ========================================================

void Snake::drawSprite(Sprite::Part part, uint8_t col, uint8_t row, bool white) {

  int16_t baseX = col * CELL;
  int16_t baseY = BODY_TOP + row * CELL;
  // display.drawBitmap(
  //   baseX, baseY,
  //   Sprite::SPRITES[part], Sprite::SIZE, Sprite::SIZE,
  //   SSD1306_WHITE, SSD1306_BLACK);

  for (uint8_t i = 0; i < SIZE; i++)
    for (uint8_t j = 0; j < SIZE; j++)
      display.fillRect(baseX + SCALE * j, baseY + SCALE * i, SCALE, SCALE, white && Sprite::pixel(part, j, i));
}

// ========================================================
// Índice del ring buffer (segmento `index` contado desde la cola)
// ========================================================

uint8_t Snake::slot(uint8_t slotIndex) const {
  return (_tailIndex + slotIndex) % MAX_LENGTH;
}

// ========================================================
// Dirección opuesta (RIGHT<->LEFT, UP<->DOWN). Se usa para
// hallar el lado de la celda por donde ENTRA la tubería: si la
// cabeza viajaba hacia `d`, el cuerpo anterior viene desde el
// lado opuesto a `d`.
// ========================================================

Snake::Dir Snake::opposite(Dir direction) const {
  switch (direction) {
    case Dir::UP: return Dir::DOWN;
    case Dir::DOWN: return Dir::UP;
    case Dir::RIGHT: return Dir::LEFT;
    case Dir::LEFT: return Dir::RIGHT;
    default: return Dir::NONE;
  }
}

// ========================================================
// Sprite del cuerpo según las direcciones de entrada (in) y
// salida (out): recto (in == out) -> BODY_TO_<dir>; giro
// (in != out) -> CORNER_<lado horizontal>_<lado vertical>
// (el índice de la curva es CORNER_RIGHT_UP(8) +
// desplazamiento: horiz RIGHT -> vert UP +0 / DOWN +1;
// horiz LEFT -> vert UP +2 / DOWN +3).
// ========================================================

Sprite::Part Snake::bodyPartFor(Dir inDir, Dir outDir) const {
  if (inDir == outDir) {
    return (Sprite::Part)(Sprite::BODY_TO_UP + ((uint8_t)outDir - 1));
  }

  // La esquina conecta el lado por el que la tubería ENTRA a la
  // celda (opuesto a la dirección de llegada `inDir`; p. ej. *iba a
  // la izquierda* -> entra por la derecha) y el lado por el que
  // SALE (`outDir`; p. ej. *ahora va a arriba*). Los nombres son
  // esos dos lados: CORNER_<horizontal>_<vertical> (RIGHT_UP si
  // entra por la derecha y sale arriba, o entra por arriba y sale
  // a la derecha).
  Dir entry = opposite(inDir);
  Dir horizontal = (entry == Dir::RIGHT || entry == Dir::LEFT) ? entry : outDir;
  Dir vertical = (entry == Dir::RIGHT || entry == Dir::LEFT) ? outDir : entry;

  uint8_t basePart = Sprite::CORNER_RIGHT_UP;
  if (horizontal == Dir::RIGHT) basePart += (vertical == Dir::UP) ? 0 : 1;
  else basePart += (vertical == Dir::UP) ? 2 : 3;

  return (Sprite::Part)basePart;
}

// ========================================================
// Sprite de la panza (cuando la cabeza deja la casilla donde
// estaba la comida). Recta (in == out) -> BELLY_TO_RIGHT (sirve
// para UP) o BELLY_TO_LEFT (sirve para DOWN); giro (in != out)
// -> BELLY_RIGHT_UP(22) + desplazamiento (igual que CORNER).
// ========================================================

Sprite::Part Snake::bellyPartFor(Dir inDir, Dir outDir) const {
  if (inDir == outDir) {
    // Panza recta: comparte sprite por par de direcciones
    if (outDir == Dir::UP || outDir == Dir::RIGHT)
      return Sprite::BELLY_TO_RIGHT;
    return Sprite::BELLY_TO_LEFT;
  }

  // Giro (igual que CORNER): la esquina conecta el lado por el que
  // la tubería ENTRA (opuesto a `inDir`) y el lado por el que SALE
  // (`outDir`). El nombre es BELLY_<horizontal>_<vertical>.
  Dir entry = opposite(inDir);
  Dir horizontal = (entry == Dir::RIGHT || entry == Dir::LEFT) ? entry : outDir;
  Dir vertical = (entry == Dir::RIGHT || entry == Dir::LEFT) ? outDir : entry;

  uint8_t basePart = Sprite::BELLY_RIGHT_UP;
  if (horizontal == Dir::RIGHT) basePart += (vertical == Dir::UP) ? 0 : 1;
  else basePart += (vertical == Dir::UP) ? 2 : 3;

  return (Sprite::Part)basePart;
}

// ====================================================================================
// Fin
// ====================================================================================