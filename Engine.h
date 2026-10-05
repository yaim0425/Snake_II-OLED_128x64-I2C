#ifndef ENGINE_H
#define ENGINE_H

#include "Boot.h"
// AISLADO: ventanas desactivadas mientras se trabaja en Menu. Arduino compila
// igual los .cpp de la carpeta, asi que ademas de sacar estos #include hay que
// dejar inertes Game.cpp, Food.cpp, MenuCredits.cpp, MenuDifficulty.cpp y
// MenuSound.cpp (ver Engine.cpp).
// #include "MenuCredits.h"
// #include "MenuDifficulty.h"
// #include "MenuSound.h"
// #include "Game.h"
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
// directamente. Durante el aislamiento solo quedan Boot, Legend
// y Menu (ver el bloque AISLADO más abajo).
//
// Solo Engine conoce el estado (State): decide qué ventana se
// ve (update()/print() despachan a la ventana activa) y, al
// cambiar de estado, llama al begin() de la ventana entrante.
// Como las ventanas son miembros y nadie más las referencia,
// la regla "una ventana nunca conoce a las demás" queda
// garantizada por el compilador.
//
// Las ventanas son instancias únicas que persisten entre
// transiciones: sus valores se conservan (a menos que su
// begin() los reinicie al entrar).
//
// ------------------------------------------------------------
// AISLADO (temporal): Engine solo posee y despacha Boot, Legend
// y Menu, para poder iterar sobre el Menu sin arrastrar el Game
// ni los submenus. El flujo queda Boot -> Legend -> Menu y ahi se
// detiene: las transiciones del Menu hacia sus submenus y hacia
// el Game estan comentadas en Engine.cpp.
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
  // Puntaje máximo (lo conserva el menú entre sesiones)
  // ========================================================

  void setBestScore(uint16_t value);

private:

  // ========================================================
  // Estado interno: determina qué se ve y a qué ventana se despacha
  //
  // AISLADO: quedan BOOT, LEGEND y MENU. Los estados del Game y de
  // los submenus se comentan junto con sus ventanas.
  // ========================================================

  enum class State : uint8_t {
    BOOT = 0,
    LEGEND,
    MENU
    // NEW,
    // CONTINUE,
    // MENU_DIFFICULTY,
    // MENU_SOUND,
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
  // AISLADO: con el Menu desactivado, beginWindow queda sin usar y
  // el compilador avisa. Es inocuo; al revertir desaparece.
  // ========================================================

  void changeState(State newState, bool beginWindow = true);

  // ========================================================
  // Ventanas (miembros propios: las posee Engine, ninguna es global)
  //
  // AISLADO: quedan Boot, Legend y Menu. El Game y los submenus
  // siguen existiendo como clases (sus .h no se compilan), pero
  // Engine ya no los posee ni los despacha.
  // ========================================================

  Boot _boot;
  Legend _legend;
  Menu _menu;
  // MenuCredits _menuCredits;
  // MenuDifficulty _menuDifficulty;
  // MenuSound _menuSound;
  // Game _game;
};

#endif

// ====================================================================================
// Fin
// ====================================================================================