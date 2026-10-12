#ifndef MENU_CREDITS_H
#define MENU_CREDITS_H

#include <Arduino.h>

#include "Config.h"
#include "Timer.h"

// ========================================================
// MenuCredits — roll de créditos (ventana del Engine)
//
// Muestra todo Config::Credits::CREDITS como un roll vertical
// automático, estilo cine, que avanza bloque a bloque:
//
//   - Header fijo 0..15 con "Credits" en 12x16 centrado.
//   - Body 16..63 = carril del roll (alto de fila 10 px).
//   - El texto se clasifica por el patrón del array: la primera
//     fila no vacía tras un separador es un Título (barra blanca
//     de ancho completo con letra negra); el resto son nombres
//     (texto normal). El bloque inicial (Snake II / versión /
//     fecha) no lleva barra.
//   - Cada bloque entra desde abajo, se centra verticalmente en
//     el Body, pausa ~PAUSE_MS y el siguiente avanza (~25 px/s).
//   - Al terminar el último bloque aparece la tarjeta final: las
//     tres filas NAME / VERSION / RELEASE_DATE centradas, o el
//     mensaje THANKS si se llegó desde Game (fromGame = true).
//   - Salida: ACTION_UP en cualquier momento corta la música
//     (sound.stop()), suena SFX_BACK y se vuelve al Menu.
//     Mientras el roll corre suena de fondo SFX_PIRATES (desde
//     Game) o SFX_CANON (desde el menú), ambas en bucle.
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
  // fromGame = false -> desde el menú: música Canon y tarjeta
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
  static constexpr int16_t HEADER_H = Config::Screen::HEADER_H;
  static constexpr int16_t BODY_TOP = Config::Screen::BODY_TOP;
  static constexpr int16_t BODY_H = Config::Screen::BODY_H;

  static constexpr int16_t CHAR_W = 6;
  static constexpr int16_t CHAR_H = 8;
  static constexpr int16_t LINE_H = CHAR_H + 2;    // alto de fila (8 px de letra + 2 de aire)
  static constexpr int16_t BODY_CTR = BODY_TOP + BODY_H / 2;  // 40: centro del carril

  static constexpr uint32_t PAUSE_MS = 800;  // pausa con el bloque centrado
  static constexpr uint32_t PX_MS = 40;      // ms por px de desplazamiento (~25 px/s)
  static constexpr int16_t START_PY = BODY_TOP + BODY_H;  // el roll arranca bajo el carril

  static constexpr uint8_t COUNT = Config::Credits::COUNT;
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
  static constexpr char TITLE[] = "Credits";
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

  int16_t blockTargetY(uint8_t block) const;  // py para centrar el bloque en el Body
  void computeBlocks();                        // deriva los bloques de CREDITS
  void startMoveTo(uint8_t block);             // anima _py hasta el centro del bloque
  void drawRoll();
  void drawCard();

  // x para centrar una fila de texto (ancho de letra charW, 12 en 12x16)
  static int16_t centerX(const char* line, uint16_t charW = CHAR_W);

  // ========================================================
  // Estado
  // ========================================================

  bool _fromGame;  // llegó desde Game (música pirata + THANKS)
  bool _winner;
  bool _clear;     // primer frame tras begin(): clear() completo + header
  bool _done;      // salir: el Engine vuelve al Menu

  Phase _phase;    // ROLL (corre el roll) o CARD (tarjeta final)

  Block _blocks[MAX_BLOCKS];
  uint8_t _blockCount;
  uint8_t _block;   // bloque actual (0.._blockCount-1)

  int16_t _py;     // posición en px de la fila 0 dentro del Body
  int16_t _from;   // py de arranque del movimiento en curso
  int16_t _to;     // py objetivo del movimiento en curso
  uint32_t _moveMs;  // duración del movimiento (px * PX_MS)

  Stopwatch _anim;   // reloj del movimiento entre bloques
  Stopwatch _pause;  // reloj de la pausa con el bloque centrado
  bool _moving;      // false = en pausa con el bloque centrado
};

#endif

// ====================================================================================
// Fin
// ====================================================================================