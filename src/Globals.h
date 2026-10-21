#pragma once
#include "src/devices/Display.h"
#include "src/devices/Buttons.h"
#include "src/devices/Sound.h"
#include "src/devices/Storage.h"

// Servicios globales: instancias cableadas en el .ino (raíz de composición).
extern Display display;
extern Buttons buttons;
extern Sound sound;
extern Storage storage;