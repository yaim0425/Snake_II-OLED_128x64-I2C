#include "Legend.h"
#include "Globals.h"
#include "Draw.h"
#include <string.h>

const char* const Legend::BTN_FUNC[4] = {
  "Btn1 / Back",
  "Btn2 / Select / Pause",
  "Btn3 / None",
  "Btn4 / None"
};

Legend::Legend()
  : _btn(0),
    _lastBtn(0),
    _ticker(NEXT),
    _blink(),
    _lastScroll(true),
    _done(false),
    _clear(true) {}

void Legend::begin() {
  _btn = 0;
  _lastBtn = 0;
  _ticker.start();
  _blink.start();
  _lastScroll = true;
  _done = false;
  _clear = true;
}

void Legend::update() {
  bool exit = false;

  if (buttons.pressed(Buttons::MOVE_UP) || buttons.pressed(Buttons::MOVE_RIGHT) || buttons.pressed(Buttons::MOVE_DOWN) || buttons.pressed(Buttons::MOVE_LEFT)) {
    sound.play(Sound::SFX_CLICK, false);
    exit = true;
  } else if (buttons.pressed(Buttons::ACTION_UP)) {
    sound.play(Sound::SFX_BACK, false);
    exit = true;
  } else if (buttons.pressed(Buttons::ACTION_RIGHT)) {
    sound.play(Sound::SFX_CONFIRM, false);
    exit = true;
  } else if (buttons.pressed(Buttons::ACTION_DOWN) || buttons.pressed(Buttons::ACTION_LEFT)) {
    sound.play(Sound::SFX_CLICK, false);
    exit = true;
  }

  if (exit) {
    _done = true;
    return;
  }

  if (_lastScroll) {
    if (_scroller.update()) return;

    _lastScroll = false;
    _ticker.start();
    _blink.start();
  };

  uint32_t steps = _ticker.consume();

  if (steps) _btn = (_btn + steps) % 4;
}

void Legend::print() {
  firstPrint(false);
  nextBtn();
  _scroller.print();
  blinkDiamond();
}

void Legend::firstPrint(bool forceRedraw) {
  if (!_clear && !forceRedraw) return;

  display.clear();
  _clear = false;
  _ticker.start();
  _blink.start();
  const char* text = "Move / Action";
  const int16_t middleX = WIDTH / 2;
  int16_t textX = 0;
  display.fillRect(0, TEXT_Y - 1, WIDTH, 8 + 1, true);
  text = "Move";
  textX = (middleX - strlen(text) * 6) / 2;
  display.drawText(text, textX, TEXT_Y, TEXT_6x8, false, true);
  Draw::triangle(Draw::DIR_UP, PAD_LEFT_X, PAD_Y - PAD_RADIO, true);
  Draw::triangle(Draw::DIR_RIGHT, PAD_LEFT_X + PAD_RADIO, PAD_Y, true);
  Draw::triangle(Draw::DIR_DOWN, PAD_LEFT_X, PAD_Y + PAD_RADIO, true);
  Draw::triangle(Draw::DIR_LEFT, PAD_LEFT_X - PAD_RADIO, PAD_Y, true);
  text = "Action";
  textX = (middleX + (middleX - strlen(text) * 6) / 2);
  display.drawText(text, textX, TEXT_Y, TEXT_6x8, false, true);

  for (int8_t i = 0; i < 4; i++) toggleDiamond(i, true);
  const char* pieText = BTN_FUNC[_btn];
  display.fillRect(0, FOOT_TOP - 1, WIDTH, FOOT_H + 1, true);
  display.drawText(pieText, display.centerX(pieText, TEXT_6x8), FOOT_TOP, TEXT_6x8, false, true);
}

void Legend::blinkDiamond() {
  if (!_scroller.done())
    return;
  else if (_lastBtn != _btn)
    return;
  else if (!_blink.changed(PERIOD, OFF))
    return;
  toggleDiamond(_btn, _blink.isVisible(PERIOD, OFF));
}

void Legend::diamondCenter(int8_t diamond, int16_t& centerX, int16_t& centerY) {
  switch (diamond) {
    case 0:
      centerX = PAD_RIGHT_X;
      centerY = PAD_Y - PAD_RADIO;
      break;

    case 1:
      centerX = PAD_RIGHT_X + PAD_RADIO;
      centerY = PAD_Y;
      break;

    case 2:
      centerX = PAD_RIGHT_X;
      centerY = PAD_Y + PAD_RADIO;
      break;

    case 3:
      centerX = PAD_RIGHT_X - PAD_RADIO;
      centerY = PAD_Y;
      break;
  }
}

void Legend::toggleDiamond(int8_t diamond, bool show) {
  int16_t centerX = 0;
  int16_t centerY = 0;
  diamondCenter(diamond, centerX, centerY);
  Draw::diamond(centerX, centerY, show);
}

void Legend::nextBtn() {
  if (_lastBtn == _btn) return;

  _scroller.begin(Draw::DIR_LEFT, Scroller::EndMode::STAY);
  _scroller.addText(BTN_FUNC[_btn], TEXT_6x8, false, true);
  _scroller.setRow(FOOT_TOP - 1);
  _scroller.startSlide();
  _lastScroll = true;
  toggleDiamond(_lastBtn, true);
  toggleDiamond(_btn, true);
  _ticker.start();
  _lastBtn = _btn;
}

bool Legend::done() const {
  return _done;
}