#ifndef STORAGE_H
#define STORAGE_H

#include "Config.h"

// ========================================================
// Storage — estado compartido fuera de las ventanas
//
// Almacena la información que NO es propia de ninguna ventana
// (hoy: el mejor puntaje, el sonido activo y la dificultad) y
// que varias partes del sistema tienen que leer y escribir.
//
// Es un contenedor de datos puro: sin dependencias de hardware
// (Display/Buttons/Sound), sin begin()/update()/print() y sin
// relojes; solo getters y setters. Cualquier valor nuevo que
// deba vivir fuera de una ventana se añade aquí.
//
// La INSTANCIA es global (ver Globals.h): `storage`, definida
// en Snake_II-OLED_128x64-I2C.ino junto a los servicios. Cualquier
// clase la usa directamente desde su .cpp (storage.bestScore())
// incluyendo Globals.h.
// ========================================================

class Storage {
public:

  // ========================================================
  // Constructor (valores por defecto: récord 0, sonido activo
  // y dificultad Config::Difficulty::DEFAULT_LEVEL)
  // ========================================================

  Storage();

  // ========================================================
  // Mejor puntaje (récord)
  // ========================================================

  uint16_t bestScore() const;
  void setBestScore(uint16_t value);

  // ========================================================
  // Sonido activo
  // ========================================================

  bool soundEnabled() const;
  void setSoundEnabled(bool enabled);

  // ========================================================
  // Dificultad (setter con clamp a Config::Difficulty)
  // ========================================================

  uint8_t difficulty() const;
  void setDifficulty(uint8_t level);

private:
  static constexpr uint8_t MIN_LEVEL = Config::Difficulty::MIN_LEVEL;
  static constexpr uint8_t MAX_LEVEL = Config::Difficulty::MAX_LEVEL;
  static constexpr uint8_t DEFAULT_LEVEL = Config::Difficulty::DEFAULT_LEVEL;

  // ========================================================
  // Estado
  // ========================================================

  uint16_t _bestScore;   // récord (0 al arrancar)
  bool _soundEnabled;    // sonido activo (true al arrancar)
  uint8_t _difficulty;   // nivel 1..10 (DEFAULT_LEVEL al arrancar)
};

#endif

// ====================================================================================
// Fin
// ====================================================================================