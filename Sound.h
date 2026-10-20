#ifndef SOUND_H
#define SOUND_H
#include <Arduino.h>
#include "Buzzer.h"

// ========================================================
// Sound — efectos de sonido (secuencias SFX)
// ========================================================

class Sound {
public:
  // Constantes
  enum Sfx : uint8_t {
    SFX_NONE = 0,
    SFX_CLICK,
    SFX_CONFIRM,
    SFX_BACK,
    SFX_EAT,
    SFX_START,
    SFX_TICK,
    SFX_TURN,
    SFX_PAUSE,
    SFX_RESUME,
    SFX_BOOT,
    SFX_PIRATES,
    SFX_DRAGON_BALL,
    SFX_COUNT
  };

  // Funciones
  explicit Sound(uint8_t pin);
  void begin();
  void play(Sfx effect, bool loop);
  void update();
  void stop();

private:
  // Constantes
  struct Note {
    uint16_t freq;
    uint16_t durMs;
  };

  struct Seq {
    const Note* notes;
    uint16_t len;
  };

  static const Note SEQ_CLICK[];
  static const Note SEQ_CONFIRM[];
  static const Note SEQ_BACK[];
  static const Note SEQ_EAT[];
  static const Note SEQ_START[];
  static const Note SEQ_TICK[];
  static const Note SEQ_TURN[];
  static const Note SEQ_PAUSE[];
  static const Note SEQ_RESUME[];
  static const Note SEQ_BOOT[];
  static const Note SEQ_PIRATES[];
  static const Note SEQ_DRAGON_BALL[];
  static const Seq EFFECTS[];

  // Variables
  Buzzer _buzzer;
  const Note* _seq;
  uint16_t _len;
  uint16_t _step;
  bool _loop;
};

// ========================================================
// Fin
// ========================================================

#endif