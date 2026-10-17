#ifndef ENGINE_H
#define ENGINE_H

#include "Boot.h"
#include "MenuCredits.h"
#include "MenuDifficulty.h"
// AISLADO: el unico pendiente del aislamiento es Game (y Food, su comida),
// que se reactivara al integrar la partida con el Menu. Arduino compila igual
// los .cpp de la carpeta, asi que ademas de no incluir Game.h aqui, Game.cpp y
// Food.cpp seguiran inertes (ver Engine.cpp).
#include "Game.h"
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
// directamente. Durante el aislamiento quedan fuera Boot, Legend,
// Menu, MenuSound, MenuDifficulty y MenuCredits (ver el bloque
// AISLADO más abajo); Game sigue aislado.
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
// AISLADO (temporal): el único pendiente del aislamiento es Game
// (con Food), que se reactivará al integrar la partida con el
// Menu. El resto de ventanas (Boot, Legend, Menu, MenuSound,
// MenuDifficulty, MenuCredits) está activo y Engine las posee y
// despacha: las transiciones del Menu hacia Game (OPT_NEW y
// OPT_CONTINUE) siguen comentadas en Engine.cpp, y MenuCredits
// ya se abre desde el Menu (OPT_CREDITS).
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
  // Repintado forzado (tras un diagnóstico del reposo)
  // ========================================================

  void repaint();

  // ========================================================
  // ¿Se está jugando? (el reposo no aplica en partida)
  // ========================================================

  bool isInGame() const;

  // ========================================================
  // ¿El reposo no aplica en este momento? (en partida y mientras
  // corre el roll de créditos, para que el conteo de inactividad
  // empiece cuando acaba)
  // ========================================================

  bool sleepBlocked() const;

  // ========================================================
  // Puntaje máximo (lo conserva el menú entre sesiones)
  // ========================================================

  // void setBestScore(uint16_t value);

private:

  // ========================================================
  // Estado interno: determina qué se ve y a qué ventana se despacha
  //
  // AISLADO: los estados del Game (NEW y CONTINUE) siguen comentados
  // junto con su ventana.
  // ========================================================

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
  // AISLADO: Game sigue fuera (su .h no se incluye y Game.cpp/Food.cpp
  // están inertes); el resto de ventanas está activo.
  // ========================================================

  Boot _boot;
  Legend _legend;
  Menu _menu;
  MenuSound _menuSound;
  MenuCredits _menuCredits;
  MenuDifficulty _menuDifficulty;
  Game _game;
};

#endif

// ====================================================================================
// Fin
// ====================================================================================