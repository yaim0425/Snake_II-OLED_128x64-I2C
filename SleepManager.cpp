#include "SleepManager.h"
#include "Globals.h"
#include "Engine.h"

#include <Arduino.h>
#include <string.h>
#include "driver/gpio.h"
#include "esp_sleep.h"

// ========================================================
// Constructor: recibe el Engine (para consultar la partida y
// repintar tras un diagnóstico); arranca en AWAKE y con los
// contadores y flags limpios.
// ========================================================

SleepManager::SleepManager(Engine& engine)
  : _state(State::AWAKE),
    _spurious(0),
    _idle(),
    _diag(),
    _diagSeen(false),
    _engine(engine) {}

// ========================================================
// Inicialización: fuentes de wake del light sleep.
//
// - GPIO por nivel para cada botón (los botones son
//   INPUT_PULLDOWN, pulsado = HIGH): vía rápida. Puede dejar de
//   re-disparar tras el primer ciclo (p. ej. con USB-CDC el host
//   impide que el light sleep persista), así que se complementa
//   con un timer de verificación.
// - Timer cada WAKE_CHECK_MS: garantía de que la CPU despierta
//   igualmente para re-verificar por polling en updateSleeping();
//   con HW sano el botón despierta al instante por el GPIO por nivel.
// ========================================================

void SleepManager::begin() {
  for (uint8_t i = 0; i < Buttons::MAX_BUTTONS; i++)
    gpio_wakeup_enable((gpio_num_t)BUTTONS[i], GPIO_INTR_HIGH_LEVEL);
  esp_sleep_enable_gpio_wakeup();
  esp_sleep_enable_timer_wakeup((uint64_t)WAKE_CHECK_MS * 1000ULL);

  _state = State::AWAKE;
  _spurious = 0;
  _diagSeen = false;
  _idle.start();
}

// ========================================================
// Un paso del reposo, no bloqueante (ver el .h). Despacha por
// fase: AWAKE cuenta inactividad, SLEEPING hace una rebanada de
// light sleep y DIAG mantiene el aviso sin delay.
// ========================================================

bool SleepManager::update() {
  switch (_state) {
    case State::AWAKE:    return updateAwake();
    case State::SLEEPING: return updateSleeping();
    case State::DIAG:     return updateDiag();
  }
  return true;   // inalcanzable (cubre -Wreturn-type)
}

// ========================================================
// AWAKE: cuenta la inactividad y decide entrar en reposo.
// Cualquier botón físico (sin supresión) = actividad y reinicia
// el contador; fuera de partida y sin actividad durante
// IDLE_TIMEOUT_MS se entra en reposo. Nunca se entra con un pin
// en HIGH: el wake por nivel despertaría al instante.
// ========================================================

bool SleepManager::updateAwake() {
  if (buttons.anyPhysical()) _idle.start();

  if (!_engine.isInGame() && !buttons.anyPhysical() && _idle.expired(IDLE_TIMEOUT_MS)) {
    enterSleep();
    return false;   // se durmió: corta el frame
  }
  return true;      // sigue el frame normal
}

// ========================================================
// SLEEPING: una rebanada de light sleep por frame. Despierta por
// botón (GPIO por nivel, vía rápida) o por el timer de
// verificación; el wake real se confirma por polling
// (buttons.read()). Nunca se sale del reposo sin botón real.
// ========================================================

bool SleepManager::updateSleeping() {
  uint64_t t0 = nowMs();
  esp_err_t err = esp_light_sleep_start();
  buttons.read();                    // refresca el estado físico tras despertar
  uint64_t slept = nowMs() - t0;

  if (buttons.anyPhysical()) {       // botón real: despierta
    wake();
    return true;
  }

  // Light sleep que no persiste: volvió casi sin dormir y sin botón
  // (rechazado o wake espurio). En una placa sana duerme ~WAKE_CHECK_MS
  // hasta el timer de verificación. N espurios seguidos = aviso.
  if (err == ESP_ERR_SLEEP_REJECT || slept < MIN_SLEEP_MS) {
    if (++_spurious >= SPURIOUS_LIMIT) {
      _spurious = 0;
      showDiag();
    }
  } else {
    _spurious = 0;
  }
  return false;   // sigue durmiendo
}

// ========================================================
// DIAG: el aviso se mantiene DIAG_MS sin delay(). Se duerme por
// rebanadas para no girar en vacío; la imagen del aviso permanece
// en el panel (power on) y un botón también despierta.
// ========================================================

bool SleepManager::updateDiag() {
  if (_diag.expired(DIAG_MS)) {   // se acabó el aviso
    display.power(false);
    _state = State::SLEEPING;
    return false;
  }

  esp_light_sleep_start();        // panel encendido: la imagen del aviso permanece
  buttons.read();
  if (buttons.anyPhysical()) {    // botón durante el aviso: despierta igual
    wake();
    return true;
  }
  return false;
}

// ========================================================
// Arranque del reposo (NO bloquea): apaga el OLED y pasa a
// SLEEPING. El light sleep real lo hace updateSleeping(),
// rebanada a rebanada, conservando la RAM (los estados de las
// ventanas y Storage siguen vivos) y despertando por cualquier
// botón (gpio_wakeup_enable con nivel HIGH, configurado en begin()).
// ========================================================

void SleepManager::enterSleep() {
  display.power(false);  // apaga el panel OLED (ahorro; la RAM del OLED se conserva)
  sound.stop();          // corta un efecto en curso
  _spurious = 0;
  _state = State::SLEEPING;
}

// ========================================================
// Despertar con un botón real.
// ========================================================

void SleepManager::wake() {
  display.power(true);  // el framebuffer del OLED sigue en el panel: misma imagen
  if (_diagSeen) {      // ...salvo que un diagnóstico la deformara
    _diagSeen = false;
    _engine.repaint();
  }
  buttons.begin();               // re-ancla el estado: el botón que despertó no es un "press"
  buttons.ignoreUntilRelease();  // ...ni cuenta como pulsación sostenida hasta soltarlo
  _idle.start();                 // no volver a dormir de inmediato
  _state = State::AWAKE;
}

// ========================================================
// Aviso de diagnóstico del reposo. Se dibuja SOLO en la banda de
// rombos del Menu (MenuStrip::VALUE_TOP..VALUE_HEIGHT), porque es
// la única franja que cualquier ventana vuelve a pintar sobre la
// imagen retenida (el OLED conserva su RAM con power off): el
// Menu, el Legend y el Boot la cubren con su clear() completo del
// primer frame y MenuSound borra esa banda en cada print(). Así el
// repintado forzado nunca deja restos del aviso, sea cual sea la
// ventana activa.
// ========================================================

void SleepManager::showDiag() {
  _diagSeen = true;

  display.power(true);
  display.clear();

  char* msg;
  // int16_t msgW;

  msg = "ERROR";
  display.drawText(msg, (WIDTH - strlen(msg) * 12) / 2, HEADER_TOP + 1, TEXT_12x16, true, false);

  msg = "SLEEP FAIL";
  display.fillRect(0, BOX_TOP - 1, WIDTH, BOX_HEIGHT + 2, true);
  display.drawText(msg, (WIDTH - strlen(msg) * 12) / 2, BOX_TOP, TEXT_12x16, false, true);

  msg = "The EPS32 is failing";
  display.fillRect(0, FOOT_TOP - 1, WIDTH, FOOT_H + 2, true);
  display.drawText(msg, (WIDTH - strlen(msg) * 6) / 2, FOOT_TOP, TEXT_6x8, false, true);

  display.show();
  _diag.start();
  _state = State::DIAG;
}

// ====================================================================================
// Fin
// ====================================================================================