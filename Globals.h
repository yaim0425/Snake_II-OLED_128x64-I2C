#pragma once

#include "Display.h"
#include "Buttons.h"
#include "Sound.h"
#include "Storage.h"

// ========================================================
// Globals — globales del proyecto (servicios de hardware +
// almacén de estado)
//
// Los ÚNICOS globales del proyecto: los servicios de hardware
// que usa todo el mundo (Display, Buttons, Sound) y el almacén
// de estado compartido (Storage: mejor puntaje, sonido activo
// y dificultad — sección 22).
// Cualquier clase los usa DIRECTAMENTE (display.drawText(...),
// buttons.pressed(...), sound.play(...), storage.bestScore()),
// sin inyectarlos por constructor.
//
// El Buzzer NO es un servicio global: es propiedad exclusiva
// de Sound, que lo contiene por valor y lo inicializa en su
// begin().
//
// Las ventanas (Boot, Menu, MenuCredits, MenuDifficulty,
// MenuSound, Game, Legend) NO son globales: viven dentro de
// Engine (miembros propios), para no romper la regla del
// despachador (una ventana nunca conoce a las demás; solo Engine
// las coordina).
//
// Las DEFINICIONES viven en Snake_II.ino, en el mismo orden
// en que se declaran aquí.
// ========================================================

extern Display display;
extern Buttons buttons;
extern Sound   sound;
extern Storage storage;

// ====================================================================================
// Fin
// ====================================================================================