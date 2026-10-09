// ====================================================================================
// SNAKE II — Enlace de dependencias (wiring)
//
// Este archivo define los GLOBALES (ver Globals.h):
//   - Display y Buttons: hardware (I2C del OLED y pines de los botones).
//   - Sound: sonido. Contiene su propia capa de hardware Buzzer (por valor,
//     pin Config::Pin::BUZZER) y la inicializa en sound.begin().
//     Declarado extern en Globals.h; definido aquí.
//   - Storage: almacén de estado compartido (mejor puntaje, sonido
//     activo, dificultad). Sin begin(): sus valores por defecto
//     salen del constructor. Declarado extern en Globals.h;
//     definido aquí.
//   - Engine: despachador puro que POSEE las ventanas (Boot, Menu,
//     MenuCredits, MenuDifficulty, MenuSound, Game, Legend) como
//     miembros. En Snake_II.ino ya NO hay ventanas globales: son
//     internas de Engine.
//     AISLADO: por ahora Engine solo posee y despacha Boot, Legend
//     y Menu (ver el bloque AISLADO en Engine.h).
//
// setup() inicia el hardware y luego engine.begin() (entra al primer
// estado, Boot); loop() hace la única lectura de botones del frame
// (buttons.read()) y llama engine.update(), engine.print(), sound.update()
// y display.show(). Sin partida en curso y sin botón durante
// Config::Power::IDLE_TIMEOUT_MS, loop() entra en reposo (light sleep,
// enterSleep()) y despierta con cualquier botón.
// ====================================================================================

#include "Config.h"
#include "Globals.h"
#include "Timer.h"
#include "Engine.h"

#include <Arduino.h>
#include "driver/gpio.h"
#include "esp_sleep.h"

// ====================================================================================
// Globales (servicios de hardware + almacén de estado), compartidos por todas
// las clases. Definidos aquí (no en un .cpp aparte); el orden de construcción
// no importa: cada servicio se inicializa en su begin() desde setup() y
// Storage solo usa su constructor.
// ====================================================================================

Display display;
Buttons buttons(Config::Pin::BUTTONS);
Sound sound(Config::Pin::BUZZER);
Storage storage;

// ====================================================================================
// Despachador: posee las ventanas (Boot, Menu, MenuCredits,
// MenuDifficulty, MenuSound, Game, Legend) y decide cuál se ve
// según su estado. Su constructor no recibe nada: las ventanas
// usan los servicios globales directamente.
//
// AISLADO: durante el trabajo en Menu solo quedan las ventanas
// Boot, Legend y Menu; el flujo es Boot -> Legend -> Menu y ahi se
// detiene.
// ====================================================================================

Engine engine;

// Cronómetro del reposo: cuenta el tiempo sin botones presionados
// fuera de partida; al llegar a Config::Power::IDLE_TIMEOUT_MS se
// duerme (light sleep). Se reinicia con cualquier botón.
Stopwatch idleTimer;

// ========================================================
// Reposo (light sleep)
//
// Apaga el OLED y detiene la CPU con esp_light_sleep_start():
// el light sleep conserva la RAM (los estados de las ventanas
// y Storage siguen vivos) y despierta por cualquier botón
// (gpio_wakeup_enable con nivel HIGH configurado en setup()).
// Al despertar, loop() continúa donde quedó.
// ========================================================

void enterSleep() {
  display.power(false);   // apaga el panel OLED (ahorro; la RAM del OLED se conserva)
  sound.stop();           // corta un efecto en curso

  esp_light_sleep_start();  // bloquea hasta el wake de un botón

  // --- al despertar ---
  display.power(true);      // el framebuffer del OLED sigue en el panel: misma imagen
  buttons.begin();          // re-ancla el estado: el botón que despertó no es un "press"
  buttons.ignoreUntilRelease();  // ...ni cuenta como pulsación sostenida hasta soltarlo
}

// ========================================================
// Inicialización: hardware + primera transición (Boot)
// ========================================================

void setup() {
  Serial.begin(115200);

  display.begin();
  buttons.begin();
  sound.begin();  // adjunta el canal del buzzer (que Sound posee) y silencia
  engine.begin();

  // Despertar del light sleep con cualquier botón (los botones son
  // INPUT_PULLDOWN, pulsado = HIGH)
  for (uint8_t i = 0; i < Buttons::MAX_BUTTONS; i++) {
    gpio_wakeup_enable((gpio_num_t)Config::Pin::BUTTONS[i], GPIO_INTR_HIGH_LEVEL);
  }
  esp_sleep_enable_gpio_wakeup();

  Serial.println("Snake II");
}

// ====================================================================================
// Bucle principal
// ====================================================================================

void loop() {
  buttons.read();  // una sola lectura de botones por frame (de esta lectura
                   // consumen los eventos todas las ventanas despachadas por Engine)

  // Cualquier botón presionado (estado físico, sin supresión) = actividad:
  // reinicia el contador del reposo
  if (buttons.anyPhysical()) idleTimer.start();

  // Fuera de partida y sin actividad durante X ms -> reposo (bloquea hasta
  // que un botón despierta; al volver, no dormir al instante). Nunca se
  // entra con un pin en HIGH: el wake por nivel despertaría al instante.
  if (!engine.isInGame() &&
      !buttons.anyPhysical() &&
      idleTimer.expired(Config::Power::IDLE_TIMEOUT_MS)) {
    enterSleep();
    idleTimer.start();
  }

  engine.update();
  engine.print();
  sound.update();
  display.show();
}

// ====================================================================================
// Fin
// ====================================================================================