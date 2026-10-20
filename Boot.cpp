#include "Boot.h"
#include "Globals.h"
#include "Sprite.h"

const char* const Boot::MESSAGE[MESSAGE_LINES] = {
  "Press any button",
  "to start"
};

Boot::Boot()
  : _blink(),
    _done(false),
    _clear(true) {}

void Boot::begin() {
  _blink.start();
  _done = false;
  _clear = true;
  sound.play(Sound::SFX_BOOT, false);
}

void Boot::update() {
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
}

void Boot::print() {
  firstPrint(false);
  blinkMessage();
}

void Boot::firstPrint(bool forceRedraw) {
  if (!_clear && !forceRedraw) return;

  display.clear();
  _clear = false;
  _blink.start();
  drawMessage(true);
  display.fillRect(0, BODY_TOP, WIDTH, BODY_H, true);
  display.drawBitmap((WIDTH - LOGO_W) / 2, BODY_TOP, LOGO, LOGO_W, LOGO_H, true, false);
}

void Boot::blinkMessage() {
  if (!_blink.changed(PERIOD, OFF)) return;

  drawMessage(_blink.isVisible(PERIOD, OFF));
}

void Boot::drawMessage(bool show) {
  for (int8_t i = 0; i < MESSAGE_LINES; i++) {
    const char* message = MESSAGE[i];
    const uint8_t x = display.centerX(message, TEXT_6x8);
    const uint8_t y = HEADER_TOP + i * 9;
    display.drawText(message, x, y, TEXT_6x8, show, false);
  }

  display.fillRect(0, BODY_TOP, WIDTH, 1, true);
}

bool Boot::done() const {
  return _done;
}