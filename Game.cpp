#include "Game.h"
#include "Globals.h"
#include "Timer.h"
#include "Blink.h"

#include "Sprite.h"

#include <stdio.h>
#include <string.h>
#include <esp_random.h>

// ========================================================
// Constructor (GAME_OVER = "sin partida en curso": begin(false)
// con este estado arranca una nueva)
// ========================================================

Game::Game()
  : _state(State::START),
    _done(false),
    _clear(true),
    _countFood(0),
    _blinkChanged(false),
    // _lastCountdown(0),
    // _redrawHeader(true),
    // _dirtyBoard(true),
    // _overlayHidden(false),
    // _lastCount(0xFF),
    // _difficulty(Config::Difficulty::DEFAULT_LEVEL),
    // _moveDelay(0),
    // _score(0),
    // _newBest(false),
    // _celeSfx(0),
    // _countdown(),
    // _step(),
    // _cele(),
    _moveSnake(false),
    _ateFood(false),
    _ateSpecialFood(false),
    _timer(10 * 1000),
    _win(false),
    _snake(),
    _specialFood(),
    _food() {}

// ========================================================
// Inicialización (al entrar en la ventana)
//
// `newGame` = true (NEW): reinicia la partida y arranca el
// conteo regresivo 3-2-1. `false` (CONTINUE): reanuda la
// partida anterior en pausa (el tablero se conserva) o
// arranca una nueva si no hay partida en curso (GAME_OVER).
// ========================================================

void Game::begin(bool newGame) {
  _done = false;
  _clear = true;
  // _redrawHeader = true;
  // _dirtyBoard = true;
  _snake.clearPending();  // ningún giro pendiente al entrar
  // _newBest = false;      // el festejo de récord lo decide die() en cada muerte
  // _celeSfx = 0;
  _win = false;
  speedFor(storage.difficulty());
  _state = State::PAUSE;

  if (!newGame)  return;

  _ateFood = false;
  _moveSnake = false;
  _ateSpecialFood = false;

  // Nueva partida (velocidad según la dificultad actual del menú)
  _snake.reset();

  randomSeed(esp_random() ^ (uint32_t)nowMs());
  _specialFood.spawn(Food::Type::SPECIAL, *this);
  _food.spawn(Food::Type::NORMAL, *this);
  _specialFood.clear();

  _state = State::START;
  _countFood = 0;
  _score = 0;

  _blink.start();
  _timer = Ticker(_moveDelay);

  _countdown = COUNT;
  _blinkChanged = false;
  // _lastCountdown = 0;

  // reset();
  // Serial.println("Snake II: nueva partida");
  // } else // {
  // Reanudar la partida anterior: queda en pausa, el jugador
  // la retoma con ACTION_RIGHT (o ACTION_LEFT)
  // _state = State::PAUSE;
  // Serial.println("Snake II: continuar partida");
  // }
}

// ========================================================
// Dificultad (la aplica el Engine desde el valor del menú)
// ========================================================

void Game::setDifficulty(uint8_t level) {
  // if (level < Config::Difficulty::MIN_LEVEL) level = Config::Difficulty::MIN_LEVEL;
  // if (level > Config::Difficulty::MAX_LEVEL) level = Config::Difficulty::MAX_LEVEL;
  // _difficulty = level;

  // // La dificultad se aplica EN CALIENTE: si hay una partida en curso
  // // (PLAY o PAUSE) su velocidad (_moveDelay) se actualiza al instante
  // // con el nuevo nivel; la próxima partida nueva la vuelve a derivar
  // // en reset().
  // _moveDelay = speedFor(_difficulty);
}

// ========================================================
// Reinicio de partida (serpiente, alimento y puntaje)
// ========================================================

void Game::reset() {
  // // Semilla del generador aleatorio (lo consume Food::spawn): esp_random()
  // // es el RNG de hardware del ESP32 (entropía real, no predecible ni de 32
  // // bits fijos como micros()); se combina con el reloj (nowMs) por defensa.
  // randomSeed(esp_random() ^ (uint32_t)nowMs());

  // _score = 0;               // el récord vive en Storage (storage.bestScore())
  // _overlayHidden = false;   // el conteo arranca con el primer dígito visible
  // _lastCount = 0xFF;        // ningún dígito del conteo mostrado aún

  // _snake.reset();

  // _moveDelay = speedFor(_difficulty);
  // _countdown.start();

  // // Alimento en una celda libre (al arrancar siempre hay celdas)
  // _food.spawn(Food::Type::NORMAL, *this);
  // // El conteo regresivo 3-2-1 suena solo con sus propios pitidos (SFX_TICK,
  // // uno por dígito, en print()): ya no hay jingle SFX_START al entrar a START.
}

// ========================================================
// Entrar a PLAY (fin del conteo regresivo o de la pausa)
// ========================================================

void Game::startPlay() {
  _state = State::PLAY;
  _timer.start();
  _clear = true;
  // _state = State::PAUSE;
}

// ========================================================
// Actualizar (consume los eventos de botones leídos en loop())
// ========================================================

void Game::update() {
  if (_done) return;

  switch (_state) {
    case State::START:
      {
        // Conteo regresivo 3-2-1: se puede pre-girar la cabeza;
        // se entra a PLAY al confirmar con ACTION_RIGHT o al
        // agotarse el conteo (COUNTDOWN_MS). Al arrancar la partida
        // (después del "1") suena el jingle GO! (SFX_START).
        // handleTurn();
        if (buttons.pressed(Buttons::ACTION_RIGHT)) {
          sound.play(Sound::SFX_START);
          startPlay();
        } else {
          _blinkChanged = _blink.changed(PERIOD, OFF);
          if (_blinkChanged && !_blink.isVisible(PERIOD, OFF))
            if (_countdown == 1) {
              sound.play(Sound::SFX_START);
              startPlay();
            } else
              _countdown -= 1;
          // } else {
          //   _updateCountdown = _blink.changed(PERIOD, OFF) && !_blink.isVisible(PERIOD, OFF);
          //   if (_countdown == 1) {
          //     sound.play(Sound::SFX_START);
          //     startPlay();
          //   } else
          //     _countdown -= 1;
        }
        break;
      }

    case State::PLAY:
      {
        if (buttons.pressed(Buttons::MOVE_UP)) {
          if (_snake.turn(Snake::Dir::UP)) sound.play(Sound::SFX_TURN);
        } else if (buttons.pressed(Buttons::MOVE_RIGHT)) {
          if (_snake.turn(Snake::Dir::RIGHT)) sound.play(Sound::SFX_TURN);
        } else if (buttons.pressed(Buttons::MOVE_DOWN)) {
          if (_snake.turn(Snake::Dir::DOWN)) sound.play(Sound::SFX_TURN);
        } else if (buttons.pressed(Buttons::MOVE_LEFT)) {
          if (_snake.turn(Snake::Dir::LEFT)) sound.play(Sound::SFX_TURN);
        }

        // // Btn2 (ACTION_RIGHT) = Select / Pause: al presionar durante la
        // // partida se congela el tablero y se muestra el panel "PAUSA".
        // // En PAUSE se retoma con el mismo botón (o ACTION_LEFT).
        if (buttons.pressed(Buttons::ACTION_RIGHT)) {
          sound.play(Sound::SFX_PAUSE);
          _state = State::PAUSE;
          break;
        }

        _moveSnake = _timer.consume() > 0;
        if (_moveSnake) step();
        // if (_step.expired(_moveDelay)) {
        //   _step.start();
        //   step();
        // }
        break;
      }

    case State::PAUSE:
      // Reanudar con Btn2 (ACTION_RIGHT, "Select / Pause") o ACTION_LEFT
      if (buttons.pressed(Buttons::ACTION_RIGHT)) {
        sound.play(Sound::SFX_RESUME);
        startPlay();
      }
      break;

    case State::GAME_OVER:
      // Cualquier botón ACTION vuelve al menú
      if (buttons.anyHeld()) _done = true;
      _win = _score > storage.bestScore();
      if (_win) storage.setBestScore(_score);
      break;

    default: break;
  }

  if (_done) return;

  // Botón común "volver al menú" de todas las ventanas (ACTION_UP).
  // En GAME_OVER ya se manejó arriba. La partida NO se pierde:
  // "Continue" la reanuda en pausa.
  if (_state != State::GAME_OVER && buttons.pressed(Buttons::ACTION_UP)) {
    _done = true;
  }
}

// ========================================================
// Cambio de dirección (MOVE): traduce el botón a Snake::Dir
// y delega en Snake::turn (giro pendiente sin reversa directa).
// El SFX_TURN solo suena si el giro fue aceptado (turn() true).
// ========================================================

void Game::handleTurn() {
  // if (buttons.pressed(Buttons::MOVE_UP)) {
  //   if (_snake.turn(Snake::Dir::UP)) sound.play(Sound::SFX_TURN);
  // } else if (buttons.pressed(Buttons::MOVE_RIGHT)) {
  //   if (_snake.turn(Snake::Dir::RIGHT)) sound.play(Sound::SFX_TURN);
  // } else if (buttons.pressed(Buttons::MOVE_DOWN)) {
  //   if (_snake.turn(Snake::Dir::DOWN)) sound.play(Sound::SFX_TURN);
  // } else if (buttons.pressed(Buttons::MOVE_LEFT)) {
  //   if (_snake.turn(Snake::Dir::LEFT)) sound.play(Sound::SFX_TURN);
  // }
}

// ========================================================
// Un paso del tablero (coordinación): la serpiente resuelve
// el movimiento/colisión/comer, Game maneja el resultado:
//   - DIED  -> die() (GAME OVER)
//   - ATE   -> puntaje (+difficultad), SFX_EAT, regenerar alimento
//              (si no hay celda libre: die())
//   - MOVED -> nada, solo repintar
// ========================================================

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
    // // El valor de la comida es el nivel de dificultad actual: como la
    // // dificultad puede cambiar en caliente, el puntaje suma el `_difficulty`
    // // del momento de comer (no el de arranque).
    // _score += _difficulty;
    // _redrawHeader = true;
    sound.play(Sound::SFX_EAT);

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

void Game::spawnFood(Food& food) {
  if (food.spawn(food.type(), *this)) return;

  _snake.reset();
  if (storage.difficulty() < Config::Difficulty::MAX_LEVEL) {
    storage.setBestScore(storage.difficulty() + 1);
    _clear = true;
  } else {
    die();
    return;
  }
  food.spawn(food.type(), *this);
}

// ========================================================
// Fin de partida (colisión o tablero lleno)
// ========================================================

void Game::die() {
  _state = State::GAME_OVER;
  _win = _score > storage.bestScore();
  if (_win) storage.setBestScore(_score);
  _done = true;

  // // El récord solo se verifica/actualiza al terminar en GAME OVER
  // // (no durante la partida): el "Best" del menú refleja partidas
  // // terminadas, no puntajes en curso. El récord vive en Storage.
  // // Al establecerse un NUEVO récord se activa el festejo (ciclo
  // // "GAME OVER"/"BUT"/"YOU ARE"/"THE BEST" en print(), con
  // // SFX_NEW_BEST solo la primera vez que aparece "THE BEST").
  // _win = _score > storage.bestScore();
  // if (_win) storage.setBestScore(_score);
  // // _cele.start();
  // // _celeSfx = 0;
  // // _redrawHeader = true;
  // // sound.play(Sound::SFX_GAME_OVER);
  // // _dirtyBoard = true;
  // _done = true;
}

// ========================================================
// ¿Una celda está ocupada por la serpiente? Lo consulta Food
// (al colocar el alimento en una celda libre). Delega en Snake.
// ========================================================

bool Game::occupied(uint8_t x, uint8_t y) const {
  return _snake.occupied(x, y) || _food.occupied(x, y) || _specialFood.occupied(x, y);
}

// ========================================================
// Dibujar un sprite en la celda (x, y): cada píxel del sprite
// 4x4 se dibuja como un bloque SPRITE_SCALE x SPRITE_SCALE px
// (2x2: completa la celda de Config::Screen::CELL px = 8x8).
// ========================================================

void Game::drawSprite(Sprite::Part part, uint8_t x, uint8_t y) {
  // int16_t baseX = (int16_t)x * Config::Screen::CELL;
  // int16_t baseY = Config::Screen::BODY_TOP + (int16_t)y * Config::Screen::CELL;

  // for (uint8_t i = 0; i < Sprite::SIZE; i++) {
  //   for (uint8_t j = 0; j < Sprite::SIZE; j++) {
  //     if (Sprite::pixel(part, j, i)) {
  //       display.fillRect(baseX + SPRITE_SCALE * (int16_t)j,
  //                        baseY + SPRITE_SCALE * (int16_t)i,
  //                        SPRITE_SCALE, SPRITE_SCALE, true);
  //     }
  //   }
  // }
}

// ========================================================
// Dibujar toda la serpiente (de la cola a la cabeza: la
// cabeza queda encima). Cada segmento usa su sprite
// persistente; la cabeza calcula el suyo en cada frame.
// ========================================================

void Game::drawSnake() {
  // uint8_t n = _snake.length();
  // for (uint8_t i = 0; i < n; i++) {
  //   const Snake::Seg& seg = _snake.segment(i);
  //   Sprite::Part part = (i == n - 1)
  //                         ? _snake.headPart(_food.has(), _food.x(), _food.y())
  //                         : seg.part;
  //   drawSprite(part, seg.col, seg.row);
  // }
}

// ========================================================
// Header del juego: puntuación (izq., 12x16) y segundos
// restantes de la comida especial (der., 12x16; por ahora
// valor fijo solo para el layout, en la clase Food)
//
// El ancho del texto se calcula a mano (strlen * Config::Screen::CHAR_W *
// tamaño): Display::getTextWidth() está comentado en Display.h (mismo
// idioma que Menu/MenuSound/Legend).
// ========================================================

// void Game::drawHeader() {
//   // char buf[8];

//   // // Puntuación actual, esquina superior izquierda (NO se mueve)
//   // snprintf(buf, sizeof(buf), "%u", (unsigned)_score);
//   // display.drawText(buf, 0, 0, TEXT_12x16);

//   // // Segundos restantes de la comida especial, a la derecha
//   // snprintf(buf, sizeof(buf), "%u", (unsigned)_food.specialTime());
//   // int16_t w = (int16_t)strlen(buf) * Config::Screen::CHAR_W * TEXT_12x16;
//   // display.drawText(buf, Config::Screen::WIDTH - w, 0, TEXT_12x16);
// }
// void Game::drawTime() {
//   // // if (_food.specialTime() == -1) return;
//   // char time[8];
//   // if (_food.specialTime() == 0) {
//   //   time[0] = ' ';
//   //   time[1] = '\0';
//   // } else
//   // snprintf(time, sizeof(time), " %u", (unsigned)_food.specialTime());
//   // int16_t w = strlen(time) * Config::Screen::CHAR_W * TEXT_12x16;
//   // // if (_food.specialTime() == -1)
//   // // display.drawText(" ", Config::Screen::WIDTH - 1, 0, TEXT_12x16);
//   // // else
//   // display.drawText(time, Config::Screen::WIDTH - w, 0, TEXT_12x16);
// }

void Game::printScore() {
  char score[8];
  snprintf(score, sizeof(score), "%u", (unsigned)_score);
  display.drawText(score, 0, 0, TEXT_12x16);
}

// ========================================================
// Overlay en el Body: banda blanca de lado a lado (fullWidth)
// o cuadro centrado alrededor del texto, en ambos casos con
// texto invertido centrado en el rectángulo.
// ========================================================

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

// ========================================================
// Velocidad por dificultad: lineal con pasos alternados de
// 101/102 ms (102 en niveles 1, 4 y 7: nivel%3 == 1). Los
// 9 saltos suman 912 ms, de 1000 (nivel 1) a 88 (nivel 10,
// Config::Difficulty::MAX_LEVEL): saltos de 102 = (nivel+1)/3.
// El nivel 1 es el más lento (1000 ms por paso) y el 10 el
// más rápido (88 ms).
// ========================================================

void Game::speedFor(uint8_t level) {
  uint8_t jumps = level - 1;       // saltos entre niveles
  uint8_t wide = (level + 1) / 3;  // saltos de 102 (nivel%3 == 1)
  _moveDelay = 1000 - jumps * 101 - wide;
}

// ========================================================
// Primer frame tras begin() (patrón de las ventanas): clear()
// completo + marcar estáticos para que print() los redibuje.
// `forceRedraw` repite el frame completo (repintado tras un
// diagnóstico del reposo).
// ========================================================

void Game::firstPrint(bool forceRedraw) {
  if (!_clear && !forceRedraw) return;

  display.clear();
  _clear = false;

  printScore();
  _food.print();
  _specialFood.print();
  _snake.print(_food, _specialFood);

  if (_state == State::PAUSE || _state == State::START) {
    if (forceRedraw)
      drawOverlay("PAUSA", false, true);
    // else
    //   drawOverlay(" ", false, true);
  }
}

// ========================================================
// Dibujar
//
// Renderizado por zonas (sin clear() global): el clear()
// completo solo al entrar (firstPrint); luego por frame se
// borra/redibuja únicamente lo dinámico:
//   - Header (puntaje/comida especial) solo cuando cambian sus valores
//   - Tablero (alimento + serpiente) solo cuando algo cambió
//     (movimiento, comida, overlay que desaparece)
//   - Overlay (conteo "3-2-1"/"PAUSA"/"GAME OVER") en cada frame
// ========================================================

void Game::print() {
  if (_done) return;

  // Primer frame tras begin(): clear() completo + redibujar todo
  firstPrint();

  _food.print();
  _specialFood.print();  
  // _specialFood.printTime();
  if (_ateFood || _ateSpecialFood) printScore();
  if (_moveSnake) _snake.print(_food, _specialFood);

  // // Header: solo cuando el puntaje o la comida especial cambió
  // if (_redrawHeader) {
  //   display.fillRect(0, 0, Config::Screen::WIDTH, Config::Screen::BODY_TOP, false);
  //   drawHeader();
  //   _redrawHeader = false;
  // }

  // // Tablero: solo cuando algo cambió
  // if (_dirtyBoard) {
  //   display.fillRect(0, Config::Screen::BODY_TOP, Config::Screen::WIDTH,
  //                    Config::Screen::HEIGHT - Config::Screen::BODY_TOP, false);
  //   if (_food.has()) _food.draw();
  //   drawSnake();
  //   _dirtyBoard = false;
  // }

  // // Overlay según el estado
  switch (_state) {
    case State::START:
      // {
      if (_blinkChanged) {
        if (_blink.isVisible(PERIOD, OFF)) {
          sound.play(Sound::SFX_TICK);
          char number[2];
          number[0] = (char)('0' + _countdown);
          number[1] = '\0';
          drawOverlay(number, false, true);
        } else {
          drawOverlay(" ", false, false);
          _clear = true;
          firstPrint();
        }
      }
      // drawOverlay("X", false);
      // if (_blink.changed(PERIOD, OFF))
      //   sound.play(Sound::SFX_TICK);
      // drawOverlay("A", false);
      // if (_blink.isVisible(PERIOD, OFF))
      // drawOverlay("B", false);
      // if (_blink.changed(PERIOD, OFF) && _blink.isVisible(PERIOD, OFF))
      // drawOverlay("C", false);

      // if (_blink.changed(PERIOD, OFF)) {
      // if (_blink.isVisible(PERIOD, OFF)) {
      // if (_blink.changed(PERIOD, OFF) && _blink.isVisible(PERIOD, OFF)) {
      // if (_updateCountdown) {
      // if (_lastCountdown != _countdown) {
      // if (_blinkChanged && _blink.isVisible(PERIOD, OFF)) {
      //   sound.play(Sound::SFX_TICK);
      //   char number[2];
      //   number[0] = (char)('0' + _countdown);
      //   number[1] = '\0';
      //   drawOverlay(number, false, true);
      // }

      // if (_blinkChanged && !_blink.isVisible(PERIOD, OFF)) {
      //   drawOverlay(" ", false, true);
      // }

      // // Conteo regresivo 3-2-1: un dígito por segundo (COUNTDOWN_MS/3).
      // // Al final de cada fracción el número (y su cuadro) desaparece antes
      // // de que aparezca el siguiente: el cambio es como un parpadeo
      // // (COUNT_HIDE_MS ocultos por dígito; al ocultarlo se restaura el
      // // tablero que hay debajo del cuadro).
      // uint32_t elapsed = (uint32_t)_countdown.elapsed();
      // uint32_t seg = COUNTDOWN_MS / 3;
      // uint32_t pos = elapsed % seg;   // posición dentro del dígito actual
      // uint32_t done = elapsed / seg;  // cuántos dígitos se completaron
      // uint8_t n = (done >= 3) ? 0 : (uint8_t)(3 - done);

      // // Pitido del conteo: un SFX_TICK por dígito, al cambiar el mostrado
      // // (incluido el "3" inicial: _lastCount arranca en 0xFF)
      // if (n != _lastCount) {
      //   _lastCount = n;
      //   sound.play(Sound::SFX_TICK);
      // }

      // if (pos >= seg - COUNT_HIDE_MS) {
      //   // Fase de parpadeo: el dígito y su cuadro desaparecen. Se marca
      //   // el tablero para borrar la zona del overlay una sola vez.
      //   if (!_overlayHidden) {
      //     _overlayHidden = true;
      //     _dirtyBoard = true;
      //   }
      // } else {
      //   _overlayHidden = false;
      //   char buf[2];
      //   buf[0] = (char)('0' + n);
      //   buf[1] = '\0';
      //   drawOverlay(buf);
      // }
      break;
      // }

    case State::PAUSE: drawOverlay("PAUSA", true, true); break;
    // case State::GAME_OVER:
    //   // {
    //   //   if (_newBest) {
    //   //     // Festejo de nuevo récord: ciclo "GAME OVER" -> "BUT" -> "YOU ARE" ->
    //   //     // "THE BEST" (NEW_BEST_SIGN_MS cada uno) hasta que se presiona
    //   //     // un botón. La fanfarria (SFX_NEW_BEST) suena solo la primera vez
    //   //     // que aparece el letrero "THE BEST" (_celeSfx; el "GAME OVER" ya sonó
    //   //     // en die() con SFX_GAME_OVER).
    //   //     uint8_t phase = (uint8_t)(_cele.elapsed() / NEW_BEST_SIGN_MS % 4);
    //   //     switch (phase) {
    //   //       case 0:
    //   //         drawOverlay("GAME OVER", true);
    //   //         break;
    //   //       case 1:
    //   //         drawOverlay("BUT", true);
    //   //         break;
    //   //       case 2:
    //   //         drawOverlay("YOU ARE", true);
    //   //         break;
    //   //       default:
    //   //         if (!(_celeSfx & 0x01)) { _celeSfx |= 0x01; sound.play(Sound::SFX_NEW_BEST); }
    //   //         drawOverlay("THE BEST", true);
    //   //         break;
    //   //     }
    //   //     break;
    //   //   }
    //   // sound.play(Sound::SFX_GAME_OVER);
    //   drawOverlay("GAME OVER", true, true);
    //   break;
      // }

    default: break;
  }
}

// ========================================================
// Salida (true = pidió volver al menú)
// ========================================================

bool Game::done() const {
  return _done;
}

// ========================================================
// ¿La partida terminó en GAME OVER? (comparar el estado
// permite distinguir la salida tras un Game Over, que deja
// la selección del menú en "New", de la salida con la
// partida en curso, que la deja en "Continue")
// ========================================================

bool Game::isGameOver() const {
  return _state == State::GAME_OVER;
}

// ========================================================
// Puntaje
// ========================================================

uint16_t Game::score() const {
  return _score;
}

Game::State Game::state() const {
  return _state;
}

bool Game::win() const {
  return _win;
}

// ====================================================================================
// Fin
// ====================================================================================
