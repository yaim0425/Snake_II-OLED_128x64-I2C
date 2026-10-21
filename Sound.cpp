#include "Sound.h"
#include "Globals.h"

// ========================================================
// Funciones comunes de las ventanas
// ========================================================

Sound::Sound(uint8_t pin)
  : _buzzer(pin),
    _seq(nullptr),
    _len(0),
    _step(0),
    _loop(false) {}

void Sound::begin() {
  _buzzer.begin();
  stop();
}

void Sound::update() {
  if (!storage.soundEnabled()) {
    if (_seq != nullptr) stop();

    return;
  }

  _buzzer.update();

  if (_seq == nullptr) return;

  if (_buzzer.busy()) return;

  _step++;

  if (_step < _len) {
    _buzzer.tone(_seq[_step].freq, _seq[_step].durMs);
  } else if (_loop) {
    _step = 0;
    _buzzer.tone(_seq[0].freq, _seq[0].durMs);
  } else {
    _seq = nullptr;
  }
}

void Sound::stop() {
  _seq = nullptr;
  _len = 0;
  _step = 0;
  _loop = false;
  _buzzer.stop();
}

// ========================================================
// Fin — Funciones comunes de las ventanas
// ========================================================





// ========================================================
// Funciones locales
// ========================================================

void Sound::play(Sfx effect, bool loop) {
  if (!storage.soundEnabled()) return;

  if (effect >= SFX_COUNT) effect = SFX_NONE;

  _seq = EFFECTS[effect].notes;
  _len = EFFECTS[effect].len;
  _loop = loop;
  _step = 0;

  if (_len > 0)
    _buzzer.tone(_seq[0].freq, _seq[0].durMs);
  else
    _seq = nullptr;
}

// ========================================================
// Fin — Funciones locales
// ========================================================
