#ifndef MENU_H
#define MENU_H

#include "Config.h"
#include "Timer.h"
#include "Blink.h"
#include "ButtonRepeat.h"

#include "Scroller.h"

class Menu {
public:

  // ========================================================
  // Configuración
  // ========================================================

  enum Option : uint8_t {
    OPT_NEW = 0,
    OPT_CONTINUE,
    OPT_DIFFICULTY,
    OPT_SOUND,
    OPT_CREDITS,

    OPT_COUNT
  };

  // ========================================================
  // Constructor (sin parámetros de servicios: usa los globales
  // Display, Buttons y Sound — Globals.h)
  // ========================================================

  Menu();

  // ========================================================
  // Inicialización
  // ========================================================

  void begin(bool showContinue = false, int8_t selected = OPT_NEW);

  // ========================================================
  // Opciones (cantidad variable)
  // ========================================================

  // void setOptions(const char* const* texts, uint8_t count);

  // ========================================================
  // Opción "Continue" (hay partida en curso que reanudar)
  //
  // true  -> la lista incluye "Continue" (5 opciones).
  // false -> se oculta (4 opciones: New, Difficulty, Sound,
  //          Credits). Es el estado inicial: sin partida no hay
  //          nada que continuar. La selección se mantiene y se
  //          adapta a la nueva cantidad de opciones.
  // ========================================================

  // void setContinueAvailable(bool available);

  // ========================================================
  // Apariencia (título del Header y pie opcional)
  // ========================================================

  // void setTitle(const char* title);
  // void setShowFooter(bool show);

  // ========================================================
  // Selección inicial por OPCIÓN LÓGICA (enum Option, p. ej.
  // OPT_NEW u OPT_CONTINUE) y reinicio de la animación.
  // Internamente se mapea al índice de la lista visible; si la
  // opción no está visible (p. ej. "Continue" oculto) la
  // selección cae a la primera opción (New).
  // ========================================================

  // void setSelected(Option option);

  // ========================================================
  // Actualizar (consume eventos de botones, navega y anima)
  // ========================================================

  void update();

  // ========================================================
  // Dibujar
  // ========================================================

  void print();
  void showOptions();

  // ========================================================
  // Accesos
  // ========================================================

  // void setBestScore(uint16_t value);

  // Devuelve la opción elegida si se confirma (ACTION_RIGHT), o -1
  // int8_t confirm() const;

  // ========================================================
  // Rombos de posición (restauración)
  // ========================================================

  // Las ventanas MenuDifficulty y MenuSound sustituyen la banda de
  // rombos (45..53) por su selector y dejan el resto de la ventana
  // como estaba. Al salir de ellas el Engine llama a esto para que
  // el Menu vuelva a pintar esa franja. A diferencia de begin(), no
  // borra la pantalla: solo la banda.
  // void restoreDiamondBand();
  bool done() const;
  int8_t selected() const;

private:
  void action();
  void firstPrint();
  void blinkOption();
  void nextOption();
  void toggleText(bool show);
  void holdButtons();
  void navigate();
  void toggleDiamond(int8_t diamond, bool show);
  void toggleTriangle(int8_t triangle, bool show);

  // void drawDiamond(int8_t selected, bool focus, bool black);

  // ========================================================
  // Opciones por defecto
  // ========================================================

  static const char* const OPTION[OPT_COUNT];

  // La lista que se muestra depende de si hay partida en curso:
  //   - con "Continue": DEFAULT_OPTIONS (5) opciones
  //   - sin "Continue": DEFAULT_OPTIONS - 1 (4) opciones
  // static constexpr uint8_t DEFAULT_OPTIONS = 5;
  // static const char* const DEFAULT_OPTION_TEXT[DEFAULT_OPTIONS];
  // static const char* const NO_CONTINUE_OPTIONS[DEFAULT_OPTIONS - 1];

  // const char* optionText(int8_t index) const;

  // Mapeo entre el índice de la lista y la opción lógica (enum
  // Option). Con "Continue" el índice coincide con el enum; sin
  // "Continue" el índice 1 pasa a Dificultad, el 2 a Sonido y el
  // 3 a Créditos. optionAt() devuelve la opción lógica de un índice
  // de la lista; indexOfOption() hace lo contrario y devuelve -1 si
  // la opción no está visible (p. ej. OPT_CONTINUE sin partida).
  // Option optionAt(int8_t index) const;
  // int8_t indexOfOption(Option option) const;

  // ========================================================
  // Geometría del menú
  // ========================================================

  static constexpr int16_t WIDTH = Config::Screen::WIDTH;
  static constexpr int16_t FOOT_TOP = Config::Screen::FOOT_TOP;

  static constexpr int16_t TRIANGLE_Y = Config::MenuStrip::TRIANGLE_Y;
  static constexpr int16_t DIAMOND_Y = Config::MenuStrip::DIAMOND_Y;

  // Cuadro de selección: fijo, ancho completo. Con el rombo activo de punta en
  // la 45: 2 filas libres (44..43) y el cuadro desde la fila 3 (42) hacia arriba
  static constexpr int16_t BOX_HEIGHT = Config::MenuStrip::BOX_HEIGHT;
  static constexpr int16_t BOX_TOP = Config::MenuStrip::BOX_TOP;

  static constexpr int16_t VALUE_HEIGHT = Config::MenuStrip::VALUE_HEIGHT;
  static constexpr int16_t VALUE_TOP = Config::MenuStrip::VALUE_TOP;

  // Posición del texto del cuadro (1 px dentro, centrado verticalmente)
  // static constexpr int16_t TEXT_SEL_TOP = 26;

  // Rombos de posición: banda 45..53, apoyada en la línea separadora 54 del pie.
  // static constexpr int16_t DIA_TOP = 45;  // punta superior del rombo (rombo simétrico 45..53)
  // static constexpr uint8_t DIA_SIZE = 8;  // rombo: punta 45, hombros 49, punta inferior 53

  // static constexpr uint32_t HOLD = Config::DefaultTimer::HOLD;      // visible fija antes de parpadear
  static constexpr uint32_t PERIOD = Config::DefaultTimer::PERIOD;  // período del parpadeo MUY rápido (ms)
  static constexpr uint8_t OFF = Config::DefaultTimer::OFF;         // % del período en que está oculto

  // Pie del Body: línea separadora y texto (el texto baja 1 px: 56 -> 57)
  // static constexpr int16_t PIE_LINE_ROW = 54;  // línea horizontal 1 px, a 2 px sobre el pie
  // static constexpr int16_t PIE_TOP = 57;       // texto "Best"/versión (antes fila 56)

  // ========================================================
  // Métodos internos
  // ========================================================

  // void blink();

  // ========================================================
  // Estado
  // ========================================================

  // uint16_t _bestScore;
  // const char* _version;

  // const char* _title;  // título del Header (default "Snake II")
  // bool _showFooter;    // pie "Best"/versión (default true)

  // uint8_t _optionCount;
  // const char* const* _optionTexts;
  bool _holdButtons;  // true si el mensaje de "Press any button..." está visible
  bool _lastScroll;
  bool _showContinue;  // muestra/oculta la opción "Continue" (default: oculta)

  int8_t _selected;  // opción actual (objetivo central)
  int8_t _lastSelected;
  // Ticker _ticker;    // avance de 1 px cada ANIM_TICK ms (acumulador por tiempo)
  Blink _blink;  // parpadeo de la opción (ancla + fase + flanco)
  // bool _redraw;         // primer frame tras begin(): clear() completo + estáticos
  // bool _diamondsDirty;  // hay que repintar solo la banda de rombos (restoreDiamondBand)
  ButtonRepeat _repeat;  // repetición de los botones de navegación al mantenerlos

  Scroller _scroller;  // scroller de 1 bit del cuadro de selección (1 banda, texto 12x16)
  int8_t _space;

  bool _confirm;
  bool _done;
  bool _clear;  // primer frame tras begin(): clear() completo + estáticos
};

#endif

// ====================================================================================
// Fin
// ====================================================================================