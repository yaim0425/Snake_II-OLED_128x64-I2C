#include "esp32-hal.h"
#include "Menu.h"
#include "Globals.h"

#include <stdio.h>

// ========================================================
// Opciones por defecto
// ========================================================

const char* const Menu::OPTION[OPT_COUNT] = {
  "New",
  "Continue",
  "Difficulty",
  "Sound",
  "Credits"
};

// const char* const Menu::DEFAULT_OPTION_TEXT[Menu::DEFAULT_OPTIONS] = {
//   "New",
//   "Continue",
//   "Difficulty",
//   "Sound",
//   "Credits"
// };

// const char* const Menu::NO_CONTINUE_OPTIONS[Menu::DEFAULT_OPTIONS - 1] = {
//   "New",
//   "Difficulty",
//   "Sound",
//   "Credits"
// };

// const char* Menu::optionText(int8_t index) const {
//   return _optionTexts[index];
// }

// ========================================================
// Mapeo índice <-> opción lógica (enum Option)
//
// Con "Continue" visible, el índice de la lista coincide con
// el valor del enum (New=0, Continue=1, Difficulty=2, Sound=3,
// Credits=4). Sin "Continue" la lista se compacta: el índice 1
// pasa a Dificultad, el 2 a Sonido y el 3 a Créditos; el
// OPT_CONTINUE deja de existir (indexOfOption devuelve -1).
//
// "Difficulty" y "Sound" ya no se editan inline aquí: confirm()
// las devuelve como cualquier otra opción y el Engine abre las
// ventanas MenuDifficulty y MenuSound.
// ========================================================

// Menu::Option Menu::optionAt(int8_t index) const {
//   if (_visibleContinue) return (Option)index;
//   switch (index) {
//     case 0: return OPT_NEW;
//     case 1: return OPT_DIFFICULTY;
//     case 2: return OPT_SOUND;
//     default: return OPT_CREDITS;
//   }
// }

// int8_t Menu::indexOfOption(Option option) const {
//   if (_visibleContinue) return (int8_t)option;
//   switch (option) {
//     case OPT_NEW: return 0;
//     case OPT_DIFFICULTY: return 1;
//     case OPT_SOUND: return 2;
//     case OPT_CREDITS: return 3;
//     default: return -1;  // OPT_CONTINUE sin partida en curso
//   }
// }

// ========================================================
// Constructor
// ========================================================

Menu::Menu()
  // : _bestScore(bestScore),
    // _version(version),
    // _title("Snake II"),
    // _showFooter(true),
    // _optionCount(OPT_NEW),
    // _optionTexts(NO_CONTINUE_OPTIONS),
  : _holdButtons(true),
    _lastScroll(true),
    _showContinue(false),
    _selected(OPT_NEW),
    _lastSelected(OPT_NEW),
    _blink(),
    _repeat(),
    // _ticker(PERIOD),
    // _redraw(true),
    // _diamondsDirty(false),
    _scroller(),
    _space(WIDTH / OPT_COUNT),
    _confirm(false),
    _done(false),
    _clear(true) {}

// ========================================================
// Inicialización
// ========================================================

void Menu::begin(bool showContinue, int8_t selected) {
  // _bestScore = 0;
  _holdButtons = true;
  _showContinue = showContinue;
  _selected = selected;
  _lastSelected = selected;
  _space = WIDTH / (OPT_COUNT + (showContinue ? 1 : 0));
  _blink.start(false);
  _lastScroll = true;
  _confirm = false;
  _done = false;
  if(selected == OPT_NEW)
    _clear = true;

  // if (_selected == OPT_NEW && false) {
  //   _showContinue = false;
  //   _selected = OPT_NEW;
  //   _lastSelected = OPT_NEW;
  //   _space = WIDTH / OPT_COUNT;
  // } else if(_selected == OPT_NEW && false) {
  //   _showContinue = true;
  //   _selected = OPT_NEW;
  //   _lastSelected = OPT_NEW;
  //   _space = WIDTH / (OPT_COUNT + 1);
  // }

  // _scroller.begin();
  // _scroller.setTexto(OPTION_TEXT[_selected], TEXT_SEL_TOP, true, TEXT_12x16);
  // _redraw = true;
  // // _optionCount = OPT_COUNT;
}

// ========================================================
// Opciones (cantidad variable)
// ========================================================

// void Menu::setOptions(const char* const* texts, uint8_t count) {
//   if (texts == nullptr) return;

//   if (count < 1) count = 1;
//   if (count > OPT_COUNT) count = OPT_COUNT;

//   _optionTexts = texts;
//   _optionCount = count;

//   if (_selected >= (int8_t)count) _selected = count - 1;

//   // _scroller.begin();
//   _timer.start();
//   _scroller.setTexto(optionText(_selected), TEXT_SEL_TOP, true, TEXT_12x16);
//   _redraw = true;
// }

// ========================================================
// Opción "Continue" (hay partida en curso que reanudar)
//
// Cambia a la lista con/ sin "Continue" (5 o 4 opciones). La
// selección se conserva y se adapta a la nueva cantidad; si la
// opción seleccionada ya no existe (p. ej. estaba en "Continue"
// y se oculta), la navegación vuelve a apuntarla a un rango
// válido (la misión de dejar la selección en "New" la cumple
// el Engine con setSelected tras llamarnos).
// ========================================================

// void Menu::setContinueAvailable(bool available) {
//   if (available == _visibleContinue) return;

//   _visibleContinue = available;
//   setOptions(available ? DEFAULT_OPTION_TEXT : NO_CONTINUE_OPTIONS,
//              available ? DEFAULT_OPTIONS : DEFAULT_OPTIONS - 1);
// }

// ========================================================
// Apariencia (título del Header y pie opcional)
// ========================================================

// void Menu::setTitle(const char* title) {
//   _title = title;
// }

// void Menu::setShowFooter(bool show) {
//   _showFooter = show;
// }

// ========================================================
// Selección inicial (clamp al rango de opciones) y reinicio
// de la animación.
// ========================================================

// void Menu::setSelected(Menu::Option option) {
//   int8_t index = indexOfOption(option);
//   if (index < 0) index = 0;  // la opción no está visible (p. ej. "Continue" oculto)

//   _selected = index;
//   // _scroller.begin();
//   _timer.start();
//   _scroller.setTexto(optionText(_selected), TEXT_SEL_TOP, true, TEXT_12x16);
//   _redraw = true;
// }

// // ========================================================
// // Rombos de posición (restauración)
// //
// // La repintamos entera (y solo ella) porque la ventana anterior
// // la dejó ocupada con su selector. No hace falta vaciar el resto
// // de la pantalla: el título, el cuadro de la opción y el pie
// // siguen siendo los del propio Menu. El Engine entra al Menu sin
// // llamar a begin() (changeState(..., false)), así que el clear()
// // completo no se dispara y esta bandera es la única que repinta.
// // ========================================================

// void Menu::restoreDiamondBand() {
//   _diamondsDirty = true;
//   _timer.start();  // el parpadeo del rombo activo arranca de cero
// }

// ========================================================
// Actualizar (consume los eventos de botones leídos en loop())
//
// "Difficulty" y "Sound" no se editan aquí: al confirmarlas
// confirm() devuelve la opción y el Engine abre la ventana
// correspondiente (MenuDifficulty / MenuSound). El Menú solo
// navega y compone.
// ========================================================

void Menu::update() {
  if (_done) return;

  // if (_holdButtons) _holdButtons = !_timer.expired(5000);

  holdButtons();
  navigate();
  action();

  if (_lastScroll) {
    if (_scroller.update()) return;
    _lastScroll = false;
    // _ticker.start();
    _blink.start(false);
  }

  _blink.update(PERIOD, OFF);


  // navigate();
  // if (true) return;  // IGNORE: no se actualiza el menú en esta versión

  // _scroller.update();
}

// ========================================================
// Navegación
// ========================================================

void Menu::holdButtons() {
  if (!_holdButtons) return;

  _holdButtons = false;
  for (int8_t button = 0; button < Buttons::MAX_BUTTONS; button++)
    _holdButtons = _holdButtons || buttons.hold(button);
}

void Menu::navigate() {
  if (_holdButtons) return;

  bool moved = false;

  if (_repeat.step(Buttons::MOVE_LEFT) && _selected > 0) {
    _selected--;
    if (!_showContinue && _selected == OPT_CONTINUE) _selected--;
    moved = true;
  } else if (_repeat.step(Buttons::MOVE_RIGHT) && _selected < OPT_COUNT - 1) {
    _selected++;
    if (!_showContinue && _selected == OPT_CONTINUE) _selected++;
    moved = true;
  }

  if (moved) sound.play(Sound::SFX_CLICK);
}

void Menu::action() {
  if (_holdButtons) return;

  if (buttons.pressed(Buttons::ACTION_UP)) {
    _selected = _showContinue ? OPT_CONTINUE : OPT_NEW;
    if (_lastSelected == _selected) return;
    sound.play(Sound::SFX_BACK);
  } else if (buttons.pressed(Buttons::ACTION_RIGHT)) {
    sound.play(Sound::SFX_CONFIRM);
    // _confirm = true;
    _confirm = !_confirm;
  }
}

// ========================================================
// Rombos de posición
// ========================================================

// void Menu::blink() {
//   if (_timer.expired(BLINK_HOLD)) {
//     bool visibleDiamond = _timer.blinkOn(BLINK_PERIOD, BLINK_OFF_PCT);
//     if ((visibleDiamond && !_visibleDiamond) || (!visibleDiamond && _visibleDiamond)) {
//       drawDiamond(_selected, true, _visibleDiamond);

//       // // Rombos de posición
//       // int8_t size = DIA_SIZE / 2;
//       // int16_t y = DIA_TOP + DIA_SIZE;
//       // int16_t x = (_selected + 1) * floor(display.getWidth() / (_optionCount + 1));

//       // // Dibujar rombos de posición (triángulos) centrados en la banda inferior
//       // display.fillTriangle(x - size, y - size, x, y - DIA_SIZE, x + size, y - size, _visibleDiamond);
//       // display.fillTriangle(x - size, y - size, x, y - 0, x + size, y - size, _visibleDiamond);

//       // Hacer intermitente el texto de la opción seleccionada
//       display.fillRect(0, BOX_TOP, display.getWidth(), BOX_HEIGHT, false);
//       if (!_visibleDiamond)
//         display.drawTextInverted(OPTION_TEXT[_selected], (Config::Screen::WIDTH - strlen(OPTION_TEXT[_selected]) * 12) / 2, BOX_TOP + 1, TEXT_12x16);

//       _visibleDiamond = !_visibleDiamond;
//     }
//   }
// }

// void Menu::drawDiamond(int8_t diamond, bool focus, bool black) {
//   // Rombos de posición
//   int8_t size = DIA_SIZE / 2;
//   int16_t y = DIA_TOP + DIA_SIZE;
//   int16_t x = (diamond + 1) * floor(display.getWidth() / (OPT_COUNT + 1));

//   if (focus) {
//     display.fillTriangle(x - size, y - size, x, y - DIA_SIZE, x + size, y - size, black);
//     display.fillTriangle(x - size, y - size, x, y - 0, x + size, y - size, black);
//   } else
//     display.fillTriangle(x - size, y, x, y - size, x + size, y, black);
// }

// ========================================================
// Dibujar
// ========================================================

void Menu::print() {
  if (_done) return;

  // // Estáticos (solo al entrar, tras el clear() completo): fondo, cuadro
  // // de selección, header (título) y pie (línea + Best/versión). Se dibujan
  // // UNA sola vez; ya no se borran ni se redibujan en cada frame.
  // if (_redraw) {
  //   display.clear();
  firstPrint();
  blink();
  nextOption();
  _scroller.print();
  //   _redraw = false;
  // }

  // // La ventana anterior (MenuDifficulty/MenuSound) sustituyó la banda de
  // // rombos por su selector y la borró entera. Esa banda (45..53) incluye la
  // // fila de la línea separadora del pie (Config::Screen::FOOT_LINE = 53),
  // // así que hay que repintar las dos cosas. Solo se toca esa franja: el
  // // título, el cuadro de la opción y el texto del pie siguen como estaban.
  // if (_diamondsDirty) {
  //   display.fillRect(0, DIA_TOP, display.getWidth(), DIA_SIZE + 1, true);
  //   for (int8_t i = 0; i < OPT_COUNT; i++)
  //     drawDiamond(i, i == _selected, false);
  //   display.fillRect(0, Config::Screen::FOOT_TOP - 2, display.getWidth(), 1, false);
  //   // display.fillRect(0, Config::Screen::FOOT_LINE, display.getWidth(), 1, false);
  //   _visibleDiamond = true;  // el rombo activo vuelve a la fase visible
  //   _diamondsDirty = false;
  // }

  // if (_lastSelected != _selected) {
  //   drawDiamond(_lastSelected, true, true);
  //   drawDiamond(_selected, false, true);
  //   drawDiamond(_lastSelected, false, false);
  //   drawDiamond(_selected, true, false);

  //   // int8_t size = DIA_SIZE / 2;
  //   // int16_t y = DIA_TOP + DIA_SIZE;
  //   // int16_t ax = (_selected + 1) * floor(display.getWidth() / (_optionCount + 1));
  //   // int16_t bx = (_lastSelected + 1) * floor(display.getWidth() / (_optionCount + 1));

  //   // display.fillTriangle(bx - size, y - size, bx, y - DIA_SIZE, bx + size, y - size, true);
  //   // display.fillTriangle(bx - size, y - size, bx, y - 0, bx + size, y - size, true);
  //   // display.fillTriangle(ax - size, y, ax, y - size, ax + size, y, true);

  //   // display.fillTriangle(ax - size, y - size, ax, y - DIA_SIZE, ax + size, y - size, false);
  //   // display.fillTriangle(ax - size, y - size, ax, y - 0, ax + size, y - size, false);
  //   // display.fillTriangle(bx - size, y, bx, y - size, bx + size, y, false);

  //   display.fillRect(0, BOX_TOP, display.getWidth(), BOX_HEIGHT, false);
  //   display.drawTextInverted(OPTION_TEXT[_selected], (Config::Screen::WIDTH - strlen(OPTION_TEXT[_selected]) * 12) / 2, BOX_TOP + 1, TEXT_12x16);

  //   _lastSelected = _selected;
  //   _timer.start();
  // }

  // blink();
  // if (true) return;  // IGNORE: no se actualiza el menú en esta versión

  // // Dinámicos (cada frame): la banda de la opción deslizante y los rombos.
  // // La banda persistente (lo que ya está en pantalla) se mantiene intacta;
  // // la tira nueva desliza y sus columnas sobrescriben la banda hasta
  // // reemplazarla por completo. La opción anterior permanece hasta ser borrada.
  // // El scroller se salta este volcado cuando está en reposo (nada nuevo que
  // // pintar: la banda ya está en la pantalla) y solo vuelca al navegar, al
  // // recomponer la opción o tras el clear de arriba.
  // _scroller.print();
}

void Menu::firstPrint() {
  if (!_clear) return;

  // ------------------------------------------------------

  display.clear();
  _clear = false;

  // ------------------------------------------------------

  // Título
  const char* name = Config::Version::NAME;
  const int16_t headerW = Config::Screen::HEADER_TOP;
  display.drawText(name, (WIDTH - strlen(name) * 12) / 2, headerW, TEXT_12x16, SSD1306_WHITE, SSD1306_BLACK);

  // const int16_t SIZE = Config::Diamond::SIZE;
  // const int16_t centerX = WIDTH / 2;
  // const int16_t centerY = Config::Screen::FOOT_TOP - SIZE - (1 + 2);

  // // Arriba (↑)
  // display.fillTriangle(
  //   centerX - SIZE, centerY,
  //   centerX, centerY - SIZE,
  //   centerX + SIZE, centerY,
  //   SSD1306_WHITE);

  // // Abajo (↓)
  // display.fillTriangle(
  //   centerX - SIZE, centerY,
  //   centerX, centerY + SIZE,
  //   centerX + SIZE, centerY,
  //   SSD1306_WHITE);

  // Cuadro de selección (banda de la opción actual)
  display.fillRect(0, BOX_TOP - 2, WIDTH, BOX_HEIGHT + 2, SSD1306_WHITE);
  toggleText(true);
  showOptions();

  const int16_t footTop = Config::Screen::FOOT_TOP;
  const int16_t footH = Config::Screen::FOOT_H;
  display.fillRect(0, footTop - 1, WIDTH, footH + 1, SSD1306_WHITE);

  // Best score
  const char* label = "Best ";
  char bestScore[9];
  sprintf(bestScore, "%u", (unsigned)storage.bestScore());
  display.drawText(label, 1, footTop, TEXT_6x8, SSD1306_BLACK, SSD1306_WHITE);
  display.drawText(bestScore, 1 + strlen(label) * 6, footTop, TEXT_6x8, SSD1306_BLACK, SSD1306_WHITE);

  // Versión
  const char* version = Config::Version::VERSION;
  display.drawText(version, WIDTH - strlen(version) * 6, footTop, TEXT_6x8, SSD1306_BLACK, SSD1306_WHITE);

  // for (uint8_t selected = 0; selected < OPT_COUNT; selected++)
  //   drawDiamond(selected, selected == _selected, false);

  // // // Rombos de posición
  // // int8_t size = DIA_SIZE / 2;
  // // int16_t y = DIA_TOP + DIA_SIZE;

  // // // Dibujar rombos de posición (triángulos) centrados en la banda inferior
  // // for (uint8_t selected = 0; selected < _optionCount; selected++) {
  // //   int16_t x = (selected + 1) * floor(display.getWidth() / (_optionCount + 1));

  // //   if (selected == _selected) {
  // //     display.fillTriangle(x - size, y - size, x, y - DIA_SIZE, x + size, y - size, false);
  // //     display.fillTriangle(x - size, y - size, x, y - 0, x + size, y - size, false);
  // //   } else
  // //     display.fillTriangle(x - size, y, x, y - size, x + size, y, false);
  // // }
}

// ========================================================
// Accesos
// ========================================================

int8_t Menu::selected() const {
  return _selected;
}

// int8_t Menu::confirm() const {
//   // Se devuelve la OPCIÓN LÓGICA (enum Option): con "Continue" oculto la
//   // lista es 4 opciones y los índices ya no coinciden con el enum, así el
//   // Engine compara con los mismos valores (OPT_NEW/OPT_CONTINUE/
//   // OPT_DIFFICULTY/OPT_SOUND/OPT_CREDITS) y es él quien abre la ventana
//   // correspondiente.
//   if (buttons.pressed(Buttons::ACTION_RIGHT))
//     return (int8_t)optionAt(_selected);
//   return -1;
// }

// void Menu::setBestScore(uint16_t value) {
//   if (value != _bestScore) {
//     _bestScore = value;
//     _redraw = true;  // el pie cambia: se redibija al volver al menú
//   }
// }

// ====================================================================================
// Fin
// ====================================================================================

bool Menu::done() const {
  return _done;
}

void Menu::blink() {
  if (_confirm) {
    _blink.set(true);
    _confirm = false;
    _done = true;
  } else if (_blink.changed()) {
    _blink.toggle();
  } else {
    return;
  }

  toggleText(_blink.state());
  toggleDiamond(_selected, _blink.state());
}

void Menu::nextOption() {
  if (_lastSelected == _selected) return;

  toggleDiamond(_lastSelected, false);
  toggleTriangle(_lastSelected, true);

  toggleTriangle(_selected, false);
  toggleDiamond(_selected, true);

  // int8_t op = _selected;
  // if (!_visibleContinue && op >= OPT_CONTINUE) op++;
  // const char* text = OPTION_TEXT[op];
  // display.fillRect(0, BOX_TOP - 2, WIDTH, BOX_HEIGHT + 2, SSD1306_WHITE);
  // display.drawText(text, (WIDTH - strlen(text) * 12) / 2, BOX_TOP, TEXT_12x16, SSD1306_BLACK, SSD1306_WHITE);

  _scroller.setTexto(OPTION[_selected], BOX_TOP, _lastSelected > _selected, TEXT_12x16);
  _scroller.startSlide();
  _lastScroll = true;

  // blinkDiamond(true);

  // _timer.start();
  // _holdDiamond = true;
  // _visibleDiamond = false;
  _lastSelected = _selected;
}

void Menu::toggleDiamond(int8_t diamond, bool show) {

  int16_t centerX = diamond + 1;
  if (!_showContinue && diamond >= OPT_CONTINUE)
    centerX--;
  centerX *= _space;

  display.fillTriangle(
    centerX - SIZE, DIAMOND_Y,
    centerX, DIAMOND_Y - SIZE,
    centerX + SIZE, DIAMOND_Y,
    show ? SSD1306_WHITE : SSD1306_BLACK);

  display.fillTriangle(
    centerX - SIZE, DIAMOND_Y,
    centerX, DIAMOND_Y + SIZE,
    centerX + SIZE, DIAMOND_Y,
    show ? SSD1306_WHITE : SSD1306_BLACK);
}

void Menu::toggleTriangle(int8_t triangle, bool show) {
  if (triangle >= OPT_COUNT) return;

  int16_t centerX = triangle + 1;
  if (!_showContinue && triangle >= OPT_CONTINUE)
    centerX--;
  centerX *= _space;

  display.fillTriangle(
    centerX - SIZE, TRIANGLE_Y,
    centerX, TRIANGLE_Y - SIZE,
    centerX + SIZE, TRIANGLE_Y,
    show ? SSD1306_WHITE : SSD1306_BLACK);
}

void Menu::toggleText(bool show) {
  const char* text = OPTION[_selected];
  const bool Color = show ? SSD1306_BLACK : SSD1306_WHITE;
  display.drawText(text, (WIDTH - strlen(text) * 12) / 2, BOX_TOP - 1, TEXT_12x16, Color, SSD1306_WHITE);
}

void Menu::showOptions() {
  display.fillRect(0, VALUE_TOP, WIDTH, VALUE_TOP + VALUE_HEIGHT, SSD1306_BLACK);
  for (int8_t pos = 0; pos < OPT_COUNT; pos++) {
    if (!_showContinue && pos == OPT_CONTINUE) continue;
    if (pos != _selected) toggleTriangle(pos, true);
  }
  toggleDiamond(_selected, true);
}

