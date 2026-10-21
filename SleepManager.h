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
  // Funciones comunes de las ventanas
  SleepManager(Engine& engine);
  void begin();
  bool update();

private:
  // Constantes
  // Tiempo de inactividad antes de suspender.
  static constexpr uint32_t IDLE_TIMEOUT_MS = Config::Screen::SLEEP_MIN * 60 * 1000;

  enum class State : uint8_t {
    AWAKE,
    SLEEPING };

  // Variables comunes de las ventanas
  State _state;
  Stopwatch _timer;
  Engine& _engine;

  // Funciones locales
  bool updateAwake();
  bool updateSleeping();
  void enterSleep();
  void wake();
};

// ========================================================
// Fin
// ========================================================

#endif