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
// Engine — despachador de ventanas
//
// POSEE las ventanas: Boot, Menu, MenuCredits, MenuDifficulty,
// MenuSound, Game y Legend son miembros propios (no globales,
// no anidadas entre sí). Cada una es una clase independiente
// con el patrón begin()/update()/print()/done() y usa los
// servicios globales (Display, Buttons, Sound — ver Globals.h)
// directamente.
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
// Todas las ventanas están activas y Engine las posee y despacha:
// el Menu abre Game (OPT_NEW/OPT_CONTINUE), MenuCredits
// (OPT_CREDITS), MenuDifficulty (OPT_DIFFICULTY) y MenuSound
// (OPT_SOUND). Al volver de Game, la sincronización del récord y
// de la opción "Continue" en el menú sigue pendiente (los métodos
// Menu::setBestScore/setContinueAvailable/setSelected aún están
// comentados), así que changeState(MENU) usa un placeholder.
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

  bool inGame() const;
  bool inCredits() const;

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