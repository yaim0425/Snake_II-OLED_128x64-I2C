#include "esp32-hal.h"
#include "MenuDifficulty.h"
#include "Globals.h"
#include "Draw.h"

#include <stdio.h>

// ========================================================
// Constructor
// ========================================================

MenuDifficulty::MenuDifficulty()
  : _difficulty(DEFAULT_LEVEL),
    _selected(DEFAULT_LEVEL),
    _lastSelected(DEFAULT_LEVEL),
    _done(false),
    _blink(),
    _clear(true),
    _repeat() {}

// ========================================================
// Inicialización
//
// No borra la pantalla ni dibuja nada: entra sobre el Menu ya
// dibujado, así que basta con poner el valor confirmado en
// edición (y en el último mostrado) y anclar el parpadeo (sin
// salto de fase respecto al reloj de 64 bits). El selector lo
// pinta print().
// ========================================================

void MenuDifficulty::begin() {
  _clear = true;
  _done = false;
  _selected = _difficulty;
  _lastSelected = _selected;
  _blink.start();
}

// ========================================================
// Actualizar
//
// MOVE_RIGHT +1 y MOVE_LEFT -1 con repetición al mantener
// presionado (primer paso inmediato, después cada Config::Button::TICK
// ms tras Config::Button::DELAY de mantención). El valor mostrado
// cambia sin aplicarse; ACTION_RIGHT lo aplica y ACTION_UP cancela.
// En ambos casos se vuelve al Menu.
// ========================================================

void MenuDifficulty::update() {
  if (_repeat.step(Buttons::MOVE_RIGHT) && _selected < MAX_LEVEL) {
    _selected++;
    sound.play(Sound::SFX_CLICK);
  }
  if (_repeat.step(Buttons::MOVE_LEFT) && _selected > MIN_LEVEL) {
    _selected--;
    sound.play(Sound::SFX_CLICK);
  }

  if (buttons.pressed(Buttons::ACTION_RIGHT)) {
    // Select: aplica el nivel mostrado
    _difficulty = _selected;
    sound.play(Sound::SFX_CONFIRM);
    _done = true;
  } else if (buttons.pressed(Buttons::ACTION_UP)) {
    // Back: cancela sin cambiar el nivel confirmado
    sound.play(Sound::SFX_BACK);
    _done = true;
  }
}

// ========================================================
// Dibujar
//
// La franja de rombos del Menu se borra y se vuelve a pintar (las
// flechas parpadean; el número no). No se toca ninguna otra zona:
// el título, el cuadro de la opción y el pie del Menu siguen como
// estaban.
// ========================================================

void MenuDifficulty::print() {
  firstPrint();
  nextOption();
  blinkOption();
}

void MenuDifficulty::clear() {
  display.fillRect(
    0, VALUE_TOP,
    WIDTH, VALUE_HEIGHT,
    false);
}

// ========================================================
// Selector de dificultad
//
// Texto 6x8 con el número 1..10 centrado con ancho constante
// (1 dígito se alinea a la derecha con un espacio inicial: " 5"
// mide lo mismo que "10", 12 px, y el centrado no se desplaza) y
// DOS flechas a los lados, apuntando al exterior ("< 5 >"):
//   - flecha izquierda: MOVE_LEFT (-1), oculta en el mínimo
//   - flecha derecha:   MOVE_RIGHT (+1), oculta en el máximo
// Las flechas parpadean juntas (visible 75%, oculto 25% de
// PERIOD ms), pegadas al número (hueco 4 px); el número no
// parpadea.
//
// Al mantener presionado MOVE_LEFT o MOVE_RIGHT el paso se vuelve
// continuo y el dibujo lo refleja: se detiene el parpadeo, solo la
// flecha del botón activo queda fija y la contraria se oculta. Al
// llegar al límite (1 o 10) el botón de ese lado ya no puede
// avanzar y se dibuja como siempre (parpadeando), sin marcar la
// repetición.
// ========================================================

void MenuDifficulty::nextOption(bool show) {
  if (_lastSelected == _selected && !show) return;

  // Número centrado con ancho constante (" 5" / "10" = 12 px)
  char buf[8];
  if (_selected < 10) sprintf(buf, " %u", _selected);
  else sprintf(buf, "%u", _selected);
  display.drawText(buf, LABEL_X, DIAMOND_Y - 3, TEXT_6x8);

  // Al mantener MOVE_LEFT (-1) o MOVE_RIGHT (+1) la repetición continua
  // queda marcada: SOLO se muestra la flecha del botón activo (fija); la
  // contraria se oculta. Sin mantener, ambas flechas se dibujan visibles y
  // es blinkOption() quien las hace parpadear. No se dibuja la flecha del
  // lado que está en su límite.
  const bool leftHeld = buttons.heldDown(Buttons::MOVE_LEFT) && _selected > MIN_LEVEL;
  const bool rightHeld = buttons.heldDown(Buttons::MOVE_RIGHT) && _selected < MAX_LEVEL;

  Draw::triangle(Draw::DIR_LEFT, TRIANGLE_L, DIAMOND_Y, _selected > MIN_LEVEL && !rightHeld);
  Draw::triangle(Draw::DIR_RIGHT, TRIANGLE_R, DIAMOND_Y, _selected < MAX_LEVEL && !leftHeld);

  _lastSelected = _selected;
  _blink.start();
}

void MenuDifficulty::blinkOption() {
  if (!_blink.changed(PERIOD, OFF)) return;

  const bool visible = _blink.isVisible(PERIOD, OFF);
  const bool leftHeld = buttons.heldDown(Buttons::MOVE_LEFT) && _selected > MIN_LEVEL;
  const bool rightHeld = buttons.heldDown(Buttons::MOVE_RIGHT) && _selected < MAX_LEVEL;

  // Flecha izquierda: fija al mantener MOVE_LEFT; oculta mientras se
  // mantiene MOVE_RIGHT; sin mantener parpadea. No se dibuja en el mínimo.
  if (_selected > MIN_LEVEL && !rightHeld)
    Draw::triangle(Draw::DIR_LEFT, TRIANGLE_L, DIAMOND_Y, leftHeld || visible);

  // Flecha derecha: fija al mantener MOVE_RIGHT; oculta mientras se
  // mantiene MOVE_LEFT; sin mantener parpadea. No se dibuja en el máximo.
  if (_selected < MAX_LEVEL && !leftHeld)
    Draw::triangle(Draw::DIR_RIGHT, TRIANGLE_R, DIAMOND_Y, rightHeld || visible);
}

void MenuDifficulty::firstPrint(bool forceRedraw) {
  if (!_clear && !forceRedraw) return;
  clear();
  _clear = false;

  nextOption(true);
}

// ========================================================
// Estado
// ========================================================

bool MenuDifficulty::done() const {
  return _done;
}

uint8_t MenuDifficulty::difficulty() const {
  return _difficulty;
}

// ====================================================================================
// Fin
// ====================================================================================