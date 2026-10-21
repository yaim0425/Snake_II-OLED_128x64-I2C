#include "Game.h"
#include "Globals.h"
#include "Draw.h"
#include "Timer.h"
#include "Blink.h"
#include <stdio.h>
#include <string.h>
#include <esp_random.h>

// ========================================================
// Funciones comunes de las ventanas
// ========================================================

Game::Game()
  : _done(false),
    _clear(true),
    _blink(),
    _state(State::START),
    _win(false),
    _score(0),
    _timer(10 * 1000),
    _countdown(COUNT),
    _blinkChanged(false),
    _moveSnake(false),
    _ateFood(false),
    _ateSpecialFood(false),
    _snake(),
    _food(Food::Type::NORMAL),
    _specialFood(Food::Type::SPECIAL),
    _countFood(0) {}

void Game::begin(bool newGame) {
  speedFor(storage.difficulty());
  _state = State::PAUSE;
  _snake.clearPending();
  _done = false;
  _clear = true;
  _win = false;

  if (!newGame) return;

  _snake.reset();
  _moveSnake = false;
  _ateFood = false;
  _ateSpecialFood = false;

  _blinkChanged = false;

  randomSeed(esp_random() ^ (uint32_t)nowMs());
  _specialFood.spawn(*this);
  _food.spawn(*this);

  _state = State::START;
  _countFood = 0;
  _score = 0;

  _blink.start();
  _specialFood.clear();
  _countdown = COUNT;
}

void Game::update() {
  if (_done) return;

  switch (_state) {
    case State::START:
      {
        if (buttons.pressed(Buttons::ACTION_RIGHT)) {
          sound.play(Sound::SFX_START, false);
          startPlay();
        } else {
          _blinkChanged = _blink.changed(PERIOD, OFF);

          if (_blinkChanged && !_blink.isVisible(PERIOD, OFF))
            if (_countdown == 1) {
              sound.play(Sound::SFX_START, false);
              startPlay();
            } else
              _countdown -= 1;
        }
        break;
      }

    case State::PLAY:
      {
        if (buttons.pressed(Buttons::MOVE_UP)) {
          if (_snake.turn(Draw::DIR_UP)) sound.play(Sound::SFX_TURN, false);
        } else if (buttons.pressed(Buttons::MOVE_RIGHT)) {
          if (_snake.turn(Draw::DIR_RIGHT)) sound.play(Sound::SFX_TURN, false);
        } else if (buttons.pressed(Buttons::MOVE_DOWN)) {
          if (_snake.turn(Draw::DIR_DOWN)) sound.play(Sound::SFX_TURN, false);
        } else if (buttons.pressed(Buttons::MOVE_LEFT)) {
          if (_snake.turn(Draw::DIR_LEFT)) sound.play(Sound::SFX_TURN, false);
        }

        if (buttons.pressed(Buttons::ACTION_RIGHT)) {
          sound.play(Sound::SFX_PAUSE, false);
          _state = State::PAUSE;
          break;
        }

        _moveSnake = _timer.consume() > 0;

        if (_moveSnake) step();
        break;
      }

    case State::PAUSE:
      if (buttons.pressed(Buttons::ACTION_RIGHT)) {
        sound.play(Sound::SFX_RESUME, false);
        startPlay();
      }
      break;

    default: break;
  }

  if (_done) return;

  if (_state != State::GAME_OVER && buttons.pressed(Buttons::ACTION_UP)) {
    sound.play(Sound::SFX_BACK, false);
    _done = true;
  }
}

void Game::print() {
  if (_done) return;

  firstPrint(false);
  _food.print();
  _specialFood.print();

  if (_ateFood || _ateSpecialFood) printScore();

  if (_moveSnake) _snake.print(_food, _specialFood);

  switch (_state) {
    case State::START:
      if (_blinkChanged) {
        if (_blink.isVisible(PERIOD, OFF)) {
          sound.play(Sound::SFX_TICK, false);
          char number[2];
          number[0] = (char)('0' + _countdown);
          number[1] = '\0';
          drawOverlay(number, false, true);
        } else {
          drawOverlay(" ", false, false);
          _clear = true;
          firstPrint(false);
        }
      }
      break;

    case State::PAUSE:
      drawOverlay("PAUSA", true, true);
      break;

    default:
      break;
  }
}

void Game::firstPrint(bool forceRedraw) {
  if (!_clear && !forceRedraw) return;

  display.clear();
  _clear = false;
  printScore();
  _food.print();
  _specialFood.print();
  _snake.print(_food, _specialFood);

  if (_state == State::PAUSE || _state == State::START) {
    if (forceRedraw) drawOverlay("PAUSA", false, true);
  }
}

bool Game::done() const {
  return _done;
}

bool Game::occupied(int16_t x, int16_t y) const {
  return _snake.occupied(x, y) || _food.occupied(x, y) || _specialFood.occupied(x, y);
}

void Game::step() {
  Snake::Result action = _snake.step(_food, _specialFood);

  if (action == Snake::Result::DIED) {
    die();
    return;
  }

  _specialFood.lessTime();
  _ateFood = action == Snake::Result::ATE_FOOT;
  _ateSpecialFood = action == Snake::Result::ATE_SPECIAL;

  if (_ateFood || _ateSpecialFood) {
    sound.play(Sound::SFX_EAT, false);

    if (_ateFood) {
      _score += _food.score();
      _food.clear();
      spawnFood(_food);
      _countFood++;
    }

    if (_countFood == FOOD_COUNT) {
      spawnFood(_specialFood);
      _countFood = 0;
    }

    if (_ateSpecialFood) {
      _score += _specialFood.score();
      _specialFood.clear();
    }
  }
}

// ========================================================
// Fin — Funciones comunes de las ventanas
// ========================================================





// ========================================================
// Funciones locales
// ========================================================

bool Game::showContinue() const {
  return !isGameOver() && _score > 0;
}

bool Game::isGameOver() const {
  return _state == State::GAME_OVER;
}

Game::State Game::state() const {
  return _state;
}

bool Game::win() const {
  return _win;
}

void Game::speedFor(uint8_t level) {
  if (level < Config::Difficulty::MIN_LEVEL) level = Config::Difficulty::MIN_LEVEL;
  if (level > Config::Difficulty::MAX_LEVEL) level = Config::Difficulty::MAX_LEVEL;

  uint8_t jumps = level - 1;
  uint8_t wide = (level + 1) / 3;
  _timer = Ticker(1000 - jumps * 101 - wide);
}

void Game::startPlay() {
  _state = State::PLAY;
  _timer.start();
  _clear = true;
}

void Game::die() {
  _state = State::GAME_OVER;
  _win = _score > storage.bestScore();

  if (_win) storage.setBestScore(_score);

  _done = true;
}

void Game::drawOverlay(const char* title, bool fullWidth, bool show) {
  int16_t w = strlen(title) * CHAR_W * TEXT_12x16;
  int16_t h = CHAR_H * TEXT_12x16;
  int16_t x = display.centerX(title, TEXT_12x16);
  int16_t y = BODY_MIDDLE - h / 2;

  if (fullWidth)
    display.fillRect(0, y - 2, WIDTH, h + 2, show);
  else
    display.fillRect(x - 2, y - 2, w + 2, h + 2, show);
  display.drawText(title, x, y, TEXT_12x16, false, show);
}

void Game::spawnFood(Food& food) {
  food.clear();

  if (food.spawn(*this)) return;

  _snake.reset();

  if (storage.difficulty() < Config::Difficulty::MAX_LEVEL) {
    storage.setDifficulty(storage.difficulty() + 1);
    speedFor(storage.difficulty());
    _timer.start();
    _clear = true;
  } else {
    die();
    return;
  }

  food.spawn(*this);
}

void Game::printScore() {
  char score[8];
  snprintf(score, sizeof(score), "%u", (unsigned)_score);
  display.drawText(score, 0, 0, TEXT_12x16, true, false);
}

// ========================================================
// Fin — Funciones locales
// ========================================================
