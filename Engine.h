#ifndef ENGINE_H
#define ENGINE_H

#include "Boot.h"
// Ventanas activas
// #include "MenuCredits.h"
#include "MenuDifficulty.h"
// #include "Game.h"
#include "MenuSound.h"
#include "Menu.h"
#include "Legend.h"

// ========================================================
// Engine — despachador de ventanas
//
// POSEE las ventanas: Boot, Menu, MenuCredits, MenuDifficulty,
// MenuSound, Game y Legend son miembros propios (no globales,
// no anidadas entre sí). Cada una es una clase independiente
// con el patrón begin()/update()/print()/done() y usa los
// servicios globales (Display, Buttons, Sound — ver Globals.h)
// directamente. Durante el aislamiento solo quedan Boot, Legend,
// Menu, MenuSound y MenuDifficulty (ver el bloque AISLADO más abajo).
//
// Solo Engine conoce el estado (State): decide qué ventana se
// ejecuta y cuándo cambiar de estado, y llama al begin() de la
// ventana entrante. Como las ventanas son miembros y nadie más las
// referencia, la regla "una ventana nunca conoce a las demás" queda
// garantizada durante la normalización.
//
// Las ventanas son instancias únicas que persisten entre
// transiciones: sus valores se conservan (a menos que su
// begin() los reinicie al entrar).
//
// ------------------------------------------------------------
// AISLADO (temporal): Engine solo posee y despacha Boot, Legend,
// Menu, MenuSound y MenuDifficulty, para poder iterar sobre el Menu
// sin arrastrar el Game ni el resto de submenus. El flujo queda
// Boot -> Legend -> Menu y ahi se detiene: las transiciones del Menu
// hacia el Game y MenuCredits aun no se pueden abrir porque la
// entrada desde el Menu (Menu::confirm) esta comentada (aunque el
// Engine ya despacha MENU_SOUND y MENU_DIFFICULTY).
// Para revertir, descomentar por bloques los estados, los
// miembros y las funciones comentadas.
// ------------------------------------------------------------
// ========================================================

class Engine {
public:

  // ========================================================
  // Constructor (no recibe nada: los servicios son globales)
  // ========================================================

  Engine();

  // ========================================================
  // Inicialización (estado inicial: Boot). Se llama desde setup().
  // ========================================================

  void begin();

  // ========================================================
  // Actualizar (solo la ventana activa + transiciones)
  // ========================================================

  void update();

  // ========================================================
  // Dibujar (dibuja solo la ventana activa)
  // ========================================================

  void print();

  // ========================================================
  // ¿Se está jugando? (el reposo no aplica en partida)
  // ========================================================

  bool isInGame() const;

  // ========================================================
  // Puntaje máximo (lo conserva el menú entre sesiones)
  // ========================================================

  // void setBestScore(uint16_t value);

private:

  // ========================================================
  // Estado interno: determina qué se ve y a qué ventana se despacha
  //
  // AISLADO: quedan BOOT, LEGEND, MENU, MENU_SOUND y MENU_DIFFICULTY.
  // Los estados del Game y MenuCredits (NEW, CONTINUE, MENU_CREDITS)
  // estan comentados.
  // ========================================================

  enum class State : uint8_t {
     BOOT = 0,
     LEGEND,
     MENU,
     MENU_SOUND,
     MENU_DIFFICULTY
     // NEW,
     // CONTINUE,
     // MENU_CREDITS
   };

  State _state;

  // ========================================================
  // Transición (fija el estado y llama al begin() de la ventana entrante)
  //
  // `beginWindow` = false deja la ventana entrante como está y no
  // llama a su begin(): es lo único que cambia, y solo se usa para
  // volver al Menu desde MenuDifficulty/MenuSound, que ya dejaron
  // repintada la banda de rombos (restoreDiamondBand) y por tanto
  // no necesitan el clear() completo de Menu::begin(). El resto del
  // cableado (p. ej. setDifficulty al entrar en Game) se aplica
  // siempre.
  //
  // AISLADO: la guarda `if (beginWindow)` sigue comentada porque
  // Menu::restoreDiamondBand() aun no se ha restaurado (pendiente en
  // Menu), asi que beginWindow queda sin usar y el compilador avisa.
  // Es inocuo; al revertir desaparece.
  // ========================================================

  void changeState(State newState);

  // ========================================================
  // Ventanas (miembros propios: las posee Engine, ninguna es global)
  //
  // Boot, Legend, Menu, MenuSound y MenuDifficulty estan activas; el
  // Game y MenuCredits aun no se reactivan (comentados).
  // ========================================================

  Legend _legend;
   Menu _menu;
   MenuSound _menuSound;
   MenuDifficulty _menuDifficulty;
   // MenuCredits _menuCredits;
   // Game _game;
};

#endif

// ====================================================================================
// Fin
// ====================================================================================
