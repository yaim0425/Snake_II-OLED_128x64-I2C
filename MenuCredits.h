#ifndef MENU_CREDITS_H
#define MENU_CREDITS_H

#include <Arduino.h>

#include "Config.h"
#include "Timer.h"
#include "Blink.h"
#include "Scroller.h"

// ========================================================
// MenuCredits — roll de créditos (ventana del Engine)
//
// Muestra todo Config::Credits::CREDITS como un roll vertical
// automático, estilo cine, que avanza bloque a bloque:
//
//   - Header fijo 0..15 con el título (CREDITS / GAME OVER /
//     YOU WIN) en 12x16 centrado.
//   - Body 16..63 = banda del roll (alto de fila 10 px).
//   - El texto se clasifica por el patrón del array: la primera
//     fila de cada bloque (tras un separador) es un Título
//     (barra blanca de ancho completo con letra negra); el resto
//     son nombres (texto normal). El bloque inicial (Snake II /
//     versión / fecha) no lleva barra.
//   - Cada bloque entra desde abajo (Scroller), se centra en el
//     Body, pausa ~PAUSE_MS y el siguiente sube.
//   - Al terminar el último bloque aparece la tarjeta final: las
//     tres filas NAME / VERSION / RELEASE_DATE centradas, o el
//     mensaje THANKS si se llegó desde Game (fromGame = true).
//     El Header permanece visible durante la tarjeta.
//   - Salida: ACTION_UP en cualquier momento corta la música
//     (sound.stop()), suena SFX_BACK y se vuelve al Menu.
//     Mientras el roll corre suena de fondo SFX_PIRATES (desde
//     Game) o SFX_DRAGON_BALL (desde el menú), ambas en bucle.
//
// El reposo no aplica mientras el roll corre: Engine pregunta
// rolling() (SleepManager solo cuenta la inactividad cuando el
// reposo está permitido, así el conteo empieza al acabar el roll).
//
// Regla: todo lo que viene de Config está renombrado en las
// constantes privadas (CREDITS/COUNT/TITLE/CARD_*); el
// .cpp NO referencia Config nunca.
// ========================================================

class MenuCredits {
public:

  // ========================================================
  // Constructor (sin parámetros de servicios: usa los globales
  // Display, Buttons y Sound — Globals.h)
  // ========================================================

  MenuCredits();

  // ========================================================
  // Inicialización
  //
  // fromGame = true  -> se llegó desde Game (música de pirata y
  //                     tarjeta final THANKS). El Engine lo pasará
  //                     al integrar Game; hoy (entrada por el menú)
  //                     siempre es false.
  // fromGame = false -> desde el menú: música Dragon Ball y tarjeta
  //                     NAME/VERSION/RELEASE_DATE.
  // ========================================================

  void begin(bool fromGame = false, bool winner = false);

  // ========================================================
  // Actualizar (consume los eventos de botones leídos en loop())
  // ========================================================

  void update();

  // ========================================================
  // Dibujar
  // ========================================================

  void print();
  void firstPrint(bool forceRedraw = false);

  // ========================================================
  // Estado
  // ========================================================

  bool done() const;     // salir con ACTION_UP: el Engine vuelve al Menu
  bool rolling() const;  // true mientras el roll corre (el reposo no aplica)

private:

  // ========================================================
  // Geometría del roll
  // ========================================================

  static constexpr int16_t WIDTH = Config::Screen::WIDTH;
  static constexpr int16_t HEADER_TOP = Config::Screen::HEADER_TOP;
  static constexpr int16_t BODY_TOP = Config::Screen::BODY_TOP;
  static constexpr int16_t BODY_H = Config::Screen::BODY_H;
  static constexpr int16_t CHAR_H = Config::Screen::CHAR_H;
  static constexpr int16_t LINE_H = CHAR_H + 2;    // alto de fila (8 px de letra + 2 de aire)

  static constexpr uint32_t PAUSE_MS = Config::Credits::PAUSE_MS;  // pausa con el bloque centrado

  static constexpr uint8_t COUNT = (uint8_t)Config::Credits::COUNT;
  static constexpr uint8_t MAX_BLOCKS = 8;

  // ========================================================
  // Renombres de Config (el .cpp no referencia Config nunca):
  //   CREDITS / COUNT      -> Config::Credits::CREDITS / COUNT
  //   TITLE                -> texto del Header de la ventana
  //   CARD_THANKS          -> Config::Credits::THANKS
  //   CARD_NAME / VERSION  -> Config::Version::NAME / VERSION
  //   CARD_DATE            -> Config::Version::RELEASE_DATE
  // ========================================================

  static constexpr auto& CREDITS = Config::Credits::CREDITS;
  static constexpr char* TITLE_CREDITS = "CREDITS";
  static constexpr char* TITLE_GAME_OVER = "GAME OVER";
  static constexpr char* TITLE_WINNER = "YOU WIN";
  static constexpr const char* CARD_THANKS = Config::Credits::THANKS;
  static constexpr const char* CARD_NAME = Config::Version::NAME;
  static constexpr const char* CARD_VERSION = Config::Version::VERSION;
  static constexpr const char* CARD_DATE = Config::Version::RELEASE_DATE;

  // ========================================================
  // Fases del roll
  // ========================================================

  enum class Phase : uint8_t { ROLL, CARD };

  // Un bloque = filas no vacías consecutivas del array (start =
  // índice de la primera fila del bloque, count = cantidad).
  struct Block { int16_t start; int16_t count; };

  // ========================================================
  // Métodos internos
  // ========================================================

  void computeBlocks();  // deriva los bloques de CREDITS
  void advance();        // compone el bloque actual en el Scroller y arranca su slide
  void drawRoll();
  void drawCard();

  // ========================================================
  // Estado
  // ========================================================

  bool _fromGame;  // llegó desde Game (música pirata + THANKS)
  bool _winner;
  bool _clear;     // primer frame tras begin(): clear() completo + header
  bool _done;      // salir: el Engine vuelve al Menu

  Blink _blink;
  int16_t _yUp;
  int16_t _yMiddle;

  int16_t _blockStart;  // índice de la primera fila del bloque actual
  int16_t _blockH;      // alto del bloque actual en px

  Phase _phase;       // ROLL (corre el roll) o CARD (tarjeta final)

  Block _blocks[MAX_BLOCKS];
  uint8_t _blockCount;
  uint8_t _block;     // bloque actual (0.._blockCount-1)

  Scroller _scroller;   // slide vertical del bloque actual

  Stopwatch _pause;      // pausa con el bloque centrado
  bool _paused;          // true: la pausa del bloque actual ya está anclada
};



#endif

// ====================================================================================
// Fin
// ====================================================================================