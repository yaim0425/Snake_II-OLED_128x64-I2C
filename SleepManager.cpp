#include "SleepManager.h"
#include "Globals.h"
#include "Engine.h"

#include <Arduino.h>
#include <string.h>
#include "driver/gpio.h"
#include "esp_sleep.h"

// ========================================================
// Constructor: recibe el Engine (para consultar la partida y
// repintar tras un diagnóstico); el contador de inactividad y el
// flag del diagnóstico arrancan limpios.
// ========================================================

SleepManager::SleepManager(Engine& engine)
  : _idle(),
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
//   igualmente para re-verificar por polling en enterSleep(); con
//   HW sano el botón despierta al instante por el GPIO por nivel.
// ========================================================

void SleepManager::begin() {
  for (uint8_t i = 0; i < Buttons::MAX_BUTTONS; i++)
    gpio_wakeup_enable((gpio_num_t)BUTTONS[i], GPIO_INTR_HIGH_LEVEL);
  esp_sleep_enable_gpio_wakeup();
  esp_sleep_enable_timer_wakeup((uint64_t)WAKE_CHECK_MS * 1000ULL);
}

// ========================================================
// Actualizar (cada frame, tras buttons.read()).
//
// Igual que antes vivía en loop(): cualquier botón presionado
// (estado físico, sin supresión) = actividad y reinicia el
// contador; fuera de partida y sin actividad durante
// IDLE_TIMEOUT_MS se entra en reposo (bloquea hasta que un botón
// despierta; al volver, no dormir al instante). Nunca se entra
// con un pin en HIGH: el wake por nivel despertaría al instante.
// ========================================================

void SleepManager::update() {
  if (buttons.anyPhysical()) _idle.start();

  if (!_engine.isInGame() && !buttons.anyPhysical() && _idle.expired(IDLE_TIMEOUT_MS)) {
    enterSleep();
    _idle.start();
  }
}

// ========================================================
// Reposo (light sleep)
//
// Apaga el OLED y detiene la CPU con esp_light_sleep_start():
// el light sleep conserva la RAM (los estados de las ventanas
// y Storage siguen vivos) y despierta por cualquier botón
// (gpio_wakeup_enable con nivel HIGH, configurado en begin()).
// Al despertar, loop() continúa donde quedó.
// ========================================================

void SleepManager::enterSleep() {
  display.power(false);  // apaga el panel OLED (ahorro; la RAM del OLED se conserva)
  sound.stop();          // corta un efecto en curso

  // Wake de verificación: aunque esp_light_sleep_start() vuelva "solo"
  // (nivel que ya no persiste, USB host, etc.), no se sale del reposo si
  // no hay un botón realmente presionado; se vuelve a dormir al instante.
  // El wake real se detecta por polling (timer cada WAKE_CHECK_MS + el GPIO
  // por nivel como vía rápida) y la pantalla queda apagada hasta entonces.
  uint8_t spurious = 0;
  while (!buttons.anyPhysical()) {
    uint64_t t0 = nowMs();
    esp_err_t err = esp_light_sleep_start();  // bloquea hasta el wake de un botón o del timer
    buttons.read();                           // refresca el estado físico tras despertar
    uint64_t slept = nowMs() - t0;

    if (buttons.anyPhysical()) break;

    // Light sleep que no persiste: volvió casi sin dormir y sin botón
    // (rechazado o wake espurio). En una placa sana duerme ~WAKE_CHECK_MS
    // hasta el timer de verificación. N espurios seguidos = aviso.
    if (err == ESP_ERR_SLEEP_REJECT || slept < MIN_SLEEP_MS) {
      if (spurious++ >= SPURIOUS_LIMIT) {
        spurious = 0;
        screenDiag();
      }
    } else spurious = 0;
  }

  // --- al despertar con un botón real ---
  display.power(true);  // el framebuffer del OLED sigue en el panel: misma imagen
  if (_diagSeen) {      // ...salvo que un diagnóstico la deformara
    _diagSeen = false;
    _engine.repaint();
  }
  buttons.begin();               // re-ancla el estado: el botón que despertó no es un "press"
  buttons.ignoreUntilRelease();  // ...ni cuenta como pulsación sostenida hasta soltarlo
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

void SleepManager::screenDiag() {
  _diagSeen = true;

  display.power(true);
  display.clear();

  char* msg;
  int16_t msgW;

  msg = "ERROR";
  display.drawText(msg, (WIDTH - strlen(msg) * 12) / 2, HEADER_TOP + 1, TEXT_12x16, true, false);

  msg = "SLEEP FAIL";
  display.fillRect(0, BOX_TOP - 1, WIDTH, BOX_HEIGHT + 2, true);
  display.drawText(msg, (WIDTH - strlen(msg) * 12) / 2, BOX_TOP, TEXT_12x16, false, true);

  msg = "The EPS32 is failing";
  display.fillRect(0, FOOT_TOP - 1, WIDTH, FOOT_H + 2, true);
  display.drawText(msg, (WIDTH - strlen(msg) * 6) / 2, FOOT_TOP, TEXT_6x8, false, true);

  display.show();
  delay(DIAG_MS);
  display.power(false);
}

// ====================================================================================
// Fin
// ====================================================================================