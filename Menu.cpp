#include "Menu.h"
#include "Globals.h"
#include "Draw.h"
#include <stdio.h>
#include <string.h>

const char* const Menu::OPTION[OPT_COUNT] = {
  "New",
  "Continue",
  "Difficulty",
  "Sound",
  "Credits"
};

Menu::Menu()
  : _lastScroll(true),
    _showContinue(false),
    _selected(OPT_NEW),
    _lastSelected(OPT_NEW),
    _blink(),
    _repeat(),
    _scroller(),
    _space(WIDTH / OPT_COUNT),
    _confirm(false),
    _done(false),
    _clear(true) {}

void Menu::begin(bool showContinue) {
  buttons.ignoreUntilRelease();

  _scroller.begin(Draw::DIR_LEFT, Scroller::EndMode::STAY);
  _space = WIDTH / (OPT_COUNT + (showContinue ? 1 : 0));
  _showContinue = showContinue;
  _lastScroll = true;
  _confirm = false;
  _done = false;
  _blink.start();

  _selected = showContinue && _selected == OPT_NEW ? OPT_CONTINUE : _selected;
  _selected = !showContinue && _selected == OPT_CONTINUE ? OPT_NEW : _selected;
  _lastSelected = _selected;

  bool clear = false;
  clear = clear || _selected == OPT_NEW;
  clear = clear || _selected == OPT_CONTINUE;
  clear = clear || _selected == OPT_CREDITS;
  if (clear)
    _clear = true;
  else
    showOptions();
}

void Menu::update() {
  if (_done) return;

  navigate();
  action();

  if (_lastScroll) {
    if (_scroller.update()) return;

    _lastScroll = false;
    _blink.start();
  }
}

void Menu::navigate() {
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

  if (moved) sound.play(Sound::SFX_CLICK, false);
}

void Menu::action() {
  if (buttons.pressed(Buttons::ACTION_UP)) {
    _selected = _showContinue ? OPT_CONTINUE : OPT_NEW;

    if (_lastSelected == _selected) return;

    sound.play(Sound::SFX_BACK, false);
  } else if (buttons.pressed(Buttons::ACTION_RIGHT)) {
    sound.play(Sound::SFX_CONFIRM, false);
    _confirm = true;
  }
}

void Menu::print() {
  if (_done) return;

  firstPrint(false);
  nextOption();
  _scroller.print();
  blinkOption();
}

void Menu::firstPrint(bool forceRedraw) {
  if (!_clear && !forceRedraw) return;

  display.clear();
  _clear = false;
  _blink.start();
  display.drawText(NAME, (WIDTH - strlen(NAME) * 12) / 2, HEADER_TOP, TEXT_12x16, true, false);
  display.fillRect(0, BOX_TOP - 1, WIDTH, BOX_HEIGHT + 2, true);
  toggleText(true);
  showOptions();
  display.fillRect(0, FOOT_TOP - 1, WIDTH, FOOT_H + 1, true);
  const char* label = "Best ";
  char bestScore[9];
  snprintf(bestScore, sizeof(bestScore), "%u", (unsigned)storage.bestScore());
  display.drawText(label, 1, FOOT_TOP, TEXT_6x8, false, true);
  display.drawText(bestScore, 1 + strlen(label) * 6, FOOT_TOP, TEXT_6x8, false, true);
  display.drawText(VERSION, WIDTH - strlen(VERSION) * 6, FOOT_TOP, TEXT_6x8, false, true);
}

int8_t Menu::selected() const {
  return _selected;
}

bool Menu::done() const {
  return _done;
}

void Menu::blinkOption() {
  if (_confirm) {
    _done = true;
    _confirm = false;
  } else if (!_scroller.done())
    return;
  else if (_lastSelected != _selected)
    return;
  else if (!_blink.changed(PERIOD, OFF))
    return;

  if (_done && !_scroller.done()) {
    display.fillRect(0, BOX_TOP - 1, WIDTH, BOX_HEIGHT + 2, true);
    _scroller.begin(Draw::DIR_RIGHT, Scroller::EndMode::STAY);
  }

  const bool visible = _done || _blink.isVisible(PERIOD, OFF);
  toggleText(visible);
  toggleDiamond(_selected, visible);
}

void Menu::nextOption() {
  if (_lastSelected == _selected) return;

  toggleDiamond(_lastSelected, false);
  toggleTriangle(_lastSelected, true);

  toggleTriangle(_selected, false);
  toggleDiamond(_selected, true);

  if (_scroller.done() && !_blink.isVisible(PERIOD, OFF)) {
    const char* text = OPTION[_lastSelected];
    display.drawText(text, display.centerX(text, TEXT_12x16), BOX_TOP, TEXT_12x16, false, true);
  }

  bool toLeft = _lastSelected > _selected;
  _scroller.begin(toLeft ? Draw::DIR_LEFT : Draw::DIR_RIGHT, Scroller::EndMode::STAY);
  _scroller.addText(OPTION[_selected], TEXT_12x16, false, true);
  _scroller.setRow(BOX_TOP - 1);
  _scroller.startSlide();
  _lastScroll = true;
  _lastSelected = _selected;
}

void Menu::toggleDiamond(int8_t diamond, bool show) {
  int16_t centerX = diamond + 1;

  if (!_showContinue && diamond >= OPT_CONTINUE) centerX--;

  centerX *= _space;
  Draw::diamond(centerX, DIAMOND_Y, show);
}

void Menu::toggleTriangle(int8_t triangle, bool show) {
  if (triangle >= OPT_COUNT) return;

  int16_t centerX = triangle + 1;

  if (!_showContinue && triangle >= OPT_CONTINUE) centerX--;

  centerX *= _space;
  Draw::triangle(Draw::DIR_UP, centerX, TRIANGLE_Y, show);
}

void Menu::toggleText(bool show) {
  const char* text = OPTION[_selected];
  display.drawText(text, display.centerX(text, TEXT_12x16), BOX_TOP, TEXT_12x16, !show, true);
}

void Menu::showOptions() {
  display.fillRect(0, VALUE_TOP, WIDTH, VALUE_HEIGHT, false);

  for (int8_t pos = 0; pos < OPT_COUNT; pos++) {
    if (!_showContinue && pos == OPT_CONTINUE) continue;

    if (pos != _selected) toggleTriangle(pos, true);
  }

  toggleDiamond(_selected, true);
}