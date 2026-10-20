#include "Globals.h"
#include "Engine.h"
#include "SleepManager.h"
#include <Arduino.h>





// ========================================================
// Declaración de las variables
// ========================================================

Display display(Config::Pin::OLED_SDA, Config::Pin::OLED_SCL, Config::Screen::ADDRESS);
Buttons buttons(Buttons::PINS, 30);
Sound sound(Buzzer::PIN);
Storage storage;
Engine engine;
SleepManager sleepManager(engine);

// ========================================================
// Fin
// ========================================================





// ========================================================
// Inicializar las variables
// ========================================================

void setup() {
  Serial.begin(115200);
  display.begin();
  buttons.begin();
  sound.begin();
  engine.begin();
  sleepManager.begin();
}

// ========================================================
// Fin
// ========================================================





// ========================================================
// Ciclo infinito
// ========================================================

void loop() {
  buttons.read();

  if (!sleepManager.update()) {
    sound.update();
    return;
  }

  engine.update();
  engine.print();
  sound.update();
  display.show();
}

// ========================================================
// Fin
// ========================================================
