#ifndef ENGINE_H
#define ENGINE_H
#include "Boot.h"
#include "MenuCredits.h"
#include "MenuDifficulty.h"
#include "Game.h"
#include "MenuSound.h"
#include "Menu.h"
#include "Legend.h"

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