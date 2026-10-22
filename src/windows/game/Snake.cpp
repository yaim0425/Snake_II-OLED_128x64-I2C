#include "Snake.h"
#include "Food.h"
#include "src/Globals.h"

// ========================================================
// Funciones comunes de las ventanas
// ========================================================

Snake::Snake()
  : _headIndex(0),
    _tailIndex(0),
    _length(0),
    _dir(Draw::DIR_RIGHT),
    _nextDir(Draw::DIR_NONE),
    _bellyPending(false),
    _tailDropped(false),
    _droppedSeg{ 0, 0, Draw::DIR_NONE, Sprite::EMPTY } {}

Snake::Result Snake::step(const Food& food, const Food& specialFood) {
  _tailDropped = false;
  Draw::Direction stepDir = _dir;

  if (_nextDir != Draw::DIR_NONE) {
    stepDir = _nextDir;
    _nextDir = Draw::DIR_NONE;
  }

  const Seg& headSegment = _body[_headIndex];
  int16_t nextX = headSegment.col;
  int16_t nextY = headSegment.row;

  switch (stepDir) {
    case Draw::DIR_UP: nextY = nextY == 0 ? ROWS - 1 : nextY - 1; break;
    case Draw::DIR_RIGHT: nextX = (nextX + 1) % COLS; break;
    case Draw::DIR_DOWN: nextY = nextY == ROWS - 1 ? 0 : nextY + 1; break;
    case Draw::DIR_LEFT: nextX = nextX == 0 ? COLS - 1 : nextX - 1; break;
    default: break;
  }

  bool eat = food.occupied(nextX, nextY) || specialFood.occupied(nextX, nextY);
  uint8_t skip = eat ? 0xFF : _tailIndex;

  for (uint8_t segmentIndex = 0; segmentIndex < _length; segmentIndex++) {
    uint8_t slotIndex = slot(segmentIndex);

    if (slotIndex == skip) continue;

    if (_body[slotIndex].col == nextX && _body[slotIndex].row == nextY) return Result::DIED;
  }

  _dir = stepDir;
  const Seg& oldHead = _body[_headIndex];
  Draw::Direction inDir = oldHead.dir;
  Draw::Direction outDir = stepDir;
  uint8_t newHeadIndex = (_headIndex + 1) % MAX_LENGTH;
  _body[newHeadIndex] = { nextX, nextY, stepDir, Sprite::HEAD_RIGHT_CLOSE };
  _headIndex = newHeadIndex;
  Seg& newBody = _body[(_headIndex + MAX_LENGTH - 1) % MAX_LENGTH];

  if (_bellyPending) {
    newBody.part = bellyPartFor(inDir, outDir);
    _bellyPending = false;
  } else
    newBody.part = bodyPartFor(inDir, outDir);
  newBody.dir = outDir;

  if (!eat) {
    _droppedSeg = _body[_tailIndex];
    _tailDropped = true;
    _tailIndex = (_tailIndex + 1) % MAX_LENGTH;
    Seg& tail = _body[_tailIndex];
    tail.part = (Sprite::Part)(Sprite::TAIL_TO_UP + ((uint8_t)tail.dir - 1));
    return Result::MOVED;
  }

  _length++;
  _bellyPending = true;

  if (specialFood.occupied(nextX, nextY)) return Result::ATE_SPECIAL;
  if (food.occupied(nextX, nextY)) return Result::ATE_FOOT;
  return Result::MOVED;
}

bool Snake::occupied(int16_t cellX, int16_t cellY) const {
  for (uint8_t segmentIndex = 0; segmentIndex < _length; segmentIndex++) {
    const Seg& currentSegment = _body[slot(segmentIndex)];
    if (currentSegment.col == cellX && currentSegment.row == cellY) return true;
  }
  return false;
}

void Snake::print(const Food& food, const Food& specialFood) {
  if (_tailDropped) {
    drawSprite(_droppedSeg.part, _droppedSeg.col, _droppedSeg.row, false);
    _tailDropped = false;
  }

  for (uint8_t i = 0; i < _length; i++) {
    const Seg& seg = _body[slot(i)];
    Sprite::Part part = i == _length - 1 ? headPart(food, specialFood) : seg.part;
    drawSprite(part, seg.col, seg.row, true);
  }
}

// ========================================================
// Fin — Funciones comunes de las ventanas
// ========================================================





// ========================================================
// Funciones locales
// ========================================================

void Snake::reset() {
  _length = START_LENGTH;
  _tailIndex = 0;
  _headIndex = START_LENGTH - 1;
  _dir = (Draw::Direction)START_DIR;
  _nextDir = Draw::DIR_NONE;
  _bellyPending = false;
  _tailDropped = false;

  _body[0] = { START_COL + 1, START_ROW, Draw::DIR_RIGHT, Sprite::TAIL_TO_RIGHT };
  _body[1] = { START_COL + 2, START_ROW, Draw::DIR_RIGHT, Sprite::BODY_TO_RIGHT };
  _body[2] = { START_COL + 3, START_ROW, Draw::DIR_RIGHT, Sprite::BODY_TO_RIGHT };
  _body[3] = { START_COL + 4, START_ROW, Draw::DIR_RIGHT, Sprite::HEAD_RIGHT_CLOSE };
}

void Snake::clearPending() {
  _nextDir = Draw::DIR_NONE;
}

bool Snake::turn(Draw::Direction direction) {
  if (direction == Draw::DIR_NONE || direction == _dir || direction == _nextDir) return false;
  if (_dir == opposite(direction)) return false;
  _nextDir = direction;
  return true;
}

uint8_t Snake::length() const {
  return _length;
}

Sprite::Part Snake::headPart(const Food& food, const Food& specialFood) const {
  uint8_t offset = (uint8_t)_dir - 1;
  const Seg& head = _body[_headIndex];
  int16_t nextX = head.col;
  int16_t nextY = head.row;

  switch (_dir) {
    case Draw::DIR_UP: nextY = nextY == 0 ? ROWS - 1 : nextY - 1; break;
    case Draw::DIR_RIGHT: nextX = (nextX + 1) % COLS; break;
    case Draw::DIR_DOWN: nextY = nextY == ROWS - 1 ? 0 : nextY + 1; break;
    case Draw::DIR_LEFT: nextX = nextX == 0 ? COLS - 1 : nextX - 1; break;
    default: break;
  }

  bool aboutToEat = food.occupied(nextX, nextY) || specialFood.occupied(nextX, nextY);
  uint8_t basePart = aboutToEat ? Sprite::HEAD_UP_OPEN : Sprite::HEAD_UP_CLOSE;
  return (Sprite::Part)(basePart + offset);
}

uint8_t Snake::slot(uint8_t slotIndex) const {
  return (_tailIndex + slotIndex) % MAX_LENGTH;
}

Draw::Direction Snake::opposite(Draw::Direction direction) const {
  switch (direction) {
    case Draw::DIR_UP: return Draw::DIR_DOWN;
    case Draw::DIR_DOWN: return Draw::DIR_UP;
    case Draw::DIR_RIGHT: return Draw::DIR_LEFT;
    case Draw::DIR_LEFT: return Draw::DIR_RIGHT;
    default: return Draw::DIR_NONE;
  }
}

Sprite::Part Snake::bodyPartFor(Draw::Direction inDir, Draw::Direction outDir) const {
  if (inDir == outDir) return (Sprite::Part)(Sprite::BODY_TO_UP + ((uint8_t)outDir - 1));

  Draw::Direction entry = opposite(inDir);
  Draw::Direction horizontal = (entry == Draw::DIR_RIGHT || entry == Draw::DIR_LEFT) ? entry : outDir;
  Draw::Direction vertical = (entry == Draw::DIR_RIGHT || entry == Draw::DIR_LEFT) ? outDir : entry;

  uint8_t basePart = Sprite::CORNER_RIGHT_UP;
  if (horizontal == Draw::DIR_RIGHT) basePart += (vertical == Draw::DIR_UP) ? 0 : 1;
  else basePart += (vertical == Draw::DIR_UP) ? 2 : 3;
  return (Sprite::Part)basePart;
}

Sprite::Part Snake::bellyPartFor(Draw::Direction inDir, Draw::Direction outDir) const {
  if (inDir == outDir) {
    if (outDir == Draw::DIR_UP || outDir == Draw::DIR_RIGHT) return Sprite::BELLY_TO_RIGHT;
    return Sprite::BELLY_TO_LEFT;
  }

  Draw::Direction entry = opposite(inDir);
  Draw::Direction horizontal = (entry == Draw::DIR_RIGHT || entry == Draw::DIR_LEFT) ? entry : outDir;
  Draw::Direction vertical = (entry == Draw::DIR_RIGHT || entry == Draw::DIR_LEFT) ? outDir : entry;

  uint8_t basePart = Sprite::BELLY_RIGHT_UP;
  if (horizontal == Draw::DIR_RIGHT) basePart += (vertical == Draw::DIR_UP) ? 0 : 1;
  else basePart += (vertical == Draw::DIR_UP) ? 2 : 3;
  return (Sprite::Part)basePart;
}

void Snake::drawSprite(Sprite::Part part, int16_t col, int16_t row, bool white) {
  int16_t baseX = col * CELL;
  int16_t baseY = BODY_TOP + row * CELL;

  for (uint8_t i = 0; i < SIZE; i++)
    for (uint8_t j = 0; j < SIZE; j++)
      display.fillRect(
        baseX + SCALE * j, baseY + SCALE * i,
        SCALE, SCALE, white && Sprite::pixel(part, j, i));
}

// ========================================================
// Fin — Funciones locales
// ========================================================
