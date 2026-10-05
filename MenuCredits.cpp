// AISLADO: unidad de translation desactivada mientras se trabaja en Boot y
// Legend. Arduino compila TODOS los .cpp de la carpeta del sketch, asi que este
// archivo se seguiria compilando aunque Engine ya no lo incluya, y llama a la
// API de Display que quedo comentada en Display.h.
// Para revertir: borrar el #if 0 de aqui y el #endif del final.
#if 0

#include "esp32-hal.h"
#include "MenuCredits.h"
#include "Globals.h"

#include "Config.h"

static const char* const ROLE_NAME[MenuCredits::NUM_ENTRIES][2] = {
  { "Dev", "opencode.ai" },
  { "Snake II", "v0.1" },
  { "Director", "YAIM904" }
};

static constexpr int16_t PIE_TOP = 54;

MenuCredits::MenuCredits()
  : _entry(1),
    _exit(false),
    _redraw(true) {}

void MenuCredits::begin() {
  _entry = 1;
  // _scrollerRol.begin();
  // _scrollerNombre.begin();
  loadEntry();
  _exit = false;
  _redraw = true;
}

void MenuCredits::update() {
  navigate();
  _scrollerRol.update();
  _scrollerNombre.update();
}

void MenuCredits::navigate() {
  uint8_t before = _entry;
  bool moved = false;

  if (buttons.pressed(Buttons::MOVE_LEFT) && _entry > 0) {
    _entry--;
    moved = true;
  }

  if (buttons.pressed(Buttons::MOVE_RIGHT) && _entry < NUM_ENTRIES - 1) {
    _entry++;
    moved = true;
  }

  if (moved) {
    sound.play(Sound::SFX_CLICK);
    loadEntry();
    // Al avanzar hacia la derecha la franja entra por la derecha
    _scrollerRol.startSlide();
    _scrollerNombre.startSlide();
    Serial.printf("MenuCredits: opcion %u -> %u\n", before, _entry);
  }

  if (buttons.pressed(Buttons::ACTION_UP)) _exit = true;
}

// ========================================================
// Geometría de las bandas
//
// El rol se centra en el espacio entre el body y el pie; el
// nombre va justo debajo de PIE_TOP. Ambas filas se calculan
// aquí (y no en print()) porque setTexto() las necesita para
// fijar dónde se imprimirá cada franja.
// ========================================================

int16_t MenuCredits::roleY() const {
  int16_t bodyTop = (int16_t)Config::Screen::BODY_TOP;
  int16_t roleH = display.getTextHeight(TEXT_12x16);
  return bodyTop + (PIE_TOP - bodyTop - roleH) / 2;
}

int16_t MenuCredits::nameY() const {
  return PIE_TOP + 1;
}

void MenuCredits::loadEntry() {
  _scrollerRol.setTexto(ROLE_NAME[_entry][0], roleY(), true, TEXT_12x16);
  _scrollerNombre.setTexto(ROLE_NAME[_entry][1], nameY(), true, TEXT_6x8);
}

void MenuCredits::print() {
  // Cada Scroller rellena su propia banda antes de volcar la
  // franja, así que aquí no hace falta pintar nada debajo: solo
  // el clear() del primer frame.
  if (_redraw) {
    display.clear();
    display.drawTextAligned("Credits", CENTER, TEXT_12x16, REGION_HEADER);
    _redraw = false;
  }

  _scrollerRol.print();
  _scrollerNombre.print();
}

bool MenuCredits::done() const {
  return _exit;
}

// ====================================================================================
// Fin
// ====================================================================================

#endif  // AISLADO (ver #if 0 al principio del archivo)
