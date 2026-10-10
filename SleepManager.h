#ifndef SLEEP_MANAGER_H
#define SLEEP_MANAGER_H

#include "Config.h"
#include "Timer.h"

// ========================================================
// SleepManager — reposo (light sleep) y diagnóstico de wake espurio
//
// Agrupa la política de reposo que antes vivía suelta en el
// wiring (`idleTimer`, `diagSeen`, `enterSleep()`, `screenDiag()`):
// cuándo dormirse (idle sin actividad fuera de partida), cómo
// despertar (GPIO por nivel + timer de verificación por polling)
// y qué hacer al volver (repintado forzado si un diagnóstico
// deformó la imagen retenida del OLED).
//
// Usa los servicios globales (Display, Buttons, Sound — Globals.h)
// directamente, como las ventanas, y recibe a Engine por referencia
// solo para dos cosas: saber si hay partida en curso (el reposo no
// aplica) y forzar el repintado tras un diagnóstico. No es una
// ventana (no tiene begin()/update()/print()/done() de ventana).
//
// El wiring solo debe llamar a begin() (una vez, en setup()) y a
// update() (cada frame, en loop(), tras buttons.read()).
// ========================================================

class Engine;

class SleepManager {
public:
  // ========================================================
  // Constructor (recibe el Engine al que repintar tras un
  // diagnóstico; los servicios Display/Buttons/Sound son globales)
  // ========================================================

  SleepManager(Engine& engine);

  // ========================================================
  // Inicialización (fuentes de wake del light sleep: GPIO por
  // nivel para cada botón + timer de verificación). Se llama
  // desde setup().
  // ========================================================

  void begin();

  // ========================================================
  // Actualizar (cada frame): reinicia el contador de inactividad
  // con cualquier botón y, si toca, entra en reposo (bloquea
  // hasta despertar con un botón). No hace nada en partida.
  // ========================================================

  void update();

private:
  static constexpr const int8_t* BUTTONS = Config::Pin::BUTTONS;
  static constexpr uint32_t WAKE_CHECK_MS = Config::Power::WAKE_CHECK_MS;
  static constexpr uint32_t IDLE_TIMEOUT_MS = Config::Power::IDLE_TIMEOUT_MS;
  static constexpr uint32_t MIN_SLEEP_MS = Config::Power::MIN_SLEEP_MS;
  static constexpr uint8_t SPURIOUS_LIMIT = Config::Power::SPURIOUS_LIMIT;
  static constexpr uint8_t WIDTH = Config::Screen::WIDTH;
  static constexpr int16_t HEADER_TOP = Config::Screen::HEADER_TOP;
  // static constexpr int16_t VALUE_TOP = Config::MenuStrip::VALUE_TOP;
  // static constexpr int16_t VALUE_HEIGHT = Config::MenuStrip::VALUE_HEIGHT;
  static constexpr int16_t BOX_TOP = Config::MenuStrip::BOX_TOP;
  static constexpr int16_t BOX_HEIGHT = Config::MenuStrip::BOX_HEIGHT;
  static constexpr uint32_t DIAG_MS = Config::Power::DIAG_MS;
  static constexpr uint8_t FOOT_H = Config::Screen::FOOT_H;
  static constexpr uint8_t FOOT_TOP = Config::Screen::FOOT_TOP;

  // Cronómetro del reposo: cuenta el tiempo sin botones presionados
  // fuera de partida; al llegar a Config::Power::IDLE_TIMEOUT_MS se
  // duerme. Se reinicia con cualquier botón.
  Stopwatch _idle;

  // true si hubo que mostrar el aviso de diagnóstico en pantalla:
  // la imagen conservada en el OLED quedó deformada y hay que
  // forzar el repintado completo al despertar con un botón.
  bool _diagSeen;

  // Engine al que consultar si hay partida y que repintar al volver.
  Engine& _engine;

  // ========================================================
  // Reposo (bloquea hasta que un botón despierta de verdad)
  // ========================================================

  void enterSleep();

  // ========================================================
  // Aviso de diagnóstico: dibuja una pantalla completa ("ERROR",
  // "SLEEP FAIL", "The ESP32 is failing") que cubre todas las
  // franjas y marca _diagSeen; al despertar, Engine::repaint()
  // fuerza el full frame de la ventana activa (ver su comentario
  // en SleepManager.cpp). No usa Serial: con USB-CDC el puerto se
  // desconecta justo en el light sleep que no persiste.
  // ========================================================

  void screenDiag();
};

#endif

// ====================================================================================
// Fin
// ====================================================================================