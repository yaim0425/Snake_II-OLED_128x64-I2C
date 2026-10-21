#include "SleepManager.h"
#include "src/Globals.h"
#include "Engine.h"
#include <Arduino.h>
#include "driver/gpio.h"
#include "esp_sleep.h"

// ========================================================
// Funciones comunes de las ventanas
// ========================================================

SleepManager::SleepManager(Engine& engine)
  : _state(State::AWAKE),
    _timer(),
    _engine(engine) {}

void SleepManager::begin() {
  for (uint8_t i = 0; i < Buttons::MAX_BUTTONS; i++) gpio_wakeup_enable((gpio_num_t)Buttons::PINS[i], GPIO_INTR_HIGH_LEVEL);
  esp_sleep_enable_gpio_wakeup();
  _state = State::AWAKE;
  _timer.start();
}

bool SleepManager::update() {
  switch (_state) {
    case State::AWAKE: return updateAwake();
    case State::SLEEPING: return updateSleeping();
  }

  return true;
}

// ========================================================
// Fin — Funciones comunes de las ventanas
// ========================================================





// ========================================================
// Funciones locales
// ========================================================

bool SleepManager::updateAwake() {
  if (buttons.anyPhysical()) _timer.start();

  if (_engine.sleepBlocked()) {
    _timer.start();
    return true;
  }

  if (!buttons.anyPhysical() && _timer.expired(IDLE_TIMEOUT_MS)) {
    enterSleep();
    return false;
  }

  return true;
}

bool SleepManager::updateSleeping() {
  esp_light_sleep_start();
  buttons.read();

  if (buttons.anyPhysical()) {
    wake();
    return true;
  }

  return false;
}

void SleepManager::enterSleep() {
  display.power(false);
  sound.stop();
  _state = State::SLEEPING;
}

void SleepManager::wake() {
  display.power(true);
  buttons.begin();
  buttons.ignoreUntilRelease();
  _timer.start();
  _state = State::AWAKE;
}

// ========================================================
// Fin — Funciones locales
// ========================================================
