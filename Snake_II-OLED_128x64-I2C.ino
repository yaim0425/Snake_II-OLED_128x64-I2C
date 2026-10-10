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
//   - SleepManager: reposo (light sleep) y diagnóstico de wake
//     espurio. Recibe a Engine por referencia (consulta la partida
//     y fuerza el repintado tras un diagnóstico) y usa los
//     servicios globales. El wiring solo llama
//     sleepManager.begin() (setup()) y sleepManager.update() (loop()).
//
// setup() inicia el hardware y luego engine.begin() (entra al primer
// estado, Boot); loop() hace la única lectura de botones del frame
// (buttons.read()) y llama sleepManager.update(), engine.update(),
// engine.print(), sound.update() y display.show().
// ====================================================================================

#include "Config.h"
#include "Globals.h"
#include "Engine.h"
#include "SleepManager.h"

#include <Arduino.h>

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

// Reposo (light sleep): política de inactividad, wake y diagnóstico
// de wake espurio. Recibe el Engine para no dormir en partida y
// repintar al volver tras un diagnóstico.
SleepManager sleepManager(engine);

// ====================================================================================
// Inicialización: hardware + primera transición (Boot)
// ====================================================================================

void setup() {
  Serial.begin(115200);

  display.begin();
  buttons.begin();
  sound.begin();  // adjunta el canal del buzzer (que Sound posee) y silencia
  engine.begin();
  sleepManager.begin();  // fuentes de wake del light sleep (GPIO por nivel + timer)

  Serial.println("Snake II");
}

// ====================================================================================
// Bucle principal
// ====================================================================================

void loop() {
  buttons.read();
  sleepManager.update();
  engine.update();
  engine.print();
  sound.update();
  display.show();
}

// ====================================================================================
// Fin
// ====================================================================================