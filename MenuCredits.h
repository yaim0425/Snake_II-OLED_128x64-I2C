#ifndef MENU_CREDITS_H
#define MENU_CREDITS_H
#include <Arduino.h>
#include "Config.h"
#include "Timer.h"
#include "Scroller.h"

// ========================================================
// MenuCredits — créditos, game over y victoria (roll)
// ========================================================

class MenuCredits {
public:
  // Funciones
  MenuCredits();
  void begin(bool fromGame, bool winner);
  void update();
  void print();
  void firstPrint(bool forceRedraw);
  bool done() const;
  bool card() const;

private:
  // Variables comunes de las ventanas
  static constexpr int16_t HEIGHT = Config::Screen::HEIGHT;
  static constexpr int16_t HEADER_TOP = Config::Screen::HEADER_TOP;
  static constexpr int16_t BODY_TOP = Config::Screen::BODY_TOP;
  static constexpr int16_t BODY_H = Config::Screen::BODY_H;
  static constexpr int16_t LINE_H = Config::Screen::CHAR_H + 2;   // alto de fila de un bloque

  // Constantes
  // Pausa al terminar cada bloque de créditos, en milisegundos.
  static constexpr uint32_t PAUSE_MS = 4 * 1000;

  // Líneas del rol (cadena vacía = separador de bloque).
  static constexpr const char* CREDITS[] = {
    Config::Version::NAME,
    Config::Version::VERSION,
    Config::Version::RELEASE_DATE,
    "",
    "Programmer",
    "OpenCode.AI",
    "YAIM0425",
    "",
    "Reviewer",
    "Claude.AI",
    "ChatGPT.com",
    "YAIM0425",
    "",
    "Director",
    "YAIM0425",
    "",
    "Producer",
    "YAIM0425",
    "",
    "Graphics",
    "YAIM0425",
    "OpenCode.AI",
    "",
    "Sound Effects",
    "OpenCode.AI",
    "",
    "License",
    "Polyform",
    "Noncommercial 1.0.0",
    "Copyright(c) 2026"
  };
  static constexpr uint8_t COUNT = sizeof(CREDITS) / sizeof(CREDITS[0]);

  static constexpr const char* TITLE_CREDITS = "CREDITS";
  static constexpr const char* TITLE_GAME_OVER = "GAME OVER";
  static constexpr const char* TITLE_WINNER = "YOU WIN";

  // Tarjeta final, al terminar el roll.
  static constexpr const char* CARD_THANKS = "Thanks for playing!";
  static constexpr const char* CARD_NAME = Config::Version::NAME;
  static constexpr const char* CARD_VERSION = Config::Version::VERSION;
  static constexpr const char* CARD_DATE = Config::Version::RELEASE_DATE;

  // Funciones
  void startStay();
  void startExit();
  void startCard();
  void startBlock(uint8_t count);
  void drawRoll();

  // Variables
  bool _fromGame;
  bool _winner;
  bool _clear;
  bool _done;
  int16_t _blockH;
  int16_t _rest;
  bool _stay;
  int8_t _line;
  Scroller _scroller;
  bool _isScrolling;
  bool _card;
  Stopwatch _pause;
};

// ========================================================
// Fin
// ========================================================

#endif