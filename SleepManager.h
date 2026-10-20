#ifndef SLEEP_MANAGER_H
#define SLEEP_MANAGER_H
#include "Timer.h"
#include "Config.h"

class Engine;

// ========================================================
// SleepManager — reposo (light sleep)
// ========================================================

class SleepManager {
public:
  // Funciones
  SleepManager(Engine& engine);
  void begin();
  bool update();

private:
  // Constantes
  // Gestión de energía: inactividad y light sleep hasta que se pulse un botón.
  static constexpr uint32_t IDLE_TIMEOUT_MS = Config::Screen::SLEEP_MIN * 60 * 1000;

  enum class State : uint8_t { AWAKE,
                               SLEEPING };

  // Funciones
  bool updateAwake();
  bool updateSleeping();
  void enterSleep();
  void wake();

  // Variables
  State _state;
  Stopwatch _timer;
  Engine& _engine;
};

// ========================================================
// Fin
// ========================================================

#endif