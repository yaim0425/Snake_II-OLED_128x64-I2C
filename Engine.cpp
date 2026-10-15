#include "Engine.h"
#include "Globals.h"

// ========================================================
// Constructor: no recibe nada. Las ventanas (servicios globales
// de por medio, ver Globals.h) son miembros propios y se
// construyen en su orden de declaración.
// ========================================================

Engine::Engine()
  : _state(State::BOOT) {}

// ========================================================
// Inicialización (estado inicial: Boot). Se llama desde setup().
// ========================================================

void Engine::begin() {
  changeState(State::BOOT);
}

// ========================================================
// Actualizar (solo la ventana activa + transiciones)
// ========================================================

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
        case Menu::OPT_SOUND: changeState(State::MENU_SOUND); break;
        case Menu::OPT_DIFFICULTY: changeState(State::MENU_DIFFICULTY); break;
        case Menu::OPT_CREDITS: changeState(State::MENU_CREDITS); break;
        default: changeState(State::LEGEND); break;
      }

      // // Confirmar opción (ACTION_RIGHT) -> cambiar de ventana
      // int8_t select = _menu.confirm();
      // if (select >= 0) {
      //   sound.play(Sound::SFX_CONFIRM);
      //   // AISLADO: los destinos de abajo (Game y submenus) siguen
      //   // desactivados, asi que el Menu no transiciona. Al reactivarlos
      //   // hay que descomentar el switch y volver a habilitar los estados
      //   // en el enum de Engine.h.
      //   // switch (select) {
      //   //   case Menu::OPT_NEW: changeState(State::NEW); break;
      //   //   case Menu::OPT_CONTINUE: changeState(State::CONTINUE); break;
      //   //   case Menu::OPT_DIFFICULTY: changeState(State::MENU_DIFFICULTY); break;
      //   //   case Menu::OPT_SOUND: changeState(State::MENU_SOUND); break;
      //   //   case Menu::OPT_CREDITS: changeState(State::MENU_CREDITS); break;
      //   // }
      // }
      break;

    case State::MENU_SOUND:
      _menuSound.update();
      if (_menuSound.done()) changeState(State::MENU);
      break;
      // _menu.showOptions();
      // changeState(State::MENU);

      // _menu.restoreDiamondBand();  // pendiente: Menu::restoreDiamondBand()
      // sigue comentado en Menu.h/Menu.cpp; por eso changeState(MENU, false)
      // hace igual el _menu.begin() completo (beginWindow se ignora).

      // AISLADO: ventanas del Game y de los demás submenus (MenuSound ya esta
      // activa: ver su case arriba). Se comentan por bloque entero, no linea
      // por linea, para que el revert sea limpio. Al reactivar hay que volver
      // a descomentar el enum de State y los miembros en Engine.h.
      //
      // case State::NEW:
      // case State::CONTINUE:
      //   {
      //
      //     _game.update();
      //     if (_game.done()) {
      //       sound.play(Sound::SFX_BACK);
      //       _menu.setBestScore(_game.bestScore());
      //       // "Continue" solo se habilita si la partida sigue en curso Y el
      //       // jugador tiene puntos (score > 0): al salir sin haber comido no
      //       // hay nada que reanudar. Con GAME OVER o sin puntos las opciones
      //       // son 4 y la selección queda en "New".
      //       bool gameOver = _game.isGameOver();
      //       bool resumable = !gameOver && _game.score() > 0;
      //       _menu.setContinueAvailable(resumable);
      //       _menu.setSelected(resumable ? Menu::OPT_CONTINUE : Menu::OPT_NEW);
      //       changeState(State::MENU);
      //     }
      //     break;
      //   }

    case State::MENU_DIFFICULTY:
      _menuDifficulty.update();
      if (_menuDifficulty.done()) changeState(State::MENU);
      break;

    case State::MENU_CREDITS:
      _menuCredits.update();
      if (_menuCredits.done()) changeState(State::MENU);
      break;
  }
}

// ========================================================
// Dibujar (dibuja solo la ventana activa)
// ========================================================

void Engine::print() {

  // NO hay clear() global: cada ventana limpia la pantalla completa
  // solo la primera vez que se dibuja (tras su begin()) y luego solo
  // borra/redibuja sus zonas dinámicas (ver cada ventana).

  switch (_state) {
    case State::BOOT: _boot.print(); break;
    case State::LEGEND: _legend.print(); break;
    case State::MENU: _menu.print(); break;
    case State::MENU_SOUND: _menuSound.print(); break;
    case State::MENU_DIFFICULTY:
      _menuDifficulty.print();
      break;
    case State::MENU_CREDITS: _menuCredits.print(); break;

      // AISLADO: solo queda Game (y Food, su comida). Al reactivarlo
      // hay que volver a sacar del comentario sus estados y su ventana.
      // case State::NEW:
      // case State::CONTINUE:
      //   _game.print();
      //   break;
  }
}

// ========================================================
// Repintado forzado (tras un diagnóstico del reposo)
//
// El diagnóstico deformó la imagen que el OLED conserva en su RAM, así
// que se fuerza el full frame de la ventana activa: clear() puntual del
// buffer (no de la pantalla física, que se sustituye en el próximo
// display.show()) y primer frame de la ventana. En MenuSound, un
// selector sobre el Menu ya dibujado, se reconstruye primero la base
// del Menu y encima su selector.
// ========================================================

void Engine::repaint() {
  display.clear();

  switch (_state) {
    case State::BOOT: _boot.firstPrint(true); break;
    case State::LEGEND: _legend.firstPrint(true); break;
    case State::MENU: _menu.firstPrint(true); break;

    case State::MENU_SOUND:
      // La base del Menu ya no se va a volver a dibujar por el loop
      // (la ventana activa es el parche): se dibuja aquí mismo.
      _menu.firstPrint(true);
      _menuSound.firstPrint(true);
      break;

    case State::MENU_DIFFICULTY:
      _menu.firstPrint(true);
      _menuDifficulty.firstPrint(true);
      break;

    case State::MENU_CREDITS:
      _menuCredits.firstPrint(true);
      break;
  }
}

// ========================================================
// ¿Se está jugando? (el reposo no aplica en partida)
//
// AISLADO: sin Game activo, siempre false. Al reactivarlo:
//   return _state == State::NEW || _state == State::CONTINUE;
// ========================================================

bool Engine::isInGame() const {
  return false;
}

// ========================================================
// ¿El reposo no aplica en este momento?
//
// En partida y mientras corre el roll de créditos: el conteo de
// inactividad se mantiene a cero y solo empieza cuando el roll
// termina (tarjeta final) o se sale de él.
// ========================================================

bool Engine::sleepBlocked() const {
  return isInGame() ||
  (_state == State::MENU_CREDITS && !_menuCredits.done());
}

// ========================================================
// Puntaje máximo (lo conserva el menú entre sesiones)
// ========================================================

// void Engine::setBestScore(uint16_t value) {
//   _menu.setBestScore(value);
// }

// ========================================================
// Transición (fija el estado y llama al begin() de la ventana entrante)
// ========================================================

void Engine::changeState(State newState) {
  _state = newState;

  switch (_state) {
    case State::BOOT: _boot.begin(); break;
    case State::LEGEND: _legend.begin(); break;
    case State::MENU:
      {
        // beginWindow=false: solo se usa al volver de MenuDifficulty/MenuSound,
        // que ya dejaron repintada la banda de rombos (restoreDiamondBand) y
        // por tanto no necesitan el clear() completo de Menu::begin().
        // if (beginWindow) _menu.begin();

        bool showContinue = false;  // !_game.isGameOver() && _game.score() > 0;
        int8_t selected = _menu.selected() != Menu::OPT_SOUND ? _menu.selected() : Menu::OPT_SOUND;
        // selected = selected == Menu::OPT_SOUND && showContinue ? _menu.OPT_CONTINUE : Menu::OPT_SOUND;
        _menu.begin(showContinue, selected);
        break;
      }

    case State::MENU_SOUND: _menuSound.begin(); break;
    case State::MENU_DIFFICULTY: _menuDifficulty.begin(); break;
    case State::MENU_CREDITS:
      _menuCredits.begin();   // desde el menú: fromGame = false (las variantes de
                              // Game se activarán al integrarlo)
      break;

      // AISLADO: solo queda Game (y Food, su comida). Al reactivarlo
      // hay que volver a sacar del comentario sus estados y su ventana.
      // case State::NEW:
      //   _game.setDifficulty(_menuDifficulty.difficulty());
      //   _game.begin(true);
      //   break;
      // case State::CONTINUE:
      //   _game.setDifficulty(_menuDifficulty.difficulty());
      //   _game.begin(false);
      //   break;
  }
}

// ====================================================================================
// Fin
// ====================================================================================
