#ifndef ENGINE_H
#define ENGINE_H
#include "src/windows/Boot.h"
#include "src/windows/MenuCredits.h"
#include "src/windows/MenuDifficulty.h"
#include "src/windows/game/Game.h"
#include "src/windows/MenuSound.h"
#include "src/windows/MenuScale.h"
#include "src/windows/Menu.h"
#include "src/windows/Legend.h"

// ========================================================
// Engine — máquina de estados: ventanas y flujo
// ========================================================

class Engine {
public:
  // Funciones comunes de las ventanas
  Engine();
  void begin();
  void update();
  void print();

  // Funciones locales
  bool inGame() const;
  bool inCredits() const;
  bool sleepBlocked() const;

private:
  // Constantes
  enum class State : uint8_t {
    BOOT = 0,
    LEGEND,
    MENU,
    MENU_SOUND,
    MENU_SCALE,
    NEW,
    CONTINUE,
    MENU_DIFFICULTY,
    MENU_CREDITS
  };

  // Variables locales
  State _state;
  Boot _boot;
  Legend _legend;
  Menu _menu;
  MenuSound _menuSound;
  MenuScale _menuScale;
  MenuCredits _menuCredits;
  MenuDifficulty _menuDifficulty;
  Game _game;

  // Funciones locales
  void changeState(State newState);
};

// ========================================================
// Fin
// ========================================================

#endif