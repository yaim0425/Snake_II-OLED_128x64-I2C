#include "Sound.h"

// ========================================================
// Secuencias de notas de cada efecto (definición de los
// miembros estáticos SEQ_*: los tonos siguen la paleta del
// GameBuzzer original). Tablas privadas de la clase: solo las
// usa la tabla EFFECTS de más abajo.
// 0 en freq = silencio (espera de durMs).
// ========================================================

const Sound::Note Sound::SEQ_CLICK[] = {
  // Navegar por el menú (like Sound 01)
  { 1800, 35 }
};

const Sound::Note Sound::SEQ_CONFIRM[] = {
  // Activar opción (like Sound 05)
  { 700, 50 },
  { 1000, 80 }
};

const Sound::Note Sound::SEQ_BACK[] = {
  // Volver al menú (descendente)
  { 900, 50 },
  { 600, 90 }
};

const Sound::Note Sound::SEQ_EAT[] = {
  // Comer el alimento (like Sound 11 - moneda)
  { 988, 60 },
  { 1319, 100 }
};

const Sound::Note Sound::SEQ_START[] = {
  // GO! (like Sound 34)
  { 800, 60 },
  { 1100, 60 },
  { 1500, 150 }
};

const Sound::Note Sound::SEQ_LEVEL_UP[] = {
  // Subir de nivel (like Sound 43)
  { 523, 60 },
  { 659, 60 },
  { 784, 60 },
  { 1047, 120 },
  { 1319, 180 }
};

const Sound::Note Sound::SEQ_GAME_OVER[] = {
  // Muerte de la serpiente (like Sound 22)
  { 800, 100 },
  { 650, 100 },
  { 500, 150 },
  { 300, 300 }
};

const Sound::Note Sound::SEQ_TICK[] = {
  // Conteo regresivo: un pitido corto por dígito
  { 900, 40 }
};

const Sound::Note Sound::SEQ_TURN[] = {
  // Cambio de dirección: blip muy corto
  { 1319, 20 }
};

const Sound::Note Sound::SEQ_PAUSE[] = {
  // Pausar: descendente
  { 600, 50 },
  { 300, 60 }
};

const Sound::Note Sound::SEQ_RESUME[] = {
  // Reanudar: ascendente
  { 500, 50 },
  { 900, 60 }
};

const Sound::Note Sound::SEQ_NEW_BEST[] = {
  // Festejo de nuevo récord: fanfarria de victoria (marcha triunfal:
  // motivo ascendente C5-E5-G5-C6 y cierre largo en E6)
  { 523, 100 },   // C5  "ta"
  { 659, 100 },   // E5  "ra"
  { 784, 100 },   // G5  "ra"
  { 1047, 140 },  // C6  "pum"
  { 784, 100 },   // G5
  { 1047, 140 },  // C6
  { 1319, 420 }   // E6  cierre largo
};

const Sound::Note Sound::SEQ_BOOT[] = {
  // Fanfarria de arranque del programa (Boot): motivo ascendente de
  // fanfarria (C5-E5-G5-C6) con repetición y cierre largo en E6; más
  // breve que el festejo de nuevo récord para no retrasar el inicio
  { 523, 120 },   // C5  "ta"
  { 659, 120 },   // E5  "ta"
  { 784, 120 },   // G5  "ta"
  { 1047, 180 },  // C6  "DAA"
  { 784, 120 },   // G5
  { 1047, 180 },  // C6
  { 1319, 360 }   // E6  cierre largo
};

const Sound::Note Sound::SEQ_PIRATES[] = {
  // "Pirates of the Caribbean" (He's a Pirate), canción completa.
  // Secuencia de notas tomada del repo de xitanggg (licencia MIT,
  // ver THIRD_PARTY_NOTICES.md). Pares {freq, durMs}; 0 = silencio.
  // Cada fila es una frase; el comentario lleva sus notas.
  // Al final se añade un enlace propio (G4 E4) para que el bucle
  // (play con loop=true) vuelva limpio al motivo inicial (E4).

  // Frase A (motivo principal)
  {  330, 125 }, {  392, 125 }, {  440, 250 }, {  440, 125 }, {    0, 125 },  // E4 G4 A4 A4 -
  {  440, 125 }, {  494, 125 }, {  523, 250 }, {  523, 125 }, {    0, 125 },  // A4 B4 C5 C5 -
  {  523, 125 }, {  587, 125 }, {  494, 250 }, {  494, 125 }, {    0, 125 },  // C5 D5 B4 B4 -
  {  440, 125 }, {  392, 125 }, {  440, 375 }, {    0, 125 },                 // A4 G4 A4 -
  // Frase B (repite A)
  {  330, 125 }, {  392, 125 }, {  440, 250 }, {  440, 125 }, {    0, 125 },  // E4 G4 A4 A4 -
  {  440, 125 }, {  494, 125 }, {  523, 250 }, {  523, 125 }, {    0, 125 },  // A4 B4 C5 C5 -
  {  523, 125 }, {  587, 125 }, {  494, 250 }, {  494, 125 }, {    0, 125 },  // C5 D5 B4 B4 -
  {  440, 125 }, {  392, 125 }, {  440, 375 }, {    0, 125 },                 // A4 G4 A4 -
  // Frase C
  {  330, 125 }, {  392, 125 }, {  440, 250 }, {  440, 125 }, {    0, 125 },  // E4 G4 A4 A4 -
  {  440, 125 }, {  523, 125 }, {  587, 250 }, {  587, 125 }, {    0, 125 },  // A4 C5 D5 D5 -
  {  587, 125 }, {  659, 125 }, {  698, 250 }, {  698, 125 }, {    0, 125 },  // D5 E5 F5 F5 -
  {  659, 125 }, {  587, 125 }, {  659, 125 }, {  440, 250 }, {    0, 125 },  // E5 D5 E5 A4 -
  // Frase D
  {  440, 125 }, {  494, 125 }, {  523, 250 }, {  523, 125 }, {    0, 125 },  // A4 B4 C5 C5 -
  {  587, 250 }, {  659, 125 }, {  440, 250 }, {    0, 125 },                 // D5 E5 A4 -
  {  440, 125 }, {  523, 125 }, {  494, 250 }, {  494, 125 }, {    0, 125 },  // A4 C5 B4 B4 -
  {  523, 125 }, {  440, 125 }, {  494, 375 }, {    0, 375 },                 // C5 A4 B4 -
  // Puente
  {  440, 250 }, {  440, 125 },                                               // A4 A4
  // Repetición de la primera parte
  {  440, 125 }, {  494, 125 }, {  523, 250 }, {  523, 125 }, {    0, 125 },  // A4 B4 C5 C5 -
  {  523, 125 }, {  587, 125 }, {  494, 250 }, {  494, 125 }, {    0, 125 },  // C5 D5 B4 B4 -
  {  440, 125 }, {  392, 125 }, {  440, 375 }, {    0, 125 },                 // A4 G4 A4 -
  {  330, 125 }, {  392, 125 }, {  440, 250 }, {  440, 125 }, {    0, 125 },  // E4 G4 A4 A4 -
  {  440, 125 }, {  494, 125 }, {  523, 250 }, {  523, 125 }, {    0, 125 },  // A4 B4 C5 C5 -
  {  523, 125 }, {  587, 125 }, {  494, 250 }, {  494, 125 }, {    0, 125 },  // C5 D5 B4 B4 -
  {  440, 125 }, {  392, 125 }, {  440, 375 }, {    0, 125 },                 // A4 G4 A4 -
  {  330, 125 }, {  392, 125 }, {  440, 250 }, {  440, 125 }, {    0, 125 },  // E4 G4 A4 A4 -
  {  440, 125 }, {  523, 125 }, {  587, 250 }, {  587, 125 }, {    0, 125 },  // A4 C5 D5 D5 -
  {  587, 125 }, {  659, 125 }, {  698, 250 }, {  698, 125 }, {    0, 125 },  // D5 E5 F5 F5 -
  {  659, 125 }, {  587, 125 }, {  659, 125 }, {  440, 250 }, {    0, 125 },  // E5 D5 E5 A4 -
  {  440, 125 }, {  494, 125 }, {  523, 250 }, {  523, 125 }, {    0, 125 },  // A4 B4 C5 C5 -
  {  587, 250 }, {  659, 125 }, {  440, 250 }, {    0, 125 },                 // D5 E5 A4 -
  {  440, 125 }, {  523, 125 }, {  494, 250 }, {  494, 125 }, {    0, 125 },  // A4 C5 B4 B4 -
  {  523, 125 }, {  440, 125 }, {  494, 375 }, {    0, 375 },                 // C5 A4 B4 -
  // Coda (final, dos veces)
  {  659, 250 }, {    0, 125 }, {    0, 375 }, {  698, 250 }, {    0, 125 }, {    0, 375 },  // E5 - - F5 - -
  {  659, 125 }, {  659, 125 }, {    0, 125 }, {  784, 125 }, {    0, 125 }, {  659, 125 }, {  587, 125 }, {    0, 125 }, {    0, 375 },  // E5 E5 - G5 - E5 D5 - -
  {  587, 250 }, {    0, 125 }, {    0, 375 }, {  523, 250 }, {    0, 125 }, {    0, 375 },  // D5 - - C5 - -
  {  494, 125 }, {  523, 125 }, {    0, 125 }, {  494, 125 }, {    0, 125 }, {  440, 500 },  // B4 C5 - B4 - A4
  {  659, 250 }, {    0, 125 }, {    0, 375 }, {  698, 250 }, {    0, 125 }, {    0, 375 },  // E5 - - F5 - -
  {  659, 125 }, {  659, 125 }, {    0, 125 }, {  784, 125 }, {    0, 125 }, {  659, 125 }, {  587, 125 }, {    0, 125 }, {    0, 375 },  // E5 E5 - G5 - E5 D5 - -
  {  587, 250 }, {    0, 125 }, {    0, 375 }, {  523, 250 }, {    0, 125 }, {    0, 375 },  // D5 - - C5 - -
  {  494, 125 }, {  523, 125 }, {    0, 125 }, {  494, 125 }, {    0, 125 }, {  440, 500 },  // B4 C5 - B4 - A4
  {  392, 125 }, {  330, 375 }                                                               // G4 E4: enlace que devuelve al motivo inicial en el bucle
};

const Sound::Note Sound::SEQ_DRAGON_BALL[] = {
  // Tema de apertura de Dragon Ball Z "Cha-La Head-Cha-La",
  // transcripción del sketch de Francisco Roco
  // (www2.udec.cl/~franciscoroco/dragonBall.txt): tema principal
  // ("suena") + cortina ("suenacortina" x2), en bucle
  // (SFX_DRAGON_BALL).
  //
  // El sketch tocaba cada nota como tone(dur); delay(dur); noTone;
  // delay(dur); (mitad sonido, mitad silencio), lo que lo hacía sonar
  // picado. Aquí cada nota suena su duración completa (2*dur, la misma
  // cadencia que el original) sin el silencio intermedio: melodía ligada.
  // Tempo del sketch: semi=50, corch=100, negra=200, blanca=400,
  // redonda=800. Cierra con un respiro {0,1000}.

  // Entrada: B3 D#4 F#4 / D#4 F#4 A4 B4
  {  246,  200 },   // B3
  {  311,  200 },   // D#4
  {  369,  200 },   // F#4
  {  311,  200 },   // D#4
  {  369,  200 },   // F#4
  {  440,  200 },   // A4
  {  493,  200 },   // B4

  // Motivo (1a parte)
  {  588,  600 },   // D5
  {  660,  600 },   // E5
  {    0,  400 },   // -
  {  588,  200 },   // D5
  {    0,  200 },   // -
  {  588,  200 },   // D5
  {  660,  600 },   // E5
  {    0,  400 },   // -
  {    0,  200 },   // -
  {  740,  400 },   // F#5
  {  784,  400 },   // G5
  {  784,  200 },   // G5
  {  660,  400 },   // E5
  {  740,  200 },   // F#5
  {  784,  400 },   // G5
  {  740,  400 },   // F#5
  {  660,  200 },   // E5
  {  660,  400 },   // E5
  {  588,  200 },   // D5
  {  588,  800 },   // D5
  {    0,  200 },   // -
  {  784,  400 },   // G5
  {  784,  200 },   // G5
  {  740,  200 },   // F#5
  {  740,  200 },   // F#5
  {  588,  200 },   // D5
  {  660,  200 },   // E5
  {  588,  200 },   // D5
  {  660, 1400 },   // E5
  {    0, 2000 },   // -

  // Motivo (2a parte)
  {  588,  600 },   // D5
  {  660,  600 },   // E5
  {    0,  400 },   // -
  {  588,  200 },   // D5
  {    0,  200 },   // -
  {  588,  200 },   // D5
  {  660,  600 },   // E5
  {    0,  400 },   // -
  {    0,  400 },   // -
  {  740,  400 },   // F#5
  {  784,  400 },   // G5
  {  784,  200 },   // G5
  {  660,  400 },   // E5
  {  740,  200 },   // F#5
  {  784,  400 },   // G5
  {  740,  400 },   // F#5
  {  660,  200 },   // E5
  {  660,  400 },   // E5
  {  588,  200 },   // D5
  {  588,  400 },   // D5
  {    0,  200 },   // -
  {  493,  200 },   // B4
  {  588,  200 },   // D5
  {  660,  200 },   // E5
  {  699,  400 },   // F5
  {  699,  400 },   // F5
  {  699,  400 },   // F5
  {  699,  400 },   // F5
  {  699, 1600 },   // F5

  // Cortina (fanfarria), dos veces
  { 1176, 100 },   // D6
  {  880, 100 },   // A5
  { 1568, 100 },   // G6
  { 1176, 100 },   // D6
  {  784, 100 },   // G5
  {  880, 100 },   // A5
  { 1176, 100 },   // D6
  {  880, 100 },   // A5
  {  784, 100 },   // G5
  {  588, 100 },   // D5
  {  784, 100 },   // G5
  {  880, 100 },   // A5
  { 1176, 100 },   // D6
  {  880, 100 },   // A5
  { 1568, 100 },   // G6
  { 1176, 100 },   // D6
  {  784, 100 },   // G5
  {  880, 100 },   // A5
  { 1176, 100 },   // D6
  {  880, 100 },   // A5
  {  784, 100 },   // G5
  {  588, 100 },   // D5
  {  784, 100 },   // G5
  {  880, 100 },   // A5

  // Respiro final (el delay(1000) del loop() original)
  {    0, 1000 }
};

// ========================================================
// Tabla de efectos: índice = Sfx (SFX_NONE..SFX_DRAGON_BALL,
// mismo orden que el enum). Cada entrada apunta a su secuencia
// y deriva el largo con sizeof en sitio (ya no hace falta
// LEN_*). SFX_NONE no tiene secuencia (no suena nada).
// ========================================================

const Sound::Seq Sound::EFFECTS[] = {
  { nullptr,             0 },                                       // SFX_NONE
  { SEQ_CLICK,     sizeof(SEQ_CLICK)       / sizeof(Sound::Note) }, // SFX_CLICK
  { SEQ_CONFIRM,   sizeof(SEQ_CONFIRM)     / sizeof(Sound::Note) }, // SFX_CONFIRM
  { SEQ_BACK,      sizeof(SEQ_BACK)        / sizeof(Sound::Note) }, // SFX_BACK
  { SEQ_EAT,       sizeof(SEQ_EAT)         / sizeof(Sound::Note) }, // SFX_EAT
  { SEQ_START,     sizeof(SEQ_START)       / sizeof(Sound::Note) }, // SFX_START
  { SEQ_LEVEL_UP,  sizeof(SEQ_LEVEL_UP)    / sizeof(Sound::Note) }, // SFX_LEVEL_UP
  { SEQ_GAME_OVER, sizeof(SEQ_GAME_OVER)   / sizeof(Sound::Note) }, // SFX_GAME_OVER
  { SEQ_TICK,      sizeof(SEQ_TICK)        / sizeof(Sound::Note) }, // SFX_TICK
  { SEQ_TURN,      sizeof(SEQ_TURN)        / sizeof(Sound::Note) }, // SFX_TURN
  { SEQ_PAUSE,     sizeof(SEQ_PAUSE)       / sizeof(Sound::Note) }, // SFX_PAUSE
  { SEQ_RESUME,    sizeof(SEQ_RESUME)      / sizeof(Sound::Note) }, // SFX_RESUME
  { SEQ_NEW_BEST,  sizeof(SEQ_NEW_BEST)    / sizeof(Sound::Note) },  // SFX_NEW_BEST
  { SEQ_BOOT,      sizeof(SEQ_BOOT)        / sizeof(Sound::Note) },  // SFX_BOOT
  { SEQ_PIRATES,   sizeof(SEQ_PIRATES)     / sizeof(Sound::Note) },  // SFX_PIRATES
  { SEQ_DRAGON_BALL, sizeof(SEQ_DRAGON_BALL) / sizeof(Sound::Note) }   // SFX_DRAGON_BALL
};

// ========================================================
// Constructor
// ========================================================

Sound::Sound(uint8_t pin)
  : _buzzer(pin),
    _seq(nullptr),
    _len(0),
    _step(0),
    _loop(false),
    _enabled(true) {}

// ========================================================
// Inicialización
// ========================================================

void Sound::begin() {
  _buzzer.begin();
  stop();
}

// ========================================================
// Sonido activado/desactivado
// ========================================================

void Sound::setEnabled(bool on) {
  _enabled = on;
  if (!on) stop();
}

bool Sound::enabled() const {
  return _enabled;
}

// ========================================================
// Reproducir un efecto (no bloqueante)
// ========================================================

void Sound::play(Sfx effect, bool loop) {
  if (!_enabled) return;
  if (effect >= SFX_COUNT) effect = SFX_NONE;   // solo por defensa: los caller pasan valores del enum

  _seq  = EFFECTS[effect].notes;
  _len  = EFFECTS[effect].len;
  _loop = loop;

  _step = 0;
  if (_len > 0) _buzzer.tone(_seq[0].freq, _seq[0].durMs);
  else          _seq = nullptr;
}

// ========================================================
// Actualizar: avanza a la siguiente nota del efecto
// ========================================================

void Sound::update() {
  // El Buzzer avanza su propia duración; cuando termina
  // la nota, busy() pasa a false y aquí se toca la siguiente.
  _buzzer.update();

  if (_seq == nullptr) return;
  if (_buzzer.busy()) return;

  _step++;

  if (_step < _len) {
    _buzzer.tone(_seq[_step].freq, _seq[_step].durMs);
  } else if (_loop) {
    // El bucle reinicia desde la primera nota: la música de fondo
    // suena indefinidamente hasta que play() arranque otro efecto
    // o stop() corte (p. ej. al salir de los créditos).
    _step = 0;
    _buzzer.tone(_seq[0].freq, _seq[0].durMs);
  } else {
    _seq = nullptr;
  }
}

// ========================================================
// Estado
// ========================================================

void Sound::stop() {
  _seq = nullptr;
  _len = 0;
  _step = 0;
  _loop = false;
  _buzzer.stop();
}

bool Sound::playing() const {
  return _seq != nullptr;
}

// ====================================================================================
// Fin
// ====================================================================================