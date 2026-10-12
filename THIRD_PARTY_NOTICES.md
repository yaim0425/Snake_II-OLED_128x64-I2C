# Avisos de terceros (Third-party notices)

Este documento lista las dependencias de terceros de Snake_II-OLED_128x64-I2C y sus
licencias.
No forma parte del código fuente del proyecto, que se licencia bajo
**PolyForm Noncommercial 1.0.0** (uso no comercial; véase
[`LICENSE.md`](LICENSE.md)); es un aviso de atribución.

## Qué se distribuye en este repositorio

Este repositorio contiene **código original** y **un contenido de terceros
atribuido** (la secuencia de notas de una canción; ver más abajo):

- Todo el código fuente (`.h`, `.cpp`, `.ino`) es original de este proyecto,
  salvo las tablas de notas `Sound::SEQ_PIRATES` y `Sound::SEQ_DRAGON_BALL`
  (`Sound.cpp`).
- Los sprites de la serpiente son tablas de bits generadas a mano en
  `Sprite.h` (namespace `Sprite`), no imágenes importadas.
- No se incluye ninguna fuente de Adafruit ni de Espressif Systems: las
  librerías se resuelven en tiempo de compilación desde el gestor de librerías
  de Arduino IDE, no desde el repositorio.

## Contenido de terceros incluido

- **Canción "Pirates of the Caribbean" (He's a Pirate)** — la secuencia de
  notas de `Sound::SEQ_PIRATES` (`Sound.cpp`) proviene del repositorio
  [xitanggg/-Pirates-of-the-Caribbean-Theme-Song](https://github.com/xitanggg/-Pirates-of-the-Caribbean-Theme-Song)
  de Xitang Zhao, publicado bajo licencia **MIT**. Se incorpora como una tabla
  de frecuencias y duraciones (`{freq, durMs}`) reformateada al estilo de este
  proyecto. A continuación se reproduce el aviso de copyright y la licencia
  exigidos por sus términos:

```
MIT License

Copyright (c) 2021 Xitang Zhao

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

- **Tema de apertura de Dragon Ball** — la secuencia de notas de
  `Sound::SEQ_DRAGON_BALL` (`Sound.cpp`) es una transcripción del sketch de
  Arduino de **Francisco Roco**, publicado en
  <https://www2.udec.cl/~franciscoroco/dragonBall.txt>. Esa página no incluye
  una licencia explícita; se incorpora aquí como una tabla de frecuencias,
  duraciones y pausas (`{freq, durMs}`) reformateada al estilo de este proyecto
  y usada como música de fondo no comercial (el proyecto se licencia bajo
  PolyForm Noncommercial 1.0.0).
```

## Dependencias (no vendorizadas)

| Dependencia | Versión usada | Licencia | Uso en el proyecto |
|-------------|---------------|----------|--------------------|
| [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library) | la que resuelva el gestor de Arduino | BSD-3-Clause | Primitivas de dibujo y fuente de texto 6x8/12x16 (`drawText`, `drawTextInverted`, `fillTriangle`, `getTextWidth`). |
| [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306) | la que resuelva el gestor de Arduino | BSD-3-Clause | Driver del panel OLED y su buffer de píxeles, al que `Display` reenvía las llamadas. |
| [Arduino-ESP32 core (esp32 by Espressif Systems)](https://github.com/espressif/arduino-esp32) | la instalada en el IDE | LGPL-2.1 (o LGPL-2.1-or-later en 3.x); integra ESP-IDF, bajo Apache-2.0 y otros | Framework del sketch: `Arduino.h` (GPIO, `pinMode`, `INPUT_PULLUP`), `HardwareSerial` (log por Serial) y `esp_timer` (reloj de 64 bits en `Timer.h`). |
| [ESP-IDF](https://github.com/espressif/esp-idf) | el que incruste el core anterior | Apache-2.0 | Base del core de Arduino-ESP32, de donde viene `esp_timer` (se enlaza, no se distribuye aquí). |

## Cómo cumplir las licencias

- **BSD-3-Clause (Adafruit):** se cumple conservando el aviso de copyright y la
  cláusula de exención de responsabilidad **en la distribución de las propias
  librerías**. Como este repositorio no las distribuye (el usuario las instala
  desde el gestor de Arduino), ese aviso viaja con ellas. Quien reutilice el
  código de Snake_II-OLED_128x64-I2C debe instalar sus propias copias de las librerías.
- **LGPL-2.1 (core de Arduino-ESP32):** se enlaza dinámicamente como
  biblioteca independiente. No se modifica ni se redistribuye aquí; quien
  reutilice el proyecto debe usar el core oficial de Espressif, sin
  modificaciones.

**No hay nada que redistribuir aquí**: el repositorio solo contiene fuentes
propias. Este archivo se mantiene para que quede constancia de qué depende el
proyecto y bajo qué términos.

## Verificación

Las licencias indicadas son las de las versiones que el proyecto usa. Si
cambias de versión del core, de la placa o de las librerías del gestor, comprueba
la licencia real de lo instalado (los paquetes incluyen su propio `LICENSE`) y
actualiza la tabla de arriba. Las versiones de la tabla son intencionadamente
genéricas: es el sketch quien fija las suyas.
