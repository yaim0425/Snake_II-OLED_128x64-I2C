#pragma once
#include "Display.h"
#include "Buttons.h"
#include "Sound.h"
#include "Storage.h"

// Servicios globales: instancias cableadas en el .ino (raíz de composición).
extern Display display;
extern Buttons buttons;
extern Sound sound;
extern Storage storage;