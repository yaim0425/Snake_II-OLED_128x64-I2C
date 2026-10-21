#include "Engine.h"

// ========================================================
// Funciones comunes de las ventanas
// ========================================================

Engine::Engine()
  : _state(State::BOOT) {}

void Engine::begin() {
  changeState(State::BOOT);
}

void Engine::update() {
  switch (_state) {
    case State::BOOT:
      _boot.update();
      if (_boot.done()) changeState(State::LEGEND);
      break;

    case State::LEGEND:
      _legend.update();
      if (_legend.done()) changeState(State::MENU);
      break;

    case State::MENU:
      _menu.update();
      if (!_menu.done()) break;

      switch (_menu.selected()) {
        case Menu::OPT_NEW:
          changeState(State::NEW);
          break;

        case Menu::OPT_CONTINUE:
          changeState(State::CONTINUE);
          break;

        case Menu::OPT_DIFFICULTY:
          changeState(State::MENU_DIFFICULTY);
          break;

        case Menu::OPT_SOUND:
          changeState(State::MENU_SOUND);
          break;

        case Menu::OPT_CREDITS:
          changeState(State::MENU_CREDITS);
          break;

        default:
          changeState(State::LEGEND);
          break;
      }
      break;

    case State::MENU_SOUND:
      _menuSound.update();
      if (_menuSound.done()) changeState(State::MENU);
      break;

    case State::NEW:
    case State::CONTINUE:
      _game.update();
      if (!_game.done()) break;
      if (!_game.isGameOver())
        changeState(State::MENU);
      else
        changeState(State::MENU_CREDITS);
      break;

    case State::MENU_DIFFICULTY:
      _menuDifficulty.update();
      if (_menuDifficulty.done()) changeState(State::MENU);
      break;

    case State::MENU_CREDITS:
      _menuCredits.update();
      if (_menuCredits.done()) changeState(State::MENU);
      break;

    default: break;
  }
}

void Engine::print() {
  switch (_state) {
    case State::BOOT:
      _boot.print();
      break;

    case State::LEGEND:
      _legend.print();
      break;

    case State::MENU:
      _menu.print();
      break;

    case State::MENU_SOUND:
      _menuSound.print();
      break;

    case State::MENU_DIFFICULTY:
      _menuDifficulty.print();
      break;

    case State::MENU_CREDITS:
      _menuCredits.print();
      break;

    case State::NEW:
    case State::CONTINUE:
      _game.print();
      break;

    default: break;
  }
}

// ========================================================
// Fin — Funciones comunes de las ventanas
// ========================================================





// ========================================================
// Funciones locales
// ========================================================

bool Engine::inGame() const {
  return (_state == State::NEW || _state == State::CONTINUE) && _game.state() == Game::State::PLAY;
}

bool Engine::inCredits() const {
  return _state == State::MENU_CREDITS && !_menuCredits.card();
}

bool Engine::sleepBlocked() const {
  return inGame() || inCredits();
}

void Engine::changeState(State newState) {

  switch (newState) {
    case State::BOOT:
      _boot.begin();
      break;

    case State::LEGEND:
      _legend.begin();
      break;

    case State::MENU_SOUND:
      _menuSound.begin();
      break;

    case State::MENU_DIFFICULTY:
      _menuDifficulty.begin();
      break;

    case State::MENU:
      _menu.begin(_game.showContinue());
      break;

    case State::MENU_CREDITS:
      {
        bool fromGame = _state == State::NEW || _state == State::CONTINUE;
        _menuCredits.begin(fromGame, _game.win());
        break;
      }

    case State::NEW:
      _game.begin(true);
      break;

    case State::CONTINUE:
      _game.begin(false);
      break;

    default: break;
  }
  _state = newState;
}

// ========================================================
// Fin — Funciones locales
// ========================================================
