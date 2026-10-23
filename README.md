# Snake II

Revive el clásico **Snake** con el sabor de la era Nokia, pero en hardware propio:
una serpiente que crece en una pantalla OLED, controlada con **botones físicos**,
con sonido, menús y ese tono retro que nunca pasa de moda.

El objetivo es simple y adictivo: come todo lo que puedas sin chocar contra ti
mismo. La dificultad sube, los reflejos se ponen a prueba y solo los mejores
alcanzan un nuevo récord.

---

## Características

- **Juego retro clásico:** serpiente que crece, comida normal y comida especial
  que desaparece con cuenta atrás.
- **Controles físicos:** 8 botones (un *pad* de dirección y un bloque de acciones)
  pensados para jugar sin mirar.
- **Sistema de puntuación:** marcador en pantalla y récord conservado durante la
  sesión.
- **Dificultad progresiva:** 10 niveles; el juego se acelera a medida que avanzas
  y puedes elegir nivel desde el menú.
- **Tablero con bordes envolventes:** la serpiente reaparece por el lado opuesto y
  el reto está en el cuerpo, no en las paredes.
- **Menús completos:** nueva partida, continuar, dificultad, sonido, escala del
  tablero y créditos.
- **Sonidos:** efectos para comer, girar, pausar, arrancar y más, además de música
  en los créditos.
- **Animaciones cuidadas:** logo de arranque, transiciones deslizantes, parpadeos y
  un conteo 3-2-1 antes de cada partida.
- **Pantalla de créditos** con roll animado y mensajes de victoria o fin de partida.
- **Ahorro de energía:** la pantalla se apaga sola tras un tiempo de inactividad y
  se despierta al pulsar cualquier botón.
- **Sin conexión:** funciona íntegramente en el dispositivo, sin Internet.

## Capturas

<!-- Agregar imágenes del juego aquí -->

## Hardware

- **ESP32-S3**.
- Pantalla **OLED 128x64** (SSD1306) conectada por **I2C**.
- **8 pulsadores** para el control.
- **Zumbador** para el sonido.

Todos los componentes son de bajo coste y fáciles de conseguir.

## Estado del proyecto

Proyecto **completo y jugable**. El juego, los menús, el sonido, las animaciones y
el modo de reposo funcionan de principio a fin. Actualmente en la versión
**v1.0.0**.

## Objetivo

Recrear la experiencia del Snake clásico en un dispositivo dedicado, con controles
físicos y una presentación cuidada, como una pieza de hardware propia: sencilla de
usar, agradable de mirar y desafiante de dominar.

## Documentación

La documentación técnica completa del proyecto se encuentra en:

[DOCUMENTACION.md](DOCUMENTACION.md)

En este documento se explica la arquitectura, estructura del código, clases,
funciones, configuración y procedimientos necesarios para modificar y ampliar el
proyecto.

## Licencia

Este proyecto se distribuye bajo la licencia **PolyForm Noncommercial 1.0.0**. Consulta
`LICENSE.md` para más detalles. Los sonidos de terceros y sus atribuciones figuran en
`THIRD_PARTY_NOTICES.md`.
