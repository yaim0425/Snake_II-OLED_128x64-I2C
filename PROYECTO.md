# Snake_II-OLED_128x64-I2C — ESP32-S3

Snake II (estilo Nokia) para ESP32-S3 con Arduino IDE. POO: una clase por archivo `.h`/`.cpp`.

---

## 0. Reglas de trabajo con la IA

- **La IA no debe hacer nada sin una orden explícita del usuario.**
- No crear, modificar ni eliminar archivos por iniciativa propia.
- **Excepción:** la IA puede modificar el `.gitignore` en cualquier momento, sin
  esperar una orden explícita.
- No proponer ni iniciar fases de desarrollo por su cuenta; solo actuar cuando
  el usuario lo indique.
- **La IA debe hacer un commit al modificar archivos** (cuando el usuario lo ordene
  o como parte del flujo de trabajo ya autorizado).
- **Agrupar los cambios por tema de modificación**: todo lo relacionado con un
  mismo tema (código, este documento y, si corresponde, API) queda en un commit, y
  el **mensaje del commit debe ser informativo**. Los micro-commits por cambio
  puntual ya no se usan.
- **La IA decide cuántos commits hacen falta según los temas**, aunque el usuario
  diga "haz un commit" o "un commit de todos los cambios": si hay varios temas, se
  hacen varios commits (nunca se fuerza todo en uno).
- **Si el cambio necesita más de un commit**, seguir este procedimiento: (1) crear
  un archivo `.json` (junto a `PROYECTO.md`, en la raíz del proyecto) que registre
  los commits **pendientes** (futuros), con la lista de archivos de cada commit y el
  mensaje de cada uno; (2) ir haciendo los commits según lo indicado en el punto 2;
  **por cada commit hecho se borra ese commit del `.json`**; (3) cuando el `.json`
  quede vacío (**todos los commits hechos), se elimina el archivo**; (4) actualizar
  `PROYECTO.md`; (5) dar recomendaciones finales, brevemente.
- **El `.json` es un punto de control (checkpoint):** si los commits se detienen por
  cualquier razón, la siguiente sesión se retoma leyendo `PROYECTO.md` + el `.json`
  (los commits pendientes que aún estén listados) y se continúa donde se quedó. Por
  eso el `.json` se debe mantener al día tras cada commit y no debe borrarse hasta
  haber completado todos.
- **La IA debe actualizar este documento (`PROYECTO.md`) antes de hacer el commit**:
  toda modificación de código debe quedar reflejada (secciones/API) y ese cambio a
  `PROYECTO.md` debe incluirse en el mismo commit.
- **Al pedir un commit, la IA no modifica el código** (`.ino`, `.h`, `.cpp`). No
  "arregla" el código de los archivos para dejarlo bien antes de commitear: si algo
  está mal, lo señala y espera orden. Sí puede, sin permiso: **modificar
  comentarios** en `.ino`/`.h`/`.cpp`/`.md`, **mover código de posición**, **ajustar
  las lineas vacias** y **ajustar la sangreia** (reordenar), pero **nunca cambiar el código**.
- **Commit y `push` van juntos.** En cuanto se crea el commit se hace `push`
  inmediatamente, sin pedir confirmación adicional.
- **Toda regla que el usuario dé debe quedar escrita en este documento**, para que
  sobreviva a la siguiente sesión (ver la frase de arranque de más abajo).
- **La IA no compila sin permiso explícito del usuario.** El usuario compila por su
  cuenta (compilar la IA y después el usuario demora el flujo de trabajo).

### Frase para iniciar una nueva sesión

Para no empezar desde cero, **copiar y pegar tal cual** el siguiente bloque en la
nueva sesión (no hay que escribir nada más):

```
Trabaja en el proyecto Snake_II-OLED_128x64-I2C (ESP32-S3, Arduino IDE) ubicado en
C:\ESP32S3\Snake_II-OLED_128x64-I2C.
```

---

## 1. Estado actual del proyecto

En desarrollo. `.ino` = wiring: define los globales (`Display`, `Buttons`, `Sound`, `Storage`; en `Globals.h`) y crea el `Engine`, dueño de las ventanas (`Boot`, `Legend`, `Menu`, `MenuDifficulty`, `MenuSound`, `MenuCredits`, `Game`). Constantes compartidas en `Config.h`.

**Arquitectura:**
- **`Storage` (§22):** almacén global de estado (mejor puntaje, sonido, dificultad); datos puros sin `begin()`/`update()`; instancia global `storage`. **Creado, sin integrar** (`Menu`/`MenuDifficulty`/`Sound` siguen con copias propias).
- **`Engine`:** despachador; posee las ventanas (no globales, no anidadas); llama `begin()` de la entrante al cambiar de estado. `loop()`: única lectura de botones + reposo (§26).
- **Render sin `clear()` global:** cada ventana limpia solo en su primer frame. `MenuDifficulty`/`MenuSound` son capas sobre el `Menu` (sustituyen la banda de rombos); al salir hacen `changeState(MENU)` — hoy `begin()` completo porque `Menu::restoreDiamondBand()` sigue comentada (§11).
- **Flujo:** `Boot` → `Legend` → `Menu`. La `Legend` solo aparece al arrancar.
- **Aislamiento (`AISLADO`):** activos `Boot`, `Legend`, `Menu`, `MenuSound`, `MenuDifficulty`; `Game` y `MenuCredits` existen pero con estados/miembros/transiciones comentados. `Menu::confirm()` comentado (entrada a `MenuSound`/`MenuDifficulty` no cableada); `ACTION_RIGHT` solo alterna el resaltado (`_confirm`); `ACTION_UP` vuelve a `New`/`Continue`.

**Clases extraídas de `Game`:** `Food` (alimento), `Snake` (lógica pura), `Sprite.h` (tabla), `Timer.h` (reloj 64 bits; migración desde `millis()`).

**Características:** dificultad en caliente (`setDifficulty` recalcula `_moveDelay`); puntaje = nivel de dificultad (`uint16_t`); festejo de récord: "GAME OVER"→"BUT"→"YOU ARE"→"THE BEST" (fanfarria solo en "THE BEST").

---

## 2. Hardware y pines

| Elemento  | Pin |
|-----------|-----|
| MOVE_UP   | 2   |
| MOVE_RIGHT | 1  |
| MOVE_DOWN | 42  |
| MOVE_LEFT | 41  |
| ACTION_UP | 38  |
| ACTION_RIGHT | 40 |
| ACTION_DOWN | 39  |
| ACTION_LEFT | 47  |
| Buzzer    | 14  |
| OLED SDA  | 8   |
| OLED SCL  | 9   |

- OLED SSD1306 monocromo 128x64, I2C, `0x3C`. Librerías: Adafruit_SSD1306, Adafruit_GFX.
- Botones `INPUT_PULLDOWN`.

---

## 3. Estructura de archivos

Directorio: `D:\Documents\ESP32S3\Snake_II-OLED_128x64-I2C`

| Archivo | Contenido |
|---------|-----------|
| `Display.h/.cpp` | Clase `Display`: OLED (`begin`, `clear`/`show`, `power(on)`, `fillRect`, `fillTriangle`, `drawText`, `drawBitmap`). Completa. |
| `Config.h` | Constantes compartidas (namespace `Config`, §19): pines, geometría/regiones, `Difficulty`, `Version`, `Credits`, `Legend`, `Diamond`, `Button`, `Scroller`, `MenuStrip`, `DefaultTimer`, `Power`. Sin `#define` para valores. |
| `Globals.h` | `extern` globales: `display`, `buttons`, `sound`, `storage` (definidos en el `.ino`). No declara `Buzzer` ni ventanas. |
| `Buttons.h/.cpp` | Clase `Buttons`: debounce; `hold`/`pressed`/`released`/`anyHeld`/`anyPhysical`/`ignoreUntilRelease`; `MAX_BUTTONS` = contador del enum `Button`; `isSet` declarado en `.h` y definido en `.cpp`. Completa. |
| `Boot.h/.cpp` | Clase `Boot`: Header "Press any button / to start" parpadeando (`PERIOD=500`, `OFF=20%`), Body con logo `Sprite::LOGO` (80×48) vía `drawBitmap(...,true,false)`. `begin()` suena `SFX_BOOT`; cualquier botón termina (sonido según botón). Animación de bandas anterior comentada. Completa. |
| `Legend.h/.cpp` | Clase `Legend`: pad MOVE (4 flechas) + 4 rombos ACTION que parpadean de a uno en ciclo lento (`NEXT=2500ms`, `PERIOD=100ms`/`OFF=50%`) y texto de la función del rombo activo en el pie (`Scroller` propio, entra deslizándose). Cualquier botón cierra con sonido (MOVE=`CLICK`, `ACTION_UP`=`BACK`, `ACTION_RIGHT`=`CONFIRM`). Completa. |
| `Scroller.h/.cpp` | Clase `Scroller`: scroller de 1 bit, 1 banda de ancho completo; texto `int8_t[]` (byte = 1 columna de 8 px); fondo negro, texto blanco. `setTexto()` compone centrado y trunca al límite; `startSlide()`; `update()` 1 px/4 ms; `print()` con cortina activa. Usada por `Menu` (1), `MenuCredits` (2 sincronizadas), `Legend` (1). Completa. |
| `Menu.h/.cpp` | Clase `Menu`: scroller de 1 bit, rombos/triángulos de posición, navegación con repetición y anti-entrada `_holdButtons`. `begin(showContinue, selected)`, `navigate()`, `action()` (`ACTION_UP`→New/Continue, `ACTION_RIGHT` alterna `_confirm`), `holdButtons()`, `blinkOption()`, `nextOption()`. API anterior (`setOptions`, `setContinueAvailable`, `setBestScore`, `setSelected`, `setTitle`, `setShowFooter`, `confirm()`, `restoreDiamondBand()`) comentada en `Menu.h`. |
| `MenuDifficulty.h/.cpp` | Clase `MenuDifficulty`: selector nivel 1..10 `< N >` con repetición al mantener. Refactorizada al estilo `MenuSound` (`firstPrint()`/`clear()`/`nextOption()`/`blinkOption()`; `_selected`/`_lastSelected`/`_clear`; `Draw::triangle`; `Blink::changed`/`isVisible`) y activa en el `Engine` (`MENU_DIFFICULTY`); entrada desde `Menu` pendiente. Capa sobre el `Menu` (banda 45..53). `difficulty()` → `Game::setDifficulty`. |
| `MenuSound.h/.cpp` | Clase `MenuSound`: selector On/Off con una flecha sobre la misma banda. Activa (`MENU_SOUND`); entrada desde `Menu` pendiente. Centra a mano (`Config::Screen::WIDTH`, `strlen*6`). Aplica con `Sound::setEnabled`. |
| `MenuCredits.h/.cpp` | Clase `MenuCredits`: 3 entradas navegables con transición lateral (2 bandas del `Scroller`), `SFX_CLICK` al navegar, vuelve con `ACTION_UP`. Nombre distinto de "Credits" para no confundir con créditos generales. Completa. |
| `Game.h/.cpp` | Clase `Game`: ventana del juego (estados `NEW`/`CONTINUE`). Coordina dificultad/velocidad, botones (MOVE→`Snake::turn`), `snake.step()`, `Food`, puntaje, sonidos, overlays y volcado del tablero (`Config::Screen::CELL`, `SPRITE_SCALE`). Completa. |
| `Food.h/.cpp` | Clase `Food`: estado (posición, tipo normal/especial), `spawn` en celdas libres (consulta `Game::occupied`), dibujo del rombo/sprite `SPECIAL_FOOD`, temporizador de la especial. Celda de `Config::Screen::CELL`. Completa. |
| `Snake.h/.cpp` | Clase `Snake`: lógica pura (buffer circular, dirección + giro pendiente sin reversa, paso con wrap, colisión, comer/crecer, elección de sprites). Sin `Display`/`Sound`/`Food`. Completa. |
| `Engine.h/.cpp` | Clase `Engine` (antes `App`): despachador; posee las ventanas `_boot`, `_menu`, `_menuDifficulty`, `_menuSound`, `_menuCredits`, `_game`, `_legend`; `changeState()` llama `begin()` de la entrante (parámetro `beginWindow=false` previsto para volver sin repintar); `isInGame()`. Completa. |
| `Snake_II-OLED_128x64-I2C.ino` | Wiring: define servicios globales y `storage`; crea `Engine engine;` y `SleepManager sleepManager(engine);`. `setup()`: `display.begin()`, `buttons.begin()`, `sound.begin()`, `engine.begin()`, `sleepManager.begin()`. `loop()`: `buttons.read()` (única lectura), `sleepManager.update()`, `engine.update()`, `engine.print()`, `sound.update()`, `display.show()`. |
| `Buzzer.h/.cpp` | Clase `Buzzer`: hardware, 1 tono no bloqueante (LEDC). Propiedad de `Sound` por valor. Completa. |
| `Sound.h/.cpp` | Clase `Sound`: secuencias de efectos sobre su `Buzzer` interno; `setEnabled`. Completa. |
| `Storage.h/.cpp` | Clase `Storage`: mejor puntaje, sonido, dificultad (defaults de `Config`; clamp en `setDifficulty`). Datos puros, sin `begin`/`update`/hardware. Instancia global `storage` (§22). Completa; **pendiente de integrar**. |
| `Sprite.h` | Namespace `Sprite`: sprites de la serpiente (4×4) + comida especial (8×4) + logo `LOGO` (80×48). Solo datos (header-only). |
| `Timer.h/.cpp` | Reloj de 64 bits (`nowMs()`, `Stopwatch`, `Ticker`) sobre `esp_timer_get_time()`. Definiciones en el `.cpp`. |
| `ButtonRepeat.h/.cpp` | Helper repetición al mantener (§23): paso inmediato + uno cada `TICK` tras `DELAY`. Lo usan `Menu` y `MenuDifficulty` (`_repeat`). |
| `Blink.h/.cpp` | Helper parpadeo (§24): `Stopwatch` + `_last`. API `start()`, `isVisible()`, `changed()`. Lo usan `Boot`, `Legend`, `Menu`, `MenuDifficulty`, `MenuSound`. |
| `Draw.h/.cpp` | Namespace `Draw`: `triangle(dir,cx,cy,show)`, `diamond(cx,cy,show)`; sin estado; `dir` 1↑ 2→ 3↓ 4←. |
| `PROYECTO.md` | Este documento. |
| `LICENSE.md` | PolyForm Noncommercial 1.0.0 (source-available; prohíbe venta/uso comercial; conservar línea `Required Notice:`). |
| `THIRD_PARTY_NOTICES.md` | Atribuciones (Adafruit BSD-3, core Arduino-ESP32 LGPL-2.1, ESP-IDF Apache-2.0); el repo no distribuye código/artwork de terceros. |

Notas:
- Arduino solo compila el `.ino` del sketch; el respaldo quedó como `.txt`.
- Refactor de servicios globales: las clases ya no reciben `Display`/`Buttons`/`Sound`/`Buzzer` por constructor (los usan vía `Globals.h`, incluido solo en los `.cpp`). `Sound` es la **única** clase que usa `Buzzer`, y lo **posee por valor** (`Sound(uint8_t pin)`, inicializado en `begin()`); no hay `Buzzer buzzer;` global. Constructores con parámetros de configuración: `Menu(bestScore, version)`, `MenuCredits()`, `Food(cols, rows, top)`, `Sound(pin)`; `Game()`/`Boot()`/`Legend()` sin parámetros.

## 4. Pantalla: geometría

- Tamaño **128x64 px**. Celda **8x8** → rejilla **16×8**. `cellSize` configurable (default 8). Los métodos usan coordenadas de celda o píxel según el método.

### Fuente / tamaño de texto (Adafruit_GFX)

| Enumerador  | textSize | Dimensiones |
|-------------|----------|-------------|
| `TEXT_6x8`  | 1        | 6×8 px      |
| `TEXT_12x16` | 2       | 12×16 px    |
| `TEXT_18x24` | 3       | 18×24 px    |

---

## 5. Clase `Display` — API

`Display.h/.cpp`.

### 5.1 Constructor
```cpp
Display(uint8_t sda = Config::Pin::OLED_SDA, uint8_t scl = Config::Pin::OLED_SCL,
        uint8_t address = Config::Screen::ADDRESS, uint8_t width = Config::Screen::WIDTH,
        uint8_t height = Config::Screen::HEIGHT, uint8_t cellSize = Config::Screen::CELL);
```
Defaults de `Config` (sobreescribibles). La lista de inicialización sigue el orden de declaración de miembros (`_screen`, `_sda`, `_scl`, `_address`, `_width`, `_height`, `_cellSize`, `_columns`, `_rows`) para evitar `-Wreorder`.

### 5.2 Enums y struct
```cpp
enum TextSize { TEXT_6x8 = 1, TEXT_12x16 = 2, TEXT_18x24 = 3 };

enum TextAlign {
  LEFT_UP, CENTER_UP, RIGHT_UP,
  CENTER_LEFT, CENTER, CENTER_RIGHT,
  LEFT_DOWN, CENTER_DOWN, RIGHT_DOWN
};

struct TextPos { int16_t x; int16_t y; };  // esquina sup-izquierda del texto
```

### 5.3 Regiones
| Región | Zona |
|--------|------|
| `REGION_HEADER` | (0,0) - (128,15) |
| `REGION_BODY`   | (0,16) - (128,64) |
| `REGION_FULL`   | (0,0) - (128,64) |

### 5.4 Métodos
| Método | Descripción |
|--------|-------------|
| `void begin()` | `Wire.begin(sda,scl)`, reloj I2C **400 kHz** (`Wire.setClock(400000)`), crea el OLED y lo limpia. **Idempotente** (si `_screen != nullptr` no reasigna; si el primer intento falló, reintenta). |
| `void clear()` | Limpia el buffer. |
| `void show()` | Vuelca el buffer al OLED. |
| `void power(bool on)` | Apaga (`0xAE`) / enciende (`0xAF`) el panel SSD1306 (ahorro en reposo, §26). Conserva el framebuffer del panel (al reencender, misma imagen sin repintar). No afecta el buffer en RAM de `Adafruit_SSD1306`. |
| `void drawPixel(x, y, white=false)` | 1 píxel (`true`=blanco, `false`=negro). |
| `void fillRect(x, y, w, h, white=false)` | Rectángulo relleno (reenvío a `Adafruit_SSD1306::fillRect`; color como `bool white`). |
| `void fillTriangle(x0,y0,x1,y1,x2,y2, white=false)` | Triángulo relleno (reenvío a `Adafruit_GFX::fillTriangle`). Helpers viejos `sortByY()`/`edgeAt()` sin uso, pendientes de borrar. |
| `TextPos getTextPos(texto, align, size=1, region=FULL)` | x,y (esquina sup-izq) según alineación/región. |
| `void drawText(texto, x, y, size=1, tColor=SSD1306_WHITE, bgColor=SSD1306_BLACK)` | Texto en posición exacta. |
| `void drawTextInverted(texto, x, y, size=1)` | Texto negro sobre cualquier fondo. |
| `void drawTextAligned(texto, align, size=1, region=FULL)` | Alineado + imprime. |
| `void drawHighlight(texto, x, y, size=1)` | Texto resaltado (cuadro blanco + texto invertido). |
| `void drawHighlightAligned(texto, align, size=1, region=FULL)` | Igual, posicionado por alineación/región. |
| `uint8_t getTextWidth(texto, size=1)` | `strlen × 6 × size` (clamp 255). |
| `uint8_t getTextHeight(size=1)` | `8 × size` (clamp 255). |
| `uint8_t getWidth()` / `getHeight()` | 128 / 64. |
| `uint8_t getCellSize()` | 8. |
| `uint8_t getColumns()` / `getRows()` | 16 / 8. |
| `Adafruit_SSD1306& screen()` | Acceso al OLED. Referencia segura (no-opcional): si `begin()` falló, devuelve un OLED "mudo" en RAM; comprobar `isReady()`. |
| `bool isReady()` | `true` si el OLED respondió en `begin()`. Tras fallo, `screen()` sigue válido y el resto de métodos no hacen nada. |

### 5.5 Renderizado de texto y resaltado
- Normal (`drawText*`): blanco sobre negro, sin cuadro.
- Resaltado (`drawHighlight*`): cuadro blanco (`fillRoundRect`, radio 0) + texto invertido centrado. El cuadro rebasa `+2*size` px en X y `+1` arriba/abajo (`+2` en Y):
```
size=1 -> texto 6x8   -> cuadro  8x10
size=2 -> texto 12x16 -> cuadro 16x18
size=3 -> texto 18x24 -> cuadro 24x26
```
- No existen "botones no seleccionados": texto normal o resaltado.

---

## 6. Decisiones de diseño

- **`char*` en lugar de `String`** en `Display` (evita fragmentación del heap). Impresión con cadenas NULL-terminated.
- **Retornos `uint8_t`** para anchos/altos (clamp 255).
- **Ancho de texto = `strlen` (bytes):** con UTF-8 (á, é, ñ) sería incorrecto; hoy solo ASCII.
- **Posición de texto devuelta:** siempre esquina superior-izquierda.
- **`Display` encapsula el OLED** (el `.ino` usaba `Adafruit_SSD1306` directo).
- **Pie de archivo "Fin":** todos los `.h`/`.cpp` terminan con `// ===…` + `// Fin` + `// ===…` (línea en blanco antes, sin salto final), como el `.ino`.
- **Idioma del código: inglés** (identificadores; p. ej. `Menu::OPT_*`, `Config::Difficulty::*_LEVEL`, estados del `Engine`). Comentarios y `PROYECTO.md` en español.
- **Servicios globales, ventanas internas:** `Display`, `Buttons`, `Sound` (y `Storage`) son los únicos globales; el `Buzzer` no (lo contiene `Sound`); las ventanas viven dentro de `Engine` (§11). El orden de construcción no importa (cada servicio se inicializa en su `begin()`).

---

## 7. Clase `Buttons` — API

`Buttons.h/.cpp` (basada en `GameInput`, `D:\Documents\ESP32S3\Snake_2\GameInput.{h,cpp}`).

### Pines (orden del enum)
`Config::Pin::BUTTONS` (§19); el `.ino` los pasa: `Buttons buttons(Config::Pin::BUTTONS);`

### Enum
```cpp
enum Button : uint8_t {
  MOVE_UP = 0, MOVE_RIGHT, MOVE_DOWN, MOVE_LEFT,
  ACTION_UP, ACTION_RIGHT, ACTION_DOWN, ACTION_LEFT,

  MAX_BUTTONS   // contador final (= 8)
};
```

### Métodos
| Método | Descripción |
|--------|-------------|
| `Buttons(const int8_t* pins, uint32_t buttonDelay = 30)` | Pines + debounce (ms). |
| `void begin()` | `INPUT_PULLDOWN` y lee estado inicial. |
| `void read()` | Lee, filtra y genera eventos. **Una vez por `loop()`** (antes de `engine.update()`). |
| `bool hold(index)` / `pressed(index)` / `released(index)` | Acceso único por botón: `hold` = mantenido (antes `state()`), `pressed` = pulso del ciclo, `released` = liberación. Los 24 getters con nombre se eliminaron. |
| `bool anyHeld()` | Algún botón confirmado (sujeto a la supresión `ignoreUntilRelease()`). |
| `bool anyPhysical()` | Algún botón físicamente presionado (`_rawButtons`, sin debounce ni supresión). La usa el reposo (§26). |

### Diseño del debounce (estados agrupados en bytes)
- 8 botones en bytes (`uint8_t`, 1 bit por botón, HIGH = presionado): `_rawButtons` (físico), `_buttons` (confirmado), `_pressed`/`_released` (eventos de 1 ciclo). `_lastButtons` eliminado (el evento se detecta contra el estado confirmado previo).
- `bool isSet(uint8_t states, uint8_t boton)` → `states & (1 << boton)`: **declarado en `Buttons.h`** (`static bool`, **sin `inline`**) y **definido en `Buttons.cpp`** (quitar el `inline` resolvió el *undefined reference*).
- Cambio físico por bit (`raw ^ _rawButtons`); instante por botón; estado nuevo tras `_buttonDelay` ms estables. Almacenamiento: 4 bytes (antes 4 `bool[8]` + 1 byte).

### Anticonflicto MOVE
Con 2+ bits MOVE (`MOVE_MASK`, bots 0..3) confirmados a la vez, se anulan todos (al final de `read()`, sobre `_buttons`/`_pressed`/`_released`). ACTION no participa. Al liberar, si queda uno solo, se reactiva solo.

---

## 7. Clase `Menu` — API

`Menu.h/.cpp`.

### Constructor
```cpp
Menu(uint16_t bestScore = 0);
```
Servicios globales (§20); único parámetro: puntaje máximo. `_scroller()` por defecto. Texto del pie de `Config::Version::VERSION` (§19).

### Métodos
| Método | Descripción |
|--------|-------------|
| `void begin(bool showContinue = false, int8_t selected = OPT_NEW)` | Fija lista visible (5 con Continue / 4 sin él) y selección; reinicia flags (`_holdButtons`, `_lastScroll`, `_confirm`, `_done`, `_clear`); `_blink.start()`; `_repeat` listo. `_clear` → `clear()` + `firstPrint()` en el primer `print()`. |
| `void update()` | `if (_done) return;` luego `holdButtons()`, `navigate()`, `action()`; si el `Scroller` vuela (`_lastScroll`) lo avanza y **congela el resto del frame**; al terminar, `_blink.start()`. |
| `void print()` | `if (_done) return;` + `firstPrint()` (primer frame o `forceRedraw`: `clear()` + título, cuadro, texto, marcadores, pie), `nextOption()`, `_scroller.print()`, `blinkOption()`. |
| `void showOptions()` | Borra la banda entre cuadro y pie y dibuja marcadores de no seleccionadas + rombo de la seleccionada (omite `OPT_CONTINUE` si está oculta). |
| `int8_t selected()` | Opción seleccionada (enum `Option`). |
| `bool done()` | `true` cuando pide salir. Hoy **nada lo pone** (`_done = true` en `action()` comentado). |

**API en pausa (comentada en `Menu.h`):** `setOptions`, `setContinueAvailable`, `setBestScore`, `setTitle`, `setShowFooter`, `setSelected`, `confirm()`, `restoreDiamondBand()`.

### Internos (privados)
| Método | Descripción |
|--------|-------------|
| `void holdButtons()` | Anti-entrada: si `_holdButtons`, solo mira `buttons.hold()` de los 8; al liberarse **todos**, se apaga. Evita que entrar con un botón pulsado mueva la selección. |
| `void navigate()` | `MOVE_LEFT`/`MOVE_RIGHT` con `_repeat.step()` (paso inmediato + cada `TICK` tras `DELAY`); salta `OPT_CONTINUE` si `_showContinue` es `false`; `SFX_CLICK` al mover; límites `OPT_NEW`/`OPT_COUNT-1`. |
| `void action()` | `ACTION_UP` → vuelve a `OPT_CONTINUE`/`OPT_NEW` (`SFX_BACK`); `ACTION_RIGHT` → alterna `_confirm` (`SFX_CONFIRM`; salida comentada). |
| `void blinkOption()` | Con `_confirm` sale (`_confirm=false; _done=true`); si no: guards `if (!_scroller.done()) return;`, `else if (_lastSelected != _selected) return;`, `else if (!_blink.changed(PERIOD, OFF)) return;`. Si el `Scroller` vuela, borra el cuadro (`fillRect(0, BOX_TOP-1, WIDTH, BOX_HEIGHT+2)`) y lo recompone (`_scroller.begin()`); repinta texto + rombo (`toggleText` + `toggleDiamond`) con `visible = _done \|\| _blink.isVisible(PERIOD, OFF)`. |
| `void nextOption()` | Si cambió la selección: restaura marcadores anteriores (`toggleDiamond(_lastSelected,false)` + `toggleTriangle(_lastSelected,true)`), coloca los nuevos, compone `OPTION[_selected]` con `setTexto(..., BOX_TOP, _lastSelected > _selected, TEXT_12x16)` y `startSlide()`; marca `_lastScroll`. |
| `void toggleText(bool show)` | Pinta/borra el texto de la opción en el cuadro (`drawText`, `BOX_TOP`, `TEXT_12x16`). |
| `void toggleDiamond(int8_t, bool)` / `void toggleTriangle(int8_t, bool)` | Pinta/borra el marcador (centro `(i+1)·_space`, restando 1 si Continue oculta): `Draw::diamond` en `DIAMOND_Y` / `Draw::triangle` en `TRIANGLE_Y`; `toggleTriangle` sale si el índice es `OPT_COUNT`. |
| ~~`holdRepeat(uint8_t)`~~ | **Eliminado**: extraído a `ButtonRepeat` (§23, `_repeat`). |

### Las opciones "Difficulty" y "Sound" son ventanas
`confirm()` comentado y `Engine` aislado (§11): hoy nada abre estas ventanas; el comportamiento es el previsto al reactivarlas. Al confirmar, el `Engine` abre `MenuDifficulty`/`MenuSound` (hermanas, §11) en vez de editar inline. Ambas entran sobre el `Menu` ya dibujado, sustituyen solo la banda de rombos (45..53) y al salir el `Engine` vuelve al `Menu` sin `begin()`. La edición inline anterior (`beginSoundEdit`/`beginDifficultyEdit`, `drawSoundSelector`/`drawDifficultySelector`) se eliminó.

### Opciones y enum
```cpp
enum Option : uint8_t {
  OPT_NEW = 0, OPT_CONTINUE, OPT_DIFFICULTY, OPT_SOUND, OPT_CREDITS,

  OPT_COUNT
};
```
5 opciones lógicas (`OPT_COUNT` contador); `OPTION[OPT_COUNT]` = textos (antes `OPTION_TEXT`). Con Continue oculta, `navigate()` **salta `OPT_CONTINUE`**. `optionAt`/`indexOfOption` comentados.

### Diseño del menú (scroller de 1 bit)
- **Título:** `Config::Version::NAME` ("Snake II"), `TEXT_12x16`, centrado en el Header. Estático (una vez tras el `clear()` de `firstPrint()`, protegido por `_clear`).
- **Pie:** banda blanca `55..63` (`fillRect(0, FOOT_TOP-1, WIDTH, FOOT_H+1)`) con "Best " + puntaje (izq.) y versión (der.), `TEXT_6x8`, fila `FOOT_TOP=56`. Estático (`setBestScore` comentado).
- **Cuadro de selección:** fijo, ancho completo, centrado en mitad del Body: `BOX_HEIGHT=16`, `BOX_TOP=BODY_MIDDLE-BOX_HEIGHT/2=26` (blanco `25..42`). Texto `TEXT_12x16` en `BOX_TOP`; solo se toca al cambiar de opción o parpadear. **No se mueve.**
- **Animación (`Scroller`, §14):** 1 instancia (12x16). Cada opción se compone centrada en un array `int8_t` antes de mostrarse. `MOVE_RIGHT` → entra por la derecha; `MOVE_LEFT` → por la izquierda; 1 columna cada `ANIM_TICK` (~0,5 s total). En `Menu` la transición está **activa** (`nextOption()` + `startSlide()`); mientras `Scroller::update()` devuelve `true`, `Menu::update()` queda congelado.
- **Marcadores:** rombo completo del seleccionado (`Draw::diamond`, `DIAMOND_Y=49`, filas 46..52) y triángulo superior de los no seleccionados (`Draw::triangle`, base `TRIANGLE_Y=53`, vértice 50). Centro en `(i+1)·_space`, `_space=WIDTH/(OPT_COUNT + (showContinue?1:0))`, restando 1 a índices `>= OPT_CONTINUE` si Continue oculta. `blinkOption()` hace parpadear el **rombo** con `Blink` (`PERIOD`/`OFF` de `Config::DefaultTimer`), sin retardo inicial (`HOLD` comentado); `_blink` se ancla en `begin()` y al terminar cada vuelo.
- Primera y última opción no conectadas (navegación con límites).

---

## 8. Clase `Boot` — API

`Boot.h/.cpp`.

### Constructor
```cpp
Boot();
```
Sin parámetros (globales `Display`/`Buttons`, §20).

### Métodos
| Método | Descripción |
|--------|-------------|
| `void begin()` | `_blink.start()`, `_done=false`, `_clear=true`. `_holdMessage` + `expired(HOLD)` comentados. |
| `void update()` | Cualquier botón termina (`_done=true`) con sonido según botón (MOVE=`SFX_CLICK`, `ACTION_UP`=`SFX_BACK`, `ACTION_RIGHT`=`SFX_CONFIRM`, `ACTION_DOWN`/`ACTION_LEFT`=`SFX_CLICK`). Mensaje parpadea desde el primer frame (`PERIOD=500`, `OFF=20%`). **Sin autoavance**. |
| `void print()` | `firstPrint(forceRedraw)`: `clear()` + `drawMessage(true)` (Header) + logo (Body) + `_blink.start()`; luego `blinkMessage()` (repinta si `_blink.changed(PERIOD, OFF)`). |
| `bool done()` | `true` al pulsar cualquier botón (`Engine` → `LEGEND`). |

### Pantalla de arranque (mensaje + logo)
- Header (0..15): `MESSAGE` = "Press any button" / "to start" (2 líneas `TEXT_6x8`) que parpadea con `Blink`.
- Body (16..63): fondo blanco + logo `Sprite::LOGO` (80×48) centrado, `drawBitmap(...,true,false)`. El logo guarda polaridad original (1=fondo, 0=glifo) → glifo negro sobre fondo blanco.
- Animación de bandas de líneas verticales (`BAR_W`, `BAR_SPACING`, `ANIM_TICK`, `TOTAL_MS`, `drawBars`, `drawBar`/`eraseOldBars`/`eraseBarDiff`) **comentada**.

---

## 9. Clase `Legend` — API

`Legend.h/.cpp`.

### Constructor
```cpp
Legend();
```
Globales `Display`/`Buttons`/`Sound` (§20).

### Métodos
| Método | Descripción |
|--------|-------------|
| `void begin()` | `_btn`/`_lastBtn=0`, arranca `_ticker` y `_blink`, `_lastScroll=true`, `_done=false`, `_clear=true`. |
| `void update()` | Sonido según botón (`done()=true`): `MOVE_*`=`CLICK`, `ACTION_UP`=`BACK`, `ACTION_RIGHT`=`CONFIRM`, `ACTION_DOWN`/`ACTION_LEFT`=`CLICK`. Rombo activo avanza cada `NEXT=2500 ms` (`_ticker`); parpadeo por frame `_blink.isVisible(PERIOD=100ms, OFF=50%)` (~10 Hz). Si el `Scroller` del pie vuela, el ciclo se **congela**; al terminar, reancla `_ticker` y `_blink.start()`. |
| `void print()` | `firstPrint(forceRedraw)`: `clear()` + banda `3..11` "Move"/"Action", pad MOVE, 4 rombos ACTION, banda del pie con `BTN_FUNC[0]`; reancla. Por frame: `nextBtn()`/`_scroller.print()` + `blinkDiamond()`. |
| `bool done()` | `true` al ir al menú. La `Legend` ya reprodujo su sonido (`Engine` no añade `SFX_BACK`). |

### Dibujo (leyenda de botones)
- Título (Header): banda blanca con "Move" (izq.) y "Action" (der.) en `TEXT_6x8` negro sobre blanco; estático.
- Pad MOVE (izq.): 4 flechas (`Draw::triangle`, lado `Config::Diamond::SIZE=3`) en cruz en `(PAD_LEFT_X=32, PAD_Y=33)`, radio `PAD_RADIO=10`.
- Pad ACTION (der.): 4 rombos completos (`Draw::diamond`, `SIZE=3`) en cruz en `(PAD_RIGHT_X=96, PAD_Y=33)`, radio 10: ↑`Btn1`, →`Btn2`, ↓`Btn3`, ←`Btn4`.
- Parpadeo: rombo activo recorre `Btn1→Btn4` en ciclo lento (`NEXT=2500 ms`) y parpadea (`PERIOD=100 ms`, `OFF=50%`); `blinkDiamond()` repinta cuando `_blink.changed(...)` (sale si `!_scroller.done()` o `_lastBtn != _btn`). Los inactivos quedan fijos; al cambiar se restaura el anterior completo (`toggleDiamond(_lastBtn,true)`).
- Pie: banda blanca `FOOT_TOP-1..63` con `BTN_FUNC[_btn]` `TEXT_6x8` centrado en `FOOT_TOP=56`: `Btn1`="Btn1 / Back", `Btn2`="Btn2 / Select / Pause", `Btn3`="Btn3 / None", `Btn4`="Btn4 / None". Entra deslizándose (`Scroller` propio `_scroller`, `setTexto(BTN_FUNC[_btn], FOOT_TOP, true, TEXT_6x8)` + `startSlide()`, por la derecha). Mientras vuela, el ciclo se congela. `PIE_TOP=57` anterior comentado.
- Salida: cualquier botón cierra con su sonido. Solo se muestra al arranque.
- Render: estáticos una vez (`firstPrint()`); por frame solo el rombo activo y el texto del pie.

## 10. Clase `Buzzer` — API (capa de hardware)

`Buzzer.h/.cpp`. **Un solo tono a la vez**, **no bloqueante**: `tone()` enciende y marca duración; `update()` (vía `Sound::update()`) lo apaga al agotarse. LEDC (Core 3.x). Sin `delay()`. **No es global:** miembro por valor de `Sound` (`Buzzer _buzzer;`), construido con el pin recibido; `Sound::begin()` llama su `begin()`. API pública pero encapsulada por `Sound`.

### Constructor
```cpp
Buzzer(uint8_t pin = Config::Pin::BUZZER);   // pin 14
```

### Métodos
| Método | Descripción |
|--------|-------------|
| `bool begin()` | `ledcAttach(pin, 2000, 10)` y silencia. Devuelve si adjuntó el canal. |
| `void update()` | Apaga el tono al agotarse su duración. Una vez por `loop()` (ya lo hace `Sound::update()`). |
| `void tone(freq, durMs = 0)` | Emite tono no bloqueante. `durMs > 0` lo apaga solo; `0` = suena hasta `stop()`. `freq = 0` = silencio (espera activa). |
| `void stop()` | Silencia y cancela la duración pendiente. |
| `bool busy()` | `true` mientras hay tono/duración en curso. |
| `bool attached()` | `true` si `begin()` adjuntó el canal LEDC. |

---

## 10.2 Clase `Sound` — API (sonidos del juego)

`Sound.h/.cpp`. Efectos = **secuencias de tonos** (`Note` = `{freq, durMs}`, `0` = silencio) sobre su **`Buzzer` interno** (por valor, inicializado en `begin()`). No bloqueante: `play()` arranca y `update()` avanza nota a nota. Desactivado, `play()` no hace nada (`setEnabled(false)`).

Las 12 secuencias `SEQ_*` son **miembros estáticos privados** (`Sound.h`, definidos en `Sound.cpp`) indexadas por una **única tabla `EFFECTS[]` de `{const Note* notes, uint8_t len}`** (orden = enum `Sfx`, `SFX_NONE` incluido con secuencia vacía): reemplaza a las 12 `LEN_*` (largo por `sizeof`) y al `switch` de `play()`, que ahora lee `EFFECTS[effect]`. Detalle de implementación: la API pública no expone `Note`, `Seq` ni tablas.

### Constructor
```cpp
explicit Sound(uint8_t pin = Config::Pin::BUZZER);
```
`Buzzer` por valor (miembro `_buzzer`, primero en la lista de inicialización): sin `Buzzer&` ni global.

### Enum y efectos
```cpp
enum Sfx : uint8_t {
  SFX_NONE = 0, SFX_CLICK, SFX_CONFIRM, SFX_BACK,
  SFX_EAT, SFX_START, SFX_LEVEL_UP, SFX_GAME_OVER,
  SFX_TICK, SFX_TURN, SFX_PAUSE, SFX_RESUME,
  SFX_NEW_BEST, SFX_BOOT, SFX_COUNT   // SFX_COUNT = tamaño de EFFECTS
};
```

| Efecto | Uso | Secuencia (Hz/ms) |
|--------|-----|-------------------|
| `SFX_CLICK` | Navegar el menú | 1800/35 |
| `SFX_CONFIRM` | Activar una opción | 700/50, 1000/80 |
| `SFX_BACK` | Volver al menú | 900/50, 600/90 |
| `SFX_EAT` | Comer | 988/60, 1319/100 |
| `SFX_START` | GO! al iniciar | 800/60, 1100/60, 1500/150 |
| `SFX_LEVEL_UP` | Subir de nivel | 523/60, 659/60, 784/60, 1047/120, 1319/180 |
| `SFX_GAME_OVER` | Muerte | 800/100, 650/100, 500/150, 300/300 |
| `SFX_TICK` | Conteo 3-2-1 (un pitido por dígito) | 900/40 |
| `SFX_TURN` | Cambio de dirección | 1319/20 |
| `SFX_PAUSE` | Pausar | 600/50, 300/60 |
| `SFX_RESUME` | Reanudar | 500/50, 900/60 |
| `SFX_NEW_BEST` | Festejo de récord (suena en "THE BEST") | 523/100, 659/100, 784/100, 1047/140, 784/100, 1047/140, 1319/420 |
| `SFX_BOOT` | Fanfarria de arranque (en `Boot`) | 523/120, 659/120, 784/120, 1047/180, 784/120, 1047/180, 1319/360 |

Paleta del `GameBuzzer` original.

### Métodos
| Método | Descripción |
|--------|-------------|
| `void begin()` | `_buzzer.begin()` (adjunta LEDC) y luego `stop()`. Único lugar que inicializa el `Buzzer`. |
| `void setEnabled(bool)` / `enabled()` | Activa/desactiva (opción "Sound"); al desactivar corta el efecto en curso. |
| `void play(Sfx)` | Arranca la secuencia (no bloqueante). |
| `void update()` | Avanza a la siguiente nota (llama `_buzzer.update()` antes). Una vez por `loop()`. |
| `void stop()` | Corta el efecto y silencia. |
| `bool playing()` | `true` mientras suena un efecto. |

En el `.ino`: `Sound sound(Config::Pin::BUZZER);`; `sound.begin()` en `setup()`, `sound.update()` en `loop()`.

---

## 10.3 Ventanas del menú: `MenuDifficulty` y `MenuSound`

Las opciones **"Difficulty"** y **"Sound"** son ventanas del `Engine` (como `MenuCredits`; históricamente `SoundWindow` y luego edición *inline* en `Menu`, ambas eliminadas). `Menu` las devuelve en `confirm()` y el `Engine` abre la correspondiente (`OPT_DIFFICULTY` → `MENU_DIFFICULTY`, `OPT_SOUND` → `MENU_SOUND`). **Estado actual:** ambas **fuera del aislamiento** (el `Engine` las posee y despacha), pero `Menu::confirm()` y la entrada desde el `Menu` siguen comentadas → no se abren todavía.

Esqueleto común (`begin`/`update`/`print`/`done`), banda de dibujo **45..53** (la de los rombos del `Menu`) y regla: **no borran la pantalla**, solo sustituyen esa franja. Al salir, el `Engine` llama `Menu::restoreDiamondBand()` y `changeState(State::MENU, false)` (sin `clear()` de `Menu::begin()`). Hoy, con `restoreDiamondBand()` comentada, el regreso hace el `begin()` completo (`changeState(MENU, false)` cae en la llamada incondicional a `_menu.begin()`).

### `MenuDifficulty` — nivel 1..10

| Miembro | Descripción |
|---------|-------------|
| `MenuDifficulty()` | `_clear=true`, `_done=false`, `_difficulty=_selected=_lastSelected=Config::Difficulty::DEFAULT_LEVEL` (5). |
| `void begin()` | `_clear=true`, `_done=false`, `_selected=_difficulty`, `_lastSelected=_selected`, `_blink.start()`. **No dibuja ni borra** (entra sobre el `Menu`). |
| `void update()` | `MOVE_RIGHT` +1 / `MOVE_LEFT` −1 con repetición (`_repeat.step()`), tope `MIN_LEVEL`/`MAX_LEVEL`, cada paso `SFX_CLICK`. `ACTION_RIGHT` **aplica** (`_difficulty=_selected`) y `done()`; `ACTION_UP` cancela. |
| `void print()` | `firstPrint()` + `nextOption()` (redibuja si cambió `_selected`) + `blinkOption()`. |
| `void firstPrint(bool forceRedraw=false)` | Primer frame (o forzado): `clear()` + `nextOption(true)`. |
| `void clear()` | Borra la banda 45..53 (`fillRect(0, VALUE_TOP, WIDTH, VALUE_HEIGHT, false)`). |
| `void nextOption(bool show=false)` | Si `_lastSelected==_selected` y no `show`, sale; si no, dibuja el número (`" 5"`/`"10"`, ancho constante 12 px, `LABEL_X`, `TEXT_6x8`) y **ambas** flechas (`Draw::triangle` en `TRIANGLE_L`/`TRIANGLE_R`): visibles salvo en su límite; al mantener `MOVE_LEFT`/`MOVE_RIGHT` solo la activa (la contraria se oculta); actualiza `_lastSelected` y reancla `_blink`. |
| `void blinkOption()` | Cuando `_blink.changed(PERIOD, OFF)`, repinta las flechas con `Draw::triangle(..., _blink.isVisible(PERIOD, OFF))` (la del botón mantenido fija; la del límite no se dibuja). El número no parpadea. |
| `bool done()` | `true` tras confirmar o cancelar. |
| `uint8_t difficulty()` | Nivel **confirmado**; lo lee el `Engine` en `changeState(NEW/CONTINUE)` → `Game::setDifficulty`. |
| `_difficulty` | `uint8_t`: nivel confirmado (sigue en la ventana; aún no migrado a `Storage`). |
| `_selected`/`_lastSelected` | `uint8_t`: nivel en edición / último mostrado. |
| `_blink` | `Blink`: fase del parpadeo (antes `Stopwatch`). |
| `_repeat` | `ButtonRepeat`: repetición de `MOVE_LEFT/RIGHT` (`Config::Button::DELAY/TICK`; se eliminaron `HOLD_REPEAT_DELAY`/`HOLD_REPEAT_TICK`). |
| ~~`drawSelector()`~~ / ~~`holdRepeat()`~~ | **Eliminados** (dibujo repartido en `nextOption`/`blinkOption`; repetición extraída a `ButtonRepeat`). |

### `MenuSound` — On/Off

| Miembro | Descripción |
|---------|-------------|
| `MenuSound()` | `_clear=true`, `_done=false`, `_selected=true`, `_lastSelected=true`. |
| `void begin()` | `_clear=true`, `_done=false`, `_selected=sound.enabled()`, `_lastSelected=_selected`, `_blink.start()`. **No dibuja ni borra**. |
| `void update()` | Solo la tecla del lado de la flecha: `MOVE_LEFT` apaga si `_selected` ON y `MOVE_RIGHT` enciende si OFF (cada uno `SFX_CLICK`). `ACTION_RIGHT` **aplica** (`sound.setEnabled(_selected)`, `SFX_CONFIRM` solo si queda ON) y `done()`; `ACTION_UP` cancela. |
| `void print()` | `firstPrint()` + `nextOption()` (redibuja si cambió `_selected`) + `blinkOption()`. |
| `void firstPrint(bool forceRedraw=false)` | Primer frame (o forzado): `clear()` + `nextOption(true)`. |
| `void clear()` | Borra la banda 45..53. |
| `void nextOption(bool show=false)` | Si `_lastSelected==_selected` y no `show`, sale; si no, dibuja `"ON "`/`"OFF"` en `LABEL_X` (`TEXT_6x8`) y **ambas** flechas: visible la del lado del destino (`_selected` → izquierda) y oculta la contraria; actualiza `_lastSelected` y reancla `_blink`. |
| `void blinkOption()` | Cuando `_blink.changed(PERIOD, OFF)`, repinta solo la flecha del destino (`_selected` → `TRIANGLE_L`, izquierda; si no → `TRIANGLE_R`) con `Draw::triangle(..., _blink.isVisible(PERIOD, OFF))`. La palabra no parpadea. |
| `bool done()` | `true` tras confirmar o cancelar. |
| `_blink` | `Blink`: fase del parpadeo (antes `Stopwatch`). |

El valor del sonido **no es propio de la ventana**: vive en el servicio global `Sound` (`setEnabled`/`enabled`), así que `MenuSound` solo lleva el valor en edición y no lo recuerda entre entradas (a diferencia de `MenuDifficulty`, que guarda el nivel confirmado).

Geometría y parpadeo de ambas (`WIDTH`, `SIZE`, `PERIOD`/`OFF`, `VALUE_TOP`/`VALUE_HEIGHT`, `DIAMOND_Y`, `TRIANGLE_L`/`TRIANGLE_R`/`LABEL_X`) son `static constexpr` de cada clase (convención de `Config.h`: allí solo lo compartido). La banda 45..53 es la de los rombos del `Menu`; su erase incluye la fila inferior, que por eso `Menu::restoreDiamondBand()` vuelve a pintar (con la línea del pie `Config::Screen::FOOT_LINE`, hoy comentada). `MenuDifficulty` añade alias de clase `DEFAULT_LEVEL`/`MIN_LEVEL`/`MAX_LEVEL` (valores de `Config::Difficulty`). La repetición al mantener usa `Config::Button`.

## 11. Clase `Engine` — despachador de ventanas

`Engine.h/.cpp` (antes `App`). **No anida** las ventanas pero las **posee como miembros**: `Boot`, `Legend`, `Menu`, `MenuCredits`, `Game` son clases independientes (hermanas) declaradas como `_boot`, `_menu`, `_menuDifficulty`, `_menuSound`, `_menuCredits`, `_game`, `_legend`. No son globales ni se pasan por referencia. **Estado actual (aislado):** activos `_boot`, `_legend`, `_menu`, `_menuSound`, `_menuDifficulty`; comentados (bloques `AISLADO`) los `#include`/miembros `_game` y `_menuCredits` y los estados `NEW`, `CONTINUE` y `MENU_CREDITS`. `MENU_SOUND`/`MENU_DIFFICULTY` activos, pero la entrada desde el `Menu` sigue sin cablear (`Menu::confirm()` comentado).

### Responsabilidad
**Despachador puro**: su **estado interno** (`enum class State`) decide qué ventana corre y cuándo cambiar (`changeState()`, que llama al `begin()` de la ventana entrante). `loop()` no participa en transiciones: solo `engine.update()`/`engine.print()`. `setup()` inicia el hardware y llama `engine.begin()` (primera transición → `boot.begin()`).

### Constructor
```cpp
Engine();
```
Sin parámetros: consume los servicios globales y posee sus ventanas. En el `.ino` los globales se definen **antes** de `Engine engine;`, así los constructores de miembros (p. ej. `Scroller`/`Food` que consultan `display.getWidth()`) ven los globales ya construidos (misma TU, orden de definición).

### Reglas de esta arquitectura
1. **Todas las clases se inician en `setup()`** y se usan en `loop()`/`Engine`. Constructores livianos (las ventanas no guardan referencias: usan globales); el trabajo real va en `begin()`/`update()`; `begin()` de ventanas lo llama `changeState()` al entrar (el primero dentro de `setup()` vía `engine.begin()`).
2. **Las ventanas NO se anidan; el `Engine` las posee:** cada una implementa `begin()/update()/print()/done()` (sin código inline en `Engine`). Son **miembros del `Engine`** (no globales): ninguna otra clase puede invocarlas.
3. **El estado determina qué se ve** (solo `Engine` conoce `State`) **y los valores se conservan**: las ventanas son instancias persistentes (hermanas, no se recrean), así sus miembros sobreviven entre transiciones; `begin()` solo reinicia lo necesario al entrar.
4. **Lectura de botones centralizada:** el hardware se lee **una sola vez por frame** en `loop()` (`buttons.read()`). Ninguna ventana llama a `read()`; todas consumen `pressed`/`released` de esa lectura (varias partes comparten el estado del frame sin "comerse" eventos).
5. **Ventanas "capa" no limpian la pantalla:** `MenuDifficulty`/`MenuSound` son capas sobre el `Menu` ya dibujado; su `begin()` no dibuja nada (su `print()` borra y repinta solo la banda de rombos) y su regreso previsto es `menu.restoreDiamondBand()` + `changeState(State::MENU, false)` (el `false` salta `Menu::begin()`). **Hoy** `restoreDiamondBand()` y la guarda `if (beginWindow)` siguen comentadas → el regreso hace igual el `begin()` completo.

### Estados internos
```cpp
enum class State : uint8_t {
  BOOT = 0,
  LEGEND,
  MENU,
  MENU_SOUND,
  // NEW,            // AISLADO: comentados junto con sus ventanas
  // CONTINUE,
  MENU_DIFFICULTY
  // MENU_CREDITS
};
```

| Estado | Ventana | Notas |
|--------|---------|-------|
| `BOOT` | `Boot` | Arranque: logo + mensaje parpadeante. Al `done()` (cualquier botón) → `LEGEND`. |
| `LEGEND` | `Legend` | Pad MOVE (4 flechas) + 4 rombos ACTION que parpadean rápidos uno a la vez (ciclo lento) con la función del rombo activo en el pie (`Scroller` propio) que entra deslizándose y congela el ciclo mientras se mueve. Cualquier botón la cierra → menú (efecto según botón; `Engine` no añade `SFX_BACK`). Solo tras el arranque. |
| `MENU` | `Menu` | **Aislado:** `ACTION_RIGHT` no sale (`confirm()` y el `switch` de destinos comentados). Solo navega con MOVE (`SFX_CLICK`); `ACTION_RIGHT` alterna `_confirm` (`SFX_CONFIRM`, opción marcada/parpadeando); `ACTION_UP` → `New`/`Continue` (`SFX_BACK`). |
| `NEW` | `Game` | **Comentado (aislado).** Previsto: `setDifficulty(_menuDifficulty.difficulty())` + `begin(true)`, con conteo 3-2-1 (`SFX_TICK` por dígito). Al `done()`: `SFX_BACK`, sincroniza récord (`menu.setBestScore`), **oculta/muestra "Continue"** (`menu.setContinueAvailable(resumable)`, con `resumable = !game.isGameOver() && game.score() > 0`), deja la selección en `Continue` si `resumable` o en `New` (`menu.setSelected(...)`) → `MENU`. |
| `CONTINUE` | `Game` | **Comentado (aislado).** Previsto: reanudar en pausa (`begin(false)`); se retoma con `ACTION_RIGHT` (Btn2) o `ACTION_LEFT`; si no hay partida en curso, arranca nueva. Al `done()`, igual que `NEW`. |
| `MENU_DIFFICULTY` | `MenuDifficulty` | **Activo** (entrada desde el `Menu` sin cablear). Selector 1..10 **sobre el `Menu`**: borra/repinta la banda 45..53 con `< N >`. `MOVE_LEFT/RIGHT` editan con repetición (`SFX_CLICK` por paso), `ACTION_RIGHT` aplica, `ACTION_UP` cancela. Al `done()` → `changeState(MENU)`; el nivel queda en `_difficulty`. |
| `MENU_SOUND` | `MenuSound` | **Activo** (entrada sin cablear). Selector On/Off sobre el `Menu`, misma banda. `MOVE_LEFT` apaga / `MOVE_RIGHT` enciende (`SFX_CLICK`), `ACTION_RIGHT` aplica (`sound.setEnabled()`, `SFX_CONFIRM` solo si queda ON), `ACTION_UP` cancela. Al `done()` → `changeState(MENU, false)` (hoy `begin()` completo por `restoreDiamondBand()` comentada). El valor real vive en `Sound`. |
| `MENU_CREDITS` | `MenuCredits` | **Comentado (aislado).** Previsto: 3 entradas navegables con `MOVE_LEFT/RIGHT` y transición lateral (rol tamaño 2 con cuadro borde a borde, centrado en el alto del Body; nombre tamaño 1 plano en el pie). Usa el **mismo `Scroller` que el menú** con **2 instancias sincronizadas** (`_scrollerRol` 12x16 + `_scrollerNombre` 6x8): cada banda tiene su tira compuesta **solo al entrar/navegar** (`loadEntry()` → `setTexto`, que fija la fila: `roleY()`/`nameY()`); cada `Scroller` rellena su banda antes de volcar (superposición al navegar rápido; `print()` no pinta fondo ni llama `redraw()`). Deslizamiento desde el borde (`setTexto(..., true, ...)` + `startSlide()`), 1 px/4 ms (~0,5 s). `SFX_CLICK` al navegar y `SFX_BACK` al salir; pasa directo a `MENU`. |

### Métodos
| Método | Descripción |
|--------|-------------|
| `void begin()` | Primera transición: `changeState(State::BOOT)`. Se llama desde `setup()`. |
| `void update()` | Actualiza la ventana activa y gestiona transiciones. Hoy `BOOT → LEGEND → MENU` + despacho de `MENU_SOUND`/`MENU_DIFFICULTY` (alcanzables solo entrando por código). Reproduce `SFX_CONFIRM` al confirmar una opción y `SFX_BACK` al volver a `MENU` desde cualquier ventana (salvo `Legend`, que toca el suyo, y `MENU_DIFFICULTY`/`MENU_SOUND`, que no lo añaden: cada selector toca su propio `SFX_CONFIRM` al aplicar o `SFX_BACK` al cancelar) — **esa entrada sigue comentada** por el aislamiento. |
| `void print()` | Despacha el dibujo a la ventana activa. **Ya no limpia la pantalla**: cada ventana limpia solo en su primer frame tras `begin()` y luego solo sus zonas dinámicas (§13). |
| `bool isInGame()` | `true` si hay partida en curso (el reposo no aplica, §26). Hoy siempre `false` (**AISLADO**); al reactivar: `_state == State::NEW \|\| _state == State::CONTINUE`. |
| `void setBestScore(uint16_t)` | **Comentado** (aislado): reenviaría al menú para conservar el récord. |
| `void changeState(State, bool beginWindow = true)` | Transición común: guarda el estado y llama `begin()` de la ventana destino. `MENU_SOUND`/`MENU_DIFFICULTY` ya la usan. **Hoy `_menu.begin()` es incondicional** (guarda `if (beginWindow)` comentada): `beginWindow=false` está deshabilitado → el regreso de `MenuSound` hace el `clear()` completo. El resto del cableado (p. ej. `setDifficulty` al entrar en `Game`) también está comentado. No reproduce sonidos. |

### Patrón de ventana
| Método | Descripción |
|--------|-------------|
| `begin()` | Restablece la ventana al entrar. `Engine` lo llama solo en `changeState()`. |
| `update()` | Maneja eventos (leídos una vez por `loop()` en `buttons.read()`; la ventana no llama a `read()`). |
| `print()` | Dibuja. `clear()` completo en el primer frame; luego solo zonas dinámicas. |
| `done()` | `true` cuando la ventana pide volver al menú. |

### Reglas del despachador
1. Solo `Engine` cambia de estado (nadie más conoce `State`).
2. Una ventana nunca cambia de estado ni conoce a las demás: expone `done()`.
3. `ACTION_UP` es el botón común "volver al menú" en todas las ventanas.
4. `ACTION_RIGHT` activa la opción del menú (su `confirm()`) — **hoy `confirm()` está comentado**: solo alterna `_confirm` dentro del `Menu`.
5. `begin()` de cada ventana se llama desde `changeState()`, nunca desde `loop()`.

## 12. Cómo compilar/probar

- IDE: Arduino IDE, placa `ESP32-S3 (Dev Module)` (verificar puerto).
- Librerías: Adafruit GFX + Adafruit_SSD1306.
- **IntelliSense en VS Code** (solo extensión Microsoft C/C++, sin Arduino/PlatformIO): el `<Arduino.h>` marcado como error lo resuelven las rutas del core; **dónde van depende de cómo se abra el proyecto**:
  - Abierto con el workspace **`D:\Documents\ESP32S3\ESP32S3.code-workspace`** (raíz `.` = `D:\Documents\ESP32S3`, con `Snake_II-OLED_128x64-I2C` y `blink`). En un `.code-workspace` **no** se lee el `c_cpp_properties.json` de subcarpeta: la config va en el bloque `settings` con claves `C_Cpp.default.*` (`compilerPath`, `includePath`, `defines`, `cppStandard`, `cStandard`, `intelliSenseMode`). Ese archivo está **fuera del repo** (no se versiona).
  - Abriendo la carpeta `Snake_II-OLED_128x64-I2C` sola (File > Open Folder) manda `.vscode/c_cpp_properties.json` (config `ESP32-S3 Dev Module (arduino-esp32 3.3.12)`, equivalente a las `C_Cpp.default.*` del workspace). Es la copia que **sí** queda en el repo.
  - Rutas:
    - `compilerPath` al toolchain real: `packages/esp32/tools/esp-x32/2601/bin/xtensa-esp-elf-g++.exe` (de ahí la extensión saca los includes de sistema: libstdc++, newlib, xtensa).
    - Core: `cores/esp32` (Arduino.h), `variants/esp32s3` (pins_arduino.h), `libraries/Wire/src`, `libraries/SPI/src` (los incluye `Adafruit_SSD1306.h`).
    - SDK ESP-IDF: `tools/esp32s3-libs/3.3.12/qio_qspi/include` (`sdkconfig.h`: `CONFIG_XTAL_FREQ`, `CONFIG_MMU_PAGE_SIZE`...) y `tools/esp32s3-libs/3.3.12/include/**` recursivo (reproduce los ~200 `-iwithprefixbefore` del core).
    - Sketchbook `D:\Documents\Arduino\libraries`: `Adafruit_GFX_Library`, `Adafruit_SSD1306`, `Adafruit_BusIO` (esta la necesita `Adafruit_GFX.h`).
    - `cppStandard: c++20` (core con `-std=gnu++2a`) y `-D`: `F_CPU`, `ARDUINO`, `ARDUINO_ARCH_ESP32`, `ARDUINO_ESP32S3_DEV`, `ARDUINO_BOARD`, `ARDUINO_VARIANT`, `ARDUINO_USB_CDC_ON_BOOT=0`, `ESP_PLATFORM`... **Sin `ESP32=ESP32` no arranca:** viene de `build.extra_flags` de `platform.txt` y hace que `Adafruit_SPIDevice.h` tome su rama ESP32; sin él cae en `typedef BitOrder BusIOBitOrder` y el core 3.3.12 ya no define ese tipo (usa `SPI_MSBFIRST`).
    - En el workspace `${workspaceFolder}` = `D:\Documents\ESP32S3`; la raíz del proyecto va como `${workspaceFolder}/Snake_II-OLED_128x64-I2C`.
    - `.vscode/settings.json` deja el `.aider-venv` (y su caché) fuera de la indexación/búsquedas.
    - Rutas absolutas: al actualizar el core o el toolchain revisar `3.3.12` y `2601` y tocarlos **en los dos sitios**.
- **Warnings C++ activados** vía `platform.local.txt` del core ESP32 (`compiler.cpp.extra_flags=-Wall -Wreorder`) para que desajustes (p. ej. orden de inicialización de miembros) salten en la compilación (no entra en el repo).
- La demo (`Snake_II-OLED_128x64-I2C.ino`) usa `Display`, `Buttons` y el sonido: el menú arranca **sin "Continue"** (solo aparece al volver del juego con partida viva **y puntos**, score > 0). Navegar menú/créditos = `SFX_CLICK`, confirmar = `SFX_CONFIRM`, volver = `SFX_BACK`; `Legend` según botón (MOVE=CLICK, ACTION_UP=BACK, ACTION_RIGHT=CONFIRM). "Sound" se edita en `MenuSound` (flechas en la banda de rombos; `MOVE_LEFT/RIGHT` cambian, `ACTION_RIGHT` aplica, `ACTION_UP` cancela). "Dificultad" en `MenuDifficulty` (`< N >`, dos flechas que se ocultan en los límites; `MOVE_RIGHT` +1, `MOVE_LEFT` −1 con repetición al mantener —al mantener se detiene el parpadeo y queda fija la flecha del botón activo—, `ACTION_RIGHT` aplica, `ACTION_UP` cancela). En `Game`: `SFX_TICK` por dígito del conteo, `SFX_START` (GO!) tras el "1", `SFX_TURN` al girar, `SFX_PAUSE`/`SFX_RESUME`, `SFX_EAT`, `SFX_GAME_OVER`, `SFX_BACK` al volver.
- SDA=8, SCL=9, dirección 0x3C.

---

## 13. Esquema de renderizado y limpieza (sin `clear()` global)

El juego ya no limpia la pantalla completa cada frame: `Engine::print()` **no** llama `display.clear()`; lo decide cada ventana.

### Reglas
1. **Clear completo solo al entrar:** `display.clear()` en el **primer `print()`** tras el `begin()` (flag `_clear` puesto en `begin()`, apagado tras ese dibujo; el flag antiguo `_redraw` está comentado). Elimina la resaca anterior. Los métodos que lo reactivaban (`Menu::setOptions/setSelected/setContinueAvailable`, `setBestScore` si cambia) están **comentados**; hoy solo lo activa `begin()`.
2. **Estáticos una sola vez:** títulos, pies, línea separadora, cuadro de selección, rótulos y pads se dibujan en ese primer frame y **no se vuelven a dibujar**; persisten en el buffer (que `display.show()` vuelca completo cada frame).
3. **Dinámicos por frame, borrando solo lo necesario:**

| Ventana | Estáticos (una vez) | Dinámicos por frame |
|---------|---------------------|---------------------|
| `Boot` | clear + `firstPrint()` (banda blanca del mensaje en el Header + logo en el Body) | Solo el mensaje del Header cuando `_blink.changed(PERIOD, OFF)` (`blinkMessage()`: repinta `TEXT_6x8` blanco o borra en negro); el logo no se toca |
| `Menu` | Cuadro blanco `25..42`, texto de la opción en `26..41`, título y pie (banda blanca `55..63` con "Best" + versión) | Banda de la opción (`Scroller::print` con vuelo **activo** en `nextOption()`) + marcadores (rombo del seleccionado `46..52` y triángulos superiores `50..53` de los no seleccionados) + parpadeo del rombo y el texto (`blinkOption()` según `_blink.changed`/`isVisible`, y reafirma visible si `_confirm`) |
| `MenuDifficulty` | — (entra sobre el `Menu`, no borra nada) | Selector `< N >`: `firstPrint()`/`clear()` borran la banda 45..53 y cada frame `nextOption()`/`blinkOption()` repintan el número (centrado, ancho constante, no parpadea) y las dos flechas, que parpadean juntas. Ocultas en su límite; al mantener, se detiene el parpadeo y queda fija la del botón activo. |
| `MenuSound` | — (entra sobre el `Menu`, no borra nada) | Selector ON/OFF: borra y redibuja **cada frame** la banda 44..54 (palabra centrada estática + flecha única en el lado del destino, que parpadea) |
| `MenuCredits` | Título del Header (el cuadro blanco del rol lo pinta el propio `Scroller` al rellenar su banda) | Bandas rol/nombre (`Scroller`, 2 instancias sincronizadas; cada una rellena su banda y se vuelcan juntas en el mismo frame, y solo si hay algo nuevo que pintar, §14) |
| `Legend` | clear + banda de título "Move"/"Action", pad MOVE, los 4 rombos de ACTION y la banda del pie con `BTN_FUNC[0]` | Rombo activo (`blinkDiamond()`/`toggleDiamond` cuando `_blink.changed(PERIOD, OFF)`) + texto del pie (`Scroller` propio): al cambiar de rombo entra deslizándose (`setTexto()` + `startSlide()`); mientras se mueve, el ciclo se congela (`_scroller.update()` devuelve `true`) y al terminar se reanclan `_ticker` y `_blink.start()`. Al cambiar, se restaura completo el rombo que deja de ser activo. |
| `Game` | clear + Header (puntaje 12x16 izq., segundos de la comida especial 12x16 der.) + alimento y serpiente | Header solo si cambia el puntaje o `_food.specialTime()` (banda 0..15); tablero (Body 16..63) solo si `_dirtyBoard` (movimiento, comida nueva, transición de estado): borra el Body y redibuja alimento + serpiente; overlay "3-2-1"/"PAUSA"/"GAME OVER"/festejo (texto invertido sobre banda blanca: cuadro centrado para el conteo, de lado a lado para PAUSA/GAME OVER/letreros "BUT"/"YOU ARE"/"THE BEST") cada frame según estado —en el conteo, al final de cada dígito el número y su cuadro se ocultan (`COUNT_HIDE_MS`), marcando `_dirtyBoard` una vez para restaurar el tablero—; al morir superando el récord, "GAME OVER" cicla "GAME OVER" → "BUT" → "YOU ARE" → "THE BEST" (`NEW_BEST_SIGN_MS` cada uno) hasta pulsar un botón, y `SFX_NEW_BEST` suena solo la primera vez que aparece "THE BEST" |

4. `MenuDifficulty`/`MenuSound` son **capas sobre el `Menu`**: no limpian la pantalla completa, solo la banda de los rombos (ver §7). Comparten estructura: `firstPrint()`/`clear()` borran la banda (45..53) en su primer frame y luego `nextOption()`/`blinkOption()` repintan el selector. El resto del `Menu` (título, cuadro, pie) queda intacto. El regreso es `changeState(State::MENU)` (sin `restoreDiamondBand()`, aún comentada) → hoy `begin()` completo del `Menu`. Los flags `_redrawSound`/`_redrawDifficulty` ya no existen.

---

## 14. Clase `Scroller` — API (scroller de 1 bit)

Encapsula la animación "scroller de 1 bit": compone un texto en un **array de `int8_t`** (cada byte = **1 columna de 8 px**, bit 0 = fila 0, bit 7 = fila 7) y lo desliza lateralmente. **Una banda** por instancia, siempre **fondo negro y texto blanco**.

- **Constantes:** `WIDTH = Config::Screen::WIDTH` (128, ancho completo), `STRIP_H = Config::Scroller::MAX_H` (32), `ANIM_TICK = 4` ms/px (vuelo ≈ 0,5 s).
- **Constructor:** `Scroller()` — sin parámetros; sin memoria dinámica (`_strip` por valor), copia permitida aunque bloqueada por coherencia.
- **API:**
  - `void begin()` — **comentado** (no existe en la versión actual).
  - `void setTexto(const char* text, int16_t y, bool toLeft = true, uint8_t size = 2)` — compone el texto centrado (cada byte = 1 columna), fija la **fila** (`_y = y`), la **dirección** (`toLeft`: `true` entra por la derecha) y el **alto** (`_height = 8*size`), calcula el límite (`floor(WIDTH / (6*size))`) y **trunca en silencio** si excede. Compone centrado en `GFXcanvas8(WIDTH, _height)` (cursor fila 0) y extrae columnas. Deja `_done` puesta (texto nuevo centrado, sin vuelo).
  - `void startSlide()` — **sin parámetros** (la dirección ya la fijó `setTexto()`). Arranca **fuera de pantalla**, baja `_done`, limpia `_isStopNow` y arranca el reloj. Borra en blanco la fila de 1 px por encima (`_y-1`) y por debajo (`_y+_height`) de la franja (si quedan en el Body).
  - `bool update()` — avanza 1 px cada `ANIM_TICK` ms (acumulador por tiempo). Devuelve `true` mientras la franja sigue entrando, `false` al terminar. Terminada, **no consume el reloj** (devuelve `false` sin avanzar) hasta `setTexto()`/`startSlide()`.
  - `bool print()` — vuelca la franja en la fila `_y`, **rellenando la banda con el fondo antes de pintar el texto**. Devuelve `false` **sin repintar** si `_done` está puesta (frame final ya en pantalla). Pone `_done` en el frame en que la tira llega a su sitio.
- **Bandera `_done` (reposo):** sustituye al antiguo `_dirty`. `print()` es no-op en cuanto la banda asienta. La bajan `setTexto()` y `startSlide()`.
- **Relleno previo:** mientras la franja entra solo cubre parte del ancho; las columnas no cubiertas conservarían lo de debajo. `print()` recorre la pantalla columna a columna calculando su equivalente en la franja (`screenX - _slideX`) y pinta los 8 px de cada byte → el texto nuevo **tapa** al anterior a medida que entra.
- **Iteración actual:** la **cortina** está **activa**: `curtain()` (triángulos que tapan la franja por delante) se ancla al **frente** (`front`, helper `timeCurtain()` = `6*_size + 4*_size - 1` px) y se dibuja cada frame. `startSlide()` arranca en `-timeCurtain()` (fuera de pantalla, lado según `toLeft`); mientras `_x < 0`, `print()` solo vuelca la cortina (el texto no entra hasta que la cortina asoma): la cortina **arrastra al texto** y el recorrido total pasa a ser `WIDTH + timeCurtain` px.
- **Usos:** `Menu` = 1 instancia (16 px, fila `BOX_TOP=26`): **transición activa**; `nextOption()` compone con `setTexto(OPTION[_selected], BOX_TOP, _lastSelected > _selected, TEXT_12x16)` y `startSlide()` al cambiar la selección; mientras `update()` devuelve `true` el resto de `Menu::update()` queda **congelado** y al terminar se reinicia `_timer` (parpadeo). `MenuCredits` = 2 sincronizadas (`_scrollerRol` 16 px + `_scrollerNombre` 8 px). `Legend` = 1 para el pie (8 px, `_printY = Config::Screen::FOOT_TOP`).
- **Detalle:** `setTexto()` usa `display.getTextWidth()` (centrado) y `display.getWidth()` (límite). `_strip[STRIP_H/8][WIDTH]` guarda columnas. `Boot` **no** usa `Scroller` (mensaje/logo estáticos con parpadeo propio, §8).

## 15. Namespace `Sprite` — API (sprites de la serpiente y comida especial)

`Sprite.h`. Tabla estática con los sprites (estilo Nokia) y el de la **comida especial**. **Namespace de solo datos** (sin instancia ni `.cpp`): `Sprite::pixel(part, x, y)` y `Sprite::specialPixel(x, y)`. El **logo** (`LOGO`, §8) se dibuja por bloques con `Display::drawBitmap()`.

**Empaquetado a 1 bit/píxel.** Cada sprite es un **literal binario** del tamaño exacto de sus px (4×4 = un `uint16_t`; comida 8×4 = un `uint8_t` por fila): en memoria solo queda la versión empaquetada, sin conversión ni tabla intermedia.

| Tabla | Antes | Ahora | Bytes |
|-------|-------|-------|-------|
| Serpiente (4×4) | `uint8_t SPRITES[COUNT][4][4]` | `uint16_t SPRITES[COUNT]` (16 px = 16 bits) | 432 → **54** |
| Comida especial (8×4) | `uint8_t SPECIAL_FOOD[4][8]` | `uint8_t SPECIAL_FOOD[4]` (1 byte/fila) | 32 → **4** |

Los literales llevan separadores `'` para leer las filas: en 4×4, cuatro grupos de 4 bits (una fila por grupo); en la comida, dos grupos de 4 por fila. El px `(0,0)` es el bit más significativo; el último, el bit 0. El acceso lo hacen las funciones `constexpr` `pixel()`/`specialPixel()` (el juego nunca indexa bits a mano).

```cpp
constexpr uint16_t SPRITES[COUNT] = {
  0b0110'0110'0100'0100,  // TAIL_TO_UP
  0b0000'0011'1111'0000,  // TAIL_TO_RIGHT
  ...
};
constexpr uint8_t SIZE = 4;                 // sprite 4×4 px
constexpr uint8_t BITS = SIZE * SIZE;       // px por sprite = bits de su uint16_t

enum Part : uint8_t {
  TAIL_TO_UP = 0, TAIL_TO_RIGHT, TAIL_TO_DOWN, TAIL_TO_LEFT,   // cola
  BODY_TO_UP,    BODY_TO_RIGHT,  BODY_TO_DOWN,   BODY_TO_LEFT, // cuerpo
  CORNER_RIGHT_UP, CORNER_RIGHT_DOWN, CORNER_LEFT_UP, CORNER_LEFT_DOWN, // curvas
  HEAD_UP_CLOSE, HEAD_RIGHT_CLOSE, HEAD_DOWN_CLOSE, HEAD_LEFT_CLOSE,     // fauces cerradas
  HEAD_UP_OPEN,  HEAD_RIGHT_OPEN,  HEAD_DOWN_OPEN,  HEAD_LEFT_OPEN,      // fauces abiertas
  BELLY_TO_RIGHT, BELLY_TO_LEFT,                  // panza recta (1 sprite por par)
  BELLY_RIGHT_UP, BELLY_RIGHT_DOWN, BELLY_LEFT_UP, BELLY_LEFT_DOWN,      // panza curva
  EMPTY,
  COUNT
};
```

### Miembros
| Miembro | Contenido |
|---------|-----------|
| `SIZE` | Lado del sprite (4). |
| `BITS` | Píxeles por sprite de la serpiente (16) = bits de su `uint16_t`. |
| `SPRITES[COUNT]` | Sprites **empaquetados** indexados por `Part`: cada uno un literal de 16 bits en cuatro grupos de 4. Rango: 0..3 cola, 4..7 cuerpo, 8..11 curvas, 12..15 cabeza cerrada, 16..19 cabeza abierta, 20..25 panza, 26 `EMPTY`, 27 `COUNT`. |
| `pixel(Part, x, y)` | `constexpr`: píxel (`x`=columna, `y`=fila) desde el `uint16_t` empaquetado. |
| `EMPTY` | Sprite vacío (todo fondo). |
| `COUNT` | Cantidad de sprites de la tabla. |
| `SPECIAL_FOOD_W`/`SPECIAL_FOOD_H` | Ancho (8) y alto (4) en px del sprite de la comida especial. |
| `SPECIAL_FOOD[SPECIAL_FOOD_H]` | Sprite 1 bit de la **comida especial** (8×4) **empaquetado por fila** (literal de 8 px por fila, px `(0,0)` = bit 7) = 4 bytes, tabla aparte por no encajar en los 4×4. |
| `specialPixel(x, y)` | `constexpr`: píxel de la comida especial. |
| `LOGO_W`/`LOGO_H` | Ancho (80) y alto (48) en px del logo del arranque. |
| `LOGO[LOGO_H * (LOGO_W / 8)]` | Logo 1 bit **empaquetado por fila** en formato de `drawBitmap()` (1bpp, MSB primero: px `(0,0)` = bit 7): 10 bytes/fila = **480 bytes** (vs. 3840 si 1 byte/px). **Polaridad inversa al resto** (**1 = fondo, 0 = glifo**), por eso `Boot` lo pinta con `Display::drawBitmap(..., true, false)` y **no tiene** función de lectura (`logoPixel` eliminado). |

La panza recta comparte sprite por par de direcciones: `BELLY_TO_RIGHT` = `BELLY_TO_UP` y `BELLY_TO_LEFT` = `BELLY_TO_DOWN`.

`Sprite.h` (antes `SnakeSprites.h`) forma parte del respaldo adaptado al estilo del proyecto (pie `// Fin`, cabecera descriptiva). Lo consumen `Snake` (§18) y `Game` (§16); `Food` usa `SPECIAL_FOOD` (§17).

---

## 16. Clase `Game` — API (ventana del juego)

`Game.h/.cpp`. Ventana del juego (estados `NEW`/`CONTINUE`), reemplaza al placeholder `InfoWindow` (eliminado). **Coordina**: la lógica vive en `Snake` (§18, que **no toca `Display`/`Sound`/`Food`**). `Game` decide el ritmo (dificultad), lee botones, traduce MOVE a `Snake::Dir`, llama `snake.step()` y maneja el resultado (`Result` MOVED/ATE/DIED) con puntaje, sonidos y regeneración del alimento, y dibuja el tablero volcando los segmentos que `Snake` expone (`length`/`segment`/`headPart`).

### Constructor
```cpp
Game();
```
Sin parámetros: consume los servicios globales. `_food`, `Snake _snake;` y el resto se construyen solos en su lista de inicialización.

### Constantes
| Constante | Valor | Significado |
|-----------|-------|-------------|
| `Config::Difficulty::MIN_LEVEL`/`MAX_LEVEL`/`DEFAULT_LEVEL` | 1/10/5 | Nivel acotado (mismo rango/fuente que el menú; ya no se duplica en `Game`). |
| `SPRITE_SCALE` | 2 | Px por lado de cada píxel del sprite al dibujar: los de `Sprite::SIZE` (4×4) se dibujan como bloques 2×2 para llenar la celda (`Sprite::SIZE * SPRITE_SCALE = Config::Screen::CELL` = 8). Antes era el `2` literal de `drawSprite`. |
| `COUNTDOWN_MS` | 3000 | Duración del conteo inicial (3 s, un dígito por segundo: 3-2-1). |
| `COUNT_HIDE_MS` | 250 | Fase de parpadeo al final de cada dígito: el número (y su cuadro) se ocultan antes del siguiente. |

La geometría del tablero (`COLS`=16, `ROWS`=6, `MAX_LENGTH`=96, rejilla 16×6 en el Body 128×48) vive en `Snake` (§18); `Game` la usa vía `Snake::COLS`/`Snake::ROWS` (p. ej. para construir `Food`). **El tamaño de celda no se declara en `Game`**: se toma de `Config::Screen::CELL` (8). Lo propio de `Game` es `SPRITE_SCALE`.

### Métodos
| Método | Descripción |
|--------|-------------|
| `void begin(bool newGame)` | `true` = nueva partida (reinicia todo y arranca el conteo 3-2-1). `false` = reanudar la partida en pausa; si no hay, arranca nueva. Conserva el récord (`_bestScore`). Llama `snake.clearPending()`. |
| `void setDifficulty(uint8_t level)` | Nivel 1..10 (clamp). **En caliente**: recalcula `_moveDelay` al instante, así una partida en curso (PLAY/PAUSE) sigue el nuevo ritmo; vale también para la próxima (`reset()` la re-deriva). |
| `void update()` | `START`: pre-gira con MOVE (giro pendiente), entra a `PLAY` con `ACTION_RIGHT` o al agotarse `COUNTDOWN_MS` (al arrancar suena `SFX_START`). `PLAY`: gira con MOVE (sin reversa directa, un único giro pendiente aplicado en el siguiente paso), avanza cada `_moveDelay` ms y `ACTION_RIGHT` (Btn2) pausa. `PAUSE`: reanuda con `ACTION_RIGHT` o `ACTION_LEFT`. `GAME_OVER`: cualquier ACTION vuelve al menú. `ACTION_UP` (Btn1) sale en cualquier estado menos `GAME_OVER`. |
| `void print()` | Renderizado por zonas (§13). |
| `bool done()` | `true` al pedir volver al menú. |
| `bool isGameOver()` | `true` si al salir la partida terminó en `GAME OVER`; lo usa el `Engine` (con `score() > 0`) para dejar la selección en `New` o `Continue`. |
| `uint16_t score()` / `bestScore()` | Puntaje actual / récord (el récord solo se actualiza al terminar en GAME OVER). El `Engine` sincroniza `bestScore()` con el menú al salir. |
| `bool occupied(uint8_t x, uint8_t y)` | ¿Celda ocupada por la serpiente? Lo consulta `Food`. **Delega en `Snake::occupied`.** |

### Reglas del juego (qué coordina Game)
- **Movimiento:** la cabeza avanza 1 celda por paso con **wrap en X e Y**. Velocidad (`_moveDelay` ms/paso) lineal con la dificultad con **pasos alternados de 101/102 ms** (102 en niveles 1, 4 y 7): `1000 - (nivel-1)*101 - (nivel+1)/3`. Los 9 saltos suman 912 ms, de **1000 ms (nivel 1) a 88 ms (nivel 10)** (secuencia 1000, 898, 797, 696, 594, 493, 392, 290, 189, 88; 101,33 ms/nivel no es entero → 6 saltos de 101 y 3 de 102). Fórmula en `Game::speedFor`.
- **Paso (`Game::step`):** llama `snake.step(_food.x(), _food.y())` y maneja `Snake::Result`:
  - `MOVED` → solo repintar (`_dirtyBoard`).
  - `ATE` → **puntaje suma el nivel actual** (`_score += _difficulty`, no +1 fijo; vale el nivel del momento de comer), `SFX_EAT`, regenera alimento (`_food.spawn(Food::Type::NORMAL, *this)`; si no hay celdas libres → tablero lleno → `die()`, la partida **se gana**/termina).
  - `DIED` → `die()`: `GAME_OVER` (`SFX_GAME_OVER`), `_hasGame = false`.
- **Comer/crecer, colisión, giro, sprites:** los resuelve `Snake` (§18); `Game` solo consume el resultado y repinta.
- **Alimento:** rombo simétrico centrado en la celda (dos `fillTriangle`), como el del menú; lo dibuja `Food` (§17). La **semilla aleatoria** se fija en `Game::reset()` con `randomSeed(esp_random() ^ (uint32_t)nowMs())` (`esp_random()` = RNG de hardware, entropía real) y se consume en `Food::spawn`.
- **Colisión con el cuerpo:** la celda destino es ilegal si coincide con el cuerpo **salvo la cola cuando NO come** (la cola se libera ese paso; bloqueante solo al comer); la resuelve `Snake::step`. Si colisiona: `GAME_OVER` (`SFX_GAME_OVER`), informa `SFX_BACK` al volver y `_hasGame = false`. **El récord (`_bestScore`) se verifica/actualiza solo aquí, en GAME OVER** (o al llenar el tablero, que también pasa por `die()`): partidas abandonadas con `ACTION_UP` no cuentan. Si supera el récord → **festejo de nuevo récord**: "GAME OVER" → "BUT" → "YOU ARE" → "THE BEST" en ciclo (`NEW_BEST_SIGN_MS` por letrero, `_newBest`/`_gameOverMs`/`_celeSfx`) hasta pulsar un botón; `SFX_NEW_BEST` suena solo la primera vez que aparece "THE BEST" (YOU ARE ya no la dispara).
- **Pausa y salida:** `ACTION_RIGHT` (Btn2) durante `PLAY` pausa ("PAUSA", `SFX_PAUSE`); en `PAUSE` retoma con el mismo botón o `ACTION_LEFT` (`SFX_RESUME`). `ACTION_UP` (Btn1) durante la partida vuelve al menú **sin perderla** (`_hasGame` mantiene el tablero; `Continue` la reanuda en pausa). `GAME_OVER` deja `_hasGame = false`. Al salir, el `Engine` deja la selección en **`Continue`** si siguió en curso **y con puntos**, o en **`New`** si terminó en GAME OVER o sin puntos (`resumable = !game.isGameOver() && game.score() > 0`, aplicado con `menu.setSelected(...)`); además **"Continue" se oculta** cuando no hay partida o no tiene puntos (`menu.setContinueAvailable(resumable)`): al arrancar, tras `GAME OVER` o sin puntos quedan 4 opciones (New, Dificultad, Sound, Créditos).
- **Header:** puntaje en `TEXT_12x16` (izq., no se mueve) y segundos restantes de la **comida especial** en `TEXT_12x16` (der., `_food.specialTime()`, por ahora fijo 60 solo layout). Se redibuja solo al cambiar. Ya no muestra el récord `HI` ("Best: N" queda solo en el menú).
- **Overlays:** conteo 3-2-1 (un dígito/segundo, `COUNTDOWN_MS/3` ms por dígito, centrado) y "PAUSA"/"GAME OVER" como **banda blanca de lado a lado**. `drawOverlay(title, fullWidth)` dibuja **cuadro centrado** (`fullWidth=false`, el conteo) o **banda borde a borde** (`fullWidth=true`, PAUSA/GAME OVER). La banda sobresale 2 px arriba y 0 abajo y está **centrada como rectángulo a lo alto del Body** (16..63): `y` se deriva del alto de la banda (`bandH = h + 2`) y el texto queda 2 px dentro; siempre `fillRoundRect` blanco + texto invertido `TEXT_12x16` centrado. **Festejo:** si al morir se supera el "Best", "GAME OVER" pasa al ciclo de cuatro letreros (`NEW_BEST_SIGN_MS` cada uno) hasta pulsar un botón; `SFX_NEW_BEST` solo la primera vez que aparece "THE BEST". **Parpadeo del conteo:** en los últimos `COUNT_HIDE_MS = 250` ms de cada dígito el número y su cuadro desaparecen; al ocultarlo se marca `_dirtyBoard` una vez (`_overlayHidden`, reiniciado en `reset()`). **Pitido por dígito:** `SFX_TICK` al aparecer cada dígito (`_lastCount`, reiniciado en `reset()`; `SFX_START` no suena en `reset()`, se reserva al arranque real). Al pasar `START → PLAY` suena `SFX_START` (GO!) justo tras el "1". Al volver a `PLAY` se marca `_dirtyBoard`.

### Validación en host (MinGW)
Antes de escribir el código se simuló en el PC la lógica núcleo (misma aritmética de ring buffer, colisiones y selección de sprites): comer/crecer (cola, score, longitud), longitud estable sin comida, colisión real (la cabeza no avanza), la cabeza **puede** ocupar la celda de la cola (anillo casi cerrado), wrap X/Y, y cargo de integridad de 400 pasos con giros. Ese núcleo vive hoy en `Snake` (§18), que por no depender de `Display`/`Buttons`/`Sound`/`Food` se compila y prueba en el PC sin stubs.

## 17. Clase `Food` — API (alimento del tablero)

`Food.h/.cpp`. Encapsula estado y lógica del alimento, extraídos de `Game` (antes `_food`/`_hasFood`/`_specialTime` y `spawnFood()`/`drawFood()`). Componente del juego (no ventana): sin `begin()`/`update()` propios; lo usa `Game` (miembro `_food`).

### Constructor
```cpp
Food(uint8_t cols, uint8_t rows, uint8_t top);
```
Recibe la geometría (dibuja con la `Display` global): rejilla de `cols`×`rows` celdas de `Config::Screen::CELL` px desde la fila `top` (el Body). `Game` la construye: `_food(Snake::COLS, Snake::ROWS, Config::Screen::BODY_TOP)`. **El tamaño de celda no se declara en `Food`** (el viejo `static constexpr uint8_t CELL = 8` duplicaba `Config::Screen::CELL`); `Food.cpp` incluye `Config.h`.

### Enum y constantes
```cpp
enum class Type : uint8_t { NORMAL = 0, SPECIAL };
static constexpr uint8_t SPECIAL_TIME_DEFAULT = 60;   // segundos iniciales de la especial (layout)
```

| Tipo | Significado |
|------|-------------|
| `NORMAL` | Comida común: rombo simétrico centrado en la celda (como el rombo seleccionado del menú). |
| `SPECIAL` | Comida especial: sprite `Sprite::SPECIAL_FOOD` (8×4, centrado verticalmente en la celda) y temporizador (`specialTime`, los segundos que el Header muestra a la derecha). Por ahora el temporizador es solo layout (valor fijo). |

### Métodos
| Método | Descripción |
|--------|-------------|
| `void begin()` | No hay alimento. |
| `bool spawn(Type, const Game&)` | Coloca un alimento del tipo dado en una **celda libre al azar**. La ocupación la responde `Game::occupied` (público para esto): se cuentan las libres y se elige la `pick`-ésima (dos recorridos, sin buffer). Devuelve `false` si no hay celdas libres (tablero lleno → partida ganada), quedando sin alimento. |
| `void clear()` | Quita el alimento. |
| `bool has()` | `true` si hay alimento. |
| `Type type()` | Tipo del alimento actual. |
| `uint8_t x()` / `y()` | Columna (0..`COLS`-1) / fila (0..`ROWS`-1). |
| `uint8_t specialTime()` | Segundos restantes de la comida especial. |
| `void setSpecialTime(uint8_t s)` | Actualiza el temporizador de la especial. |
| `void draw() const` | Dibuja según tipo: `NORMAL` → rombo (`drawNormal`, dos `fillTriangle`); `SPECIAL` → sprite (`drawSpecial`, píxel a píxel del sprite de 1 bit). Si no hay alimento, no dibuja. |

`Game` usa `_food`: lo genera en `reset()` y al comer (`_food.spawn(Food::Type::NORMAL, *this)`; si devuelve `false` se muere), consulta `_food.has()/x()/y()` para comida y boca abierta, dibuja `_food.draw()` al volcar el tablero y muestra `_food.specialTime()` en el Header. La construye en la lista de inicialización como arriba.

---

## 18. Clase `Snake` — API (lógica pura de la serpiente)

`Snake.h/.cpp`. Encapsula toda la lógica de la serpiente, extraída de `Game`. Componente del juego (no ventana): **no depende de `Display`, `Sound` ni `Food`** —solo de `Arduino.h` (`delay`, `random`) y `Sprite.h` (enums de partes para elegir sprites que `Game` dibujará)—, por lo que se compila y prueba en el PC sin el resto.

### Qué hace y qué no
`Snake` mantiene el **buffer circular** de segmentos, la dirección commitida, el giro pendiente (sin buffer ni reversa directa), el avance con wrap, la detección de colisión, comer/crecer y la elección del sprite de cada parte. `Game` **coordina**: le pasa el alimento en cada paso, aplica puntaje/sonidos con el resultado, regenera la comida y dibuja. `Game` mantiene la **dificultad (ritmo)** y la **velocidad** (aquí no intervienen): `Snake::step` se llama una vez por paso y `Game` controla la cadencia.

### Constructor
```cpp
Snake();
```
Deja la serpiente vacía (longitud 0, dirección `NONE`); hay que llamar `reset()` antes de usarla.

### Constantes
| Constante | Valor | Significado |
|-----------|-------|-------------|
| `COLS`, `ROWS` | 16, 6 | Tablero: rejilla 16×6 de celdas de 8 px en el Body (128×48). Antes en `Game`; `Game`/`Food` las usan vía `Snake::COLS`/`Snake::ROWS`. |
| `MAX_LENGTH` | 96 | Segmentos máximos (una celda por segmento; 16×6 celdas caben y nunca se repite celda viva). |
| `Dir::NONE`/`UP`/`RIGHT`/`DOWN`/`LEFT` | 0/1/2/3/4 | Direcciones. El orden (UP=1..LEFT=4) coincide con el de los sprites por dirección (`dir-1`). `NONE` = sin dirección (estado vacío). |
| `Result::MOVED`/`ATE`/`DIED` | — | Resultado de `step`: avanzó sin comer / comió (crece, la cola NO avanza ese paso) / colisión (muerte, sin moverse). |

### Métodos
| Método | Descripción |
|--------|-------------|
| `void reset()` | Inicializa: celdas `(1,2)..(4,2)`, cabeza a la derecha, longitud 4, giro pendiente `NONE`. |
| `void clearPending()` | Descarta el giro pendiente (`_nextDir = NONE`). Lo llama `Game::begin()` al entrar (START) para no reanudar con un giro acumulado del menú. |
| `bool turn(Dir d)` | **Único giro pendiente, sin buffer ni reversa directa**. Se evalúa **siempre** desde la dirección actual de la cabeza (`_dir`, la COMMITIDA): seguir, girar a izquierda o derecha; la contraria (180°) se **ignora**. Devuelve `true` si el giro quedó pendiente (el último válido pisa al anterior) para que `Game` toque `SFX_TURN` solo al aceptar. Así no se genera un GAME OVER espurio por una reversa falsa del último MOVE. |
| `Result step(uint8_t fx, uint8_t fy)` | **Avanza un paso** (aplica el giro pendiente si lo hay). `(fx, fy)` = alimento (lo pasa `Game` desde `_food`). Devuelve `Result`; `Game` interpreta: `ATE` → crece (la cola NO avanza ese paso), `DIED` → no se mueve. |
| `bool occupied(uint8_t x, uint8_t y)` | `true` si la celda `(x,y)` está ocupada. Lo consulta `Food` (al colocar alimento) vía `Game::occupied`, que delega aquí. |
| `uint8_t length()` | Longitud actual. |
| `const Seg& segment(uint8_t i)` | Segmento `i` (0 = cola, `length()-1` = cabeza) para que `Game` lo dibuje. |
| `Sprite::Part headPart(bool hasFood, uint8_t fx, uint8_t fy)` | Sprite de la cabeza según la dirección commitida: `HEAD_<dir>_OPEN` **una casilla antes** de llegar al alimento (la comida está en la próxima celda según `_dir`) y `HEAD_<dir>_CLOSE` al colisionar con él (o sin comida). Lo consulta `Game` cada frame. |

### Internos (segmentos y buffer)
```cpp
enum class Dir : uint8_t { NONE = 0, UP = 1, RIGHT, DOWN, LEFT };
enum class Result : uint8_t { MOVED, ATE, DIED };
struct Seg {
    uint8_t x, y;
    Dir dir;            // hacia el segmento siguiente (más cerca de la cabeza)
    Sprite::Part part;  // sprite persistente
};
Seg _body[MAX_LENGTH];
uint8_t _headIx, _tailIx;   // índices circular (cabeza/cola)
uint8_t _length;
Dir _dir, _nextDir;         // commitida + giro pendiente
Sprite::Part bodyPartFor(Dir in, Dir out);
Sprite::Part bellyPartFor(Dir in, Dir out);
```

- **Ring buffer:** cola en `_tailIx`, cabeza en `_headIx`. Cada paso **agrega una parte nueva** (la cabeza) y **elimina la última** (la cola) salvo al comer (la cola NO avanza). El nuevo segmento se escribe en `slot()` = `_headIx+1` módulo `MAX_LENGTH`; la cabeza nueva apunta a ese slot.
- **El cuerpo NO se mueve:** la casilla que la cabeza deja se convierte en cuerpo con su `part` persistente (`bodyPartFor`/`bellyPartFor`), según por qué lado entra y sale la tubería: recta `BODY_TO_<dir>` (`in == out`) o esquina `CORNER_<horizontal>_<vertical>`/`BELLY_RIGHT_UP`.. al girar (`in != out`). La panza recta comparte sprite por par: `BELLY_TO_RIGHT` = `BELLY_TO_UP` y `BELLY_TO_LEFT` = `BELLY_TO_DOWN`.
- **Comer (`bodyPartFor` modo `EAT`):** con `in == out` se toma la panza recta de la dirección `in`; con giro (comer y girar a la vez), la panza curva `BELLY_<horizontal>_<vertical>`.
- **Colisión (`step`):** la celda destino es ilegal si coincide con el cuerpo **salvo la cola cuando NO come**. El giro pendiente se evalúa con una **copia local** (`dir`) y `_dir` (la COMMITIDA) solo se actualiza si el destino es legal, así al morir la cabeza conserva la orientación real de su último movimiento (su sprite se dibuja según `_dir`).
- **Wrap:** `x = (x + 1) % COLS` etc.: sale por un borde, aparece por el opuesto.

### Validación en host
Por no depender de `Display`/`Buttons`/`Sound`/`Food`, `Snake` se compila y prueba en el PC (MinGW) sin stubs: los casos del núcleo original (comer/crecer, longitud estable sin comida, colisión real sin avanzar, cabeza sobre la celda de la cola, wrap X/Y, cargo de integridad con giros) se conservan y pasan mirando a esta clase directamente.

## 19. `Config.h` — constantes compartidas (namespace `Config`)

`Config.h`. **Un solo archivo pequeño para lo verdaderamente compartido**: constantes que usan varias clases o propias del hardware/placa. No es un cajón de sastre: lo específico de una clase queda como `static constexpr` dentro de ella (p. ej. `ANIM_TICK` en `Scroller`, `NEW_BEST_SIGN_MS` en `Game`, `ARROW_BLINK_PERIOD` en `Menu`).

- **Sin `#define` para valores:** las constantes llevan tipo y viven en namespaces (no contaminan el ámbito global ni chocan con librerías Adafruit/core). `#define` solo para lo que necesita el preprocesador (`#ifdef DEBUG`). Header: `#pragma once` + `#include <Arduino.h>`.
- **`constexpr` no ocupa RAM** y compila en Arduino IDE con el core ESP32 (C++17).

```cpp
namespace Config {

namespace Pin {
  constexpr int8_t BUTTONS[8] = { 2, 1, 42, 41, 38, 40, 39, 47 }; // orden Buttons::Button
  constexpr uint8_t BUZZER   = 14;  // zumbador
  constexpr uint8_t OLED_SDA = 8;   // I2C: datos
  constexpr uint8_t OLED_SCL = 9;   // I2C: reloj
}

namespace Screen {
  constexpr uint8_t WIDTH = 128, HEIGHT = 64, CELL = 8, ADDRESS = 0x3C;
  constexpr uint8_t HEADER_TOP = 0, HEADER_H = 16;  // Header 0..15
  constexpr uint8_t BODY_TOP = 16, BODY_H = 48;     // Body 16..63
  constexpr uint8_t FOOT_TOP = HEIGHT - 8, FOOT_H = 8;  // texto del pie (56..63)
  // constexpr uint8_t FOOT_LINE = HEIGHT - (8 + 2 + 1);  // línea separadora (comentada: no la dibuja nadie)
}

namespace Difficulty {
  constexpr uint8_t MIN_LEVEL     = 1;
  constexpr uint8_t MAX_LEVEL     = 10;
  constexpr uint8_t DEFAULT_LEVEL = 5;
}

namespace Version {
  constexpr char* VERSION      = "v1.0.0";
  constexpr char* RELEASE_DATE = "2026/12/31";
}

namespace Button {          // repetición al mantener un botón (nuevo)
  constexpr uint32_t DELAY = 400;  // mantener para empezar a repetir (ms)
  constexpr uint32_t TICK  = 100;  // intervalo de repetición (ms)
}
}
```

Los namespaces internos (`Pin`, `Screen`, `Difficulty`, `Version`, `Credits`, `Legend`, `Diamond`, `Button`, `Scroller`, `MenuStrip`, `DefaultTimer`) van en columna 0, sin indentar respecto de `namespace Config {`.

### Qué contiene y quién lo usa
| Namespace | Constantes | Las usan |
|-----------|------------|----------|
| `Config::Pin` | `BUTTONS`, `BUZZER`, `OLED_SDA`, `OLED_SCL` | el `.ino` (pasa `BUTTONS` a `Buttons` y `BUZZER` a `Sound`, que construye su `Buzzer`), `Buzzer` (pin por defecto), `Display` (pines I2C por defecto) |
| `Config::Screen` | `WIDTH`/`HEIGHT`/`CELL`/`ADDRESS` y regiones `HEADER_*`/`BODY_*`/`FOOT_*` | `Display` (defaults y `regionBounds`), `Boot` (bandas TITULO=Header/CUERPO=Body), `Game` (tablero en el Body, celda de sprites y `BODY_TOP`), `Food` (celda: centro del rombo y sprite especial), `MenuCredits` (rol del Body), `Legend` (línea/fila del pie), `Scroller` (`WIDTH`) |
| `Config::Difficulty` | `MIN_LEVEL`/`MAX_LEVEL`/`DEFAULT_LEVEL` | `MenuDifficulty` y `Game` (`setDifficulty`/velocidad): antes duplicadas. El sufijo `_LEVEL`/`DEFAULT_LEVEL` evita la macro `DEFAULT` del core ESP32. |
| `Config::Version` | `VERSION`/`RELEASE_DATE` | `Menu` (`version`, texto del pie; antes `"v0.1"` en la firma). `RELEASE_DATE` aún sin uso (queda para créditos o pie). `NAME` ("Snake II") lo dibuja `Boot` como título. |
| `Config::Button` | `DELAY` (400 ms)/`TICK` (100 ms) | `ButtonRepeat` (§23): mantener `DELAY` ms para repetir y luego un paso cada `TICK` ms; lo usan `Menu` y `MenuDifficulty` a través suyo. **Nuevo**. |
| `Config::Legend` | `NEXT` (2500 ms)/`PERIOD` (100 ms)/`OFF` (50 %); `HOLD` **comentado** | `Legend` (ciclo de rombos con `_ticker`, parpadeo con `_blink`/`Blink`): `HOLD` pasó de 900 ms a comentado; `NEXT` subió de 2200 a 2500 ms. |
| `Config::Diamond` | `SIZE` (3 px) | `Menu` (rombo marcador y triángulos: `Menu::SIZE`) y `Legend` (rombos de ACTION: `Legend::SIZE`). |
| `Config::Scroller` | `MAX_H` (32)/`ANIMATION` (4 ms/px); `TOP` **comentado** | `Scroller` (`STRIP_H = MAX_H`; `ANIM_TICK = ANIMATION`). `TOP` quedó **comentado**. |
| `Config::MenuStrip` | `BODY_H` (39)/`BODY_MIDDLE` (34)/`BOX_HEIGHT` (16)/`BOX_TOP` (= `BODY_MIDDLE - BOX_HEIGHT/2` = 26)/`VALUE_TOP` (44)/`VALUE_HEIGHT` (11)/`DIAMOND_Y` (= `VALUE_TOP + VALUE_HEIGHT/2` = 49)/`TRIANGLE_Y` (= `Screen::FOOT_TOP - 3` = 53) | `Menu` (cuadro y marcadores, §7) y `MenuSound` (banda del selector ON/OFF). Geometría **centrada en la mitad del Body** (`B`/`C` auxiliares). |
| `Config::DefaultTimer` | `PERIOD` (500 ms)/`OFF` (20 %); `HOLD` **comentado** | Parpadeo "de cualquier ventana" sin cronómetro propio: hoy `Boot` (mensaje) y `Menu` (rombo de la opción). `HOLD` comentado (sin fase fija inicial). |
| `Config::Power` | `IDLE_TIMEOUT_MS` (60000 ms) | Reposo (§26): tiempo sin actividad fuera de partida antes del light sleep. Lo usa el `.ino` (cronómetro `idleTimer` en `loop()`). |

Las regiones `HEADER_TOP/H` y `BODY_TOP/H` reemplazan las constantes repetidas `BODY_TOP`/`BODY_H`/`TITLE_TOP`/`TITLE_H` de `Menu`, `Game`, `Boot` y `MenuCredits`; los `static constexpr` de esas clases se eliminaron. `Boot` tenía además constantes de la animación de bandas (`BAR_W`, `BAR_SPACING`, `ANIM_TICK`, `TOTAL_MS`), hoy **comentadas**.

La banda del pie tiene su geometría en `Config::Screen`: `FOOT_LINE` (`HEIGHT - (FOOT_H + 1 + 1)` = 54) está **comentado**; `FOOT_TOP` = 56 con `FOOT_H` = 8, derivados de `HEIGHT` como `HEADER_*`/`BODY_*` (antes `FOOT_TOP` = `HEIGHT - 9`, `FOOT_H` = 7). Hoy los usan **`Legend`** y **`Menu`** (pie: banda blanca `55..63` con "Best" y la versión).

`CELL` es la **única** medida de px por celda (reemplaza al `static constexpr uint8_t CELL = 8` de `Food` y a los `8` literales de `Game::drawSprite`). Lo **no** compartido (factor de escala del sprite 4×4 a la celda) queda como constante propia con nombre: `Game::SPRITE_SCALE = 2` (`Sprite::SIZE * SPRITE_SCALE == Config::Screen::CELL`).

---

## 20. `Globals.h` — globales (servicios y estado)

`Globals.h`. Declara con `extern` los **globales** compartidos: servicios de hardware (`Display`, `Buttons`, `Sound`) y el almacén de estado (`Storage`, §22). Las clases ya no reciben los servicios por constructor (§3, §6, §11): el único lugar donde se **instancian** es el `.ino`.

```cpp
// Globals.h
#pragma once

#include "Display.h"
#include "Buttons.h"
#include "Sound.h"
#include "Storage.h"

extern Display display;
extern Buttons buttons;
extern Sound   sound;
extern Storage storage;
```

### Reglas
1. **Se instancian solo en el `.ino`**, y cada servicio se inicializa en su `begin()` desde `setup()` (`Storage` no tiene `begin()`: bastan sus valores por defecto). Por eso el **orden de definición ya no importa**: el `Engine`, declarado al final, se construye sobre globales que solo necesitan su constructor —`Scroller` (`display.getWidth()`) y `Food` son seguros— y el trabajo real va en los `begin()`. **No** se usa `static order/fiasco` ni factories.
2. **Son globales los servicios de hardware y `Storage`.** Las **ventanas** (`Boot`, `Legend`, `Menu`, `MenuCredits`, `Game`) NO: son miembros del `Engine` (§11). El **`Buzzer` tampoco** (hardware interno de `Sound`, por valor). `Storage` sí es global (almacén de estado compartido: ni ventana ni hardware).
3. **`Globals.h` se incluye solo desde los `.cpp`** (las cabeceras no lo incluyen): evita acoplar los `.h` al global y mantiene los includes predecibles. Un `.cpp` que usa un servicio global o `storage` debe incluir `Globals.h`. `Sound.h` sí incluye `Buzzer.h` (su miembro es `Buzzer` por valor).

---

## 21. `Timer.h` — reloj de 64 bits y cronómetros

`Timer.h/.cpp`. Antes header-only; ahora las definiciones están en el `.cpp` (patrón del proyecto). Motivo: `Stopwatch`/`Ticker` los usan casi todas las ventanas y, como métodos `inline` en la clase, el compilador repetía el mismo código en cada unidad de traducción. **Nada se declara `inline` en el header**: una función `inline` definida en otro `.cpp` no genera símbolo y daría *undefined reference*.

Ante los límites de `millis()` (32 bits: da la vuelta cada ~49,7 días; `elapsed % period` sobre un instante absoluto salta de fase una vez por giro), el tiempo se centraliza en el **reloj de 64 bits**: `esp_timer_get_time()` (µs desde el arranque, no envuelve en ~292.000 años). Con 64 bits las restas y los módulos son seguros.

**`esp_timer.h` vive solo en el `.cpp`** (único consumidor: `nowMs()`). `<Arduino.h>` **se mantiene en el header** a propósito: otras unidades heredan los tipos enteros y no lo incluyen (`Buttons.cpp` usa `pinMode`/`digitalRead`, `Game.cpp` usa `Serial`). Quitarlo exigiría añadirlo explícitamente en esos `.cpp`: deuda conocida.

### API
```cpp
uint64_t nowMs();        // instante actual en milisegundos (64 bits)

class Stopwatch {
  void start();                        // reinicia (llamar en el begin() de la ventana)
  uint64_t elapsed() const;            // ms desde start()
  bool expired(uint32_t ms) const;     // ¿ya pasaron `ms`?
  bool blinkOn(period, offPct) const;  // ¿fase visible? (oculto el primer offPct%)
};

class Ticker {
  explicit Ticker(uint32_t period);    // pasos de `period` ms
  void start();                        // reinicia el acumulador
  uint32_t consume();                  // pasos completos desde la última consulta (conserva el residuo)
};
```

### Reglas de uso
1. **Medir con restas, nunca contra un instante absoluto:** `nowMs() - inicio` (enteros sin signo de 64 bits).
2. **`Stopwatch` para plazos:** `COUNTDOWN_MS`/`_moveDelay`/`NEW_BEST_SIGN_MS` en `Game`, `_durationMs` en `Buzzer`, y el interior de `ButtonRepeat`/`Blink` (§23-§24). El parpadeo de `Boot`/`Menu`/`Legend` ya no usa `expired(HOLD)` (fase fija comentada). `start()` se llama en el `begin()` de la ventana o al fijar la zona que parpadea.
3. **`Ticker` para pasos periódicos:** `ANIM_TICK` en `Scroller` y `NEXT` en `Legend` (`consume()` devuelve los pasos de una vez). En `Boot`/`Menu` está **comentado** (sus animaciones cambiaron).
4. **`blinkOn(period, offPct)`** reemplaza al `millis() % período` absoluto. Ya no lo llaman las ventanas directamente: lo encapsula `Blink` (§24).

### Quién lo usa
| Clase | Reloj/cronómetro | Cambio |
|-------|------------------|--------|
| `Boot` | `Blink _blink` | Mensaje con `Blink` (`changed`/`isVisible`), anclado al `begin()` (`PERIOD`/`OFF` de `Config::DefaultTimer`; `HOLD` comentado). El antiguo `Stopwatch _timer` quedó dentro del helper; `Ticker _ticker` + `TOTAL_MS` comentados. |
| `Menu` | `Blink _blink` + `ButtonRepeat _repeat` | Parpadeo con `Blink`, anclado a `begin()` y al final de cada vuelo del scroller; navegación con repetición vía `ButtonRepeat` (`expired(DELAY)`/`expired(TICK)`). `Ticker` viejo comentado. |
| `MenuDifficulty` | `Blink _blink`, `ButtonRepeat _repeat` | Extraído de `Menu`: `Blink` (`changed`/`isVisible`) y `ButtonRepeat` (se eliminaron `HOLD_REPEAT_DELAY`/`HOLD_REPEAT_TICK` y `holdRepeat`). |
| `MenuSound` | `Blink _blink` | Extraído de `Menu`: `Blink`. |
| `Legend` | `Ticker _ticker` (`NEXT`) + `Blink _blink` | Ciclo de rombos con `ticker.consume()` cada `NEXT = 2500 ms`; parpadeo con `Blink`, anclado al `begin()` y reanclado (`start()`) al terminar el vuelo del scroller. `HOLD`/`DWELL_MS`/`HOLD_MS` ya no existen. |
| `Scroller` | `Ticker _timer` (`ANIM_TICK`) | El acumulador `_colAcc`/`_animLast` pasa a `consume()` (misma cadencia, sin `while`). |
| `Game` | `nowMs()` en `_moveLast`/`_startMs`/`_gameOverMs` (`uint64_t`) | Plazos/módulos del conteo y festejo sobre 64 bits (sin techo de 49,7 días). |
| `Buzzer` | `_startMs` (`uint64_t`) + `nowMs()` | El fin de la duración se compara con resta de 64 bits. |
| `Buttons` | `_buttonLast` (`uint64_t`) + `nowMs()` | El debounce se mide sobre 64 bits. |

`Sound` no tiene reloj propio (delega en `Buzzer`). `Snake` sigue siendo lógica pura (solo `Arduino.h`) y no usa el reloj.

## 22. Clase `Storage` — API (estado compartido)

`Storage.h/.cpp`. Almacena la información que **no debe vivir en ninguna ventana** y que varias partes leen/escriben: hoy el **mejor puntaje**, el **sonido activo** y la **dificultad**. Cualquier otro valor compartido que surja (p. ej. si hay partida en curso) se añade aquí.

**Contenedor de datos puro**: no depende de `Display`/`Buttons`/`Sound`, sin `begin()`/`update()`/`print()` ni relojes; solo getters/setters. Valores por defecto del constructor (de `Config` donde aplica).

### Instancia global
Lo global es la **instancia** —no métodos ni propiedades estáticas—: `extern Storage storage;` en `Globals.h` (§20) y `Storage storage;` en el `.ino`. Cualquier clase la usa desde su `.cpp` (`storage.bestScore()`) incluyendo `Globals.h`.

### Constructor
```cpp
Storage();
```
Sin parámetros. Defaults: `_bestScore = 0`, `_soundEnabled = true`, `_difficulty = Config::Difficulty::DEFAULT_LEVEL`.

### Métodos
| Método | Descripción |
|--------|-------------|
| `uint16_t bestScore() const` | Mejor puntaje (récord). |
| `void setBestScore(uint16_t)` | Fija el récord tal cual (no compara: decidir qué es récord es del llamador). |
| `bool soundEnabled() const` | ¿Sonido activo? |
| `void setSoundEnabled(bool)` | Activa/desactiva el sonido. |
| `uint8_t difficulty() const` | Nivel de dificultad (1..10). |
| `void setDifficulty(uint8_t)` | Fija el nivel con **clamp** a `Config::Difficulty::MIN_LEVEL`/`MAX_LEVEL`. |

### Estado actual (pendiente de integrar)
La clase y su instancia global **existen pero no las consume nadie**: `Menu` guarda su `_bestScore` (§7), `MenuDifficulty` su `_difficulty` (§10.3) y `Sound` su `_enabled` (§10.2). Mover esas copias a `storage` queda pendiente (decisión del usuario: en esa tarea solo se creó la clase).

---

## 23. `ButtonRepeat.h` — repetición al mantener un botón

`ButtonRepeat.h/.cpp`. Helper **por composición** (no hereda ni es heredado): encapsula "paso inmediato al pulsar y, si se mantiene, un paso cada `tick` ms tras `delay` ms". Sustituye los dos `Stopwatch` (`_repeat`/`_repeatTick`) que `Menu` y `MenuDifficulty` llevaban sueltos, que además habían **divergido** (`Menu` repetía con `buttons.hold()`; `MenuDifficulty` con el viejo `buttons.state()`). Aquí se unifica en `hold()`.

### API
```cpp
class ButtonRepeat {
public:
  explicit ButtonRepeat(uint32_t delay = Config::Button::DELAY,
                        uint32_t tick  = Config::Button::TICK);

  bool step(uint8_t button);   // ¿toca aplicar un paso en este frame?
};
```
- `step(button)`: `true` si el botón pasó a presión (`pressed`, reinicia ambos cronómetros) o si sigue mantenido (`hold`) y ya vencieron `delay` y `tick` (reinicia solo el de cadencia). `false` en cualquier otro caso.

### Decisiones
- **Sin `reset()`**: ningún call site lo necesita hoy (YAGNI); si aparece, se agrega.
- **Tiempos por defecto de `Config::Button`** (`DELAY = 400`, `TICK = 100`): las ventanas no repiten constantes propias.
- Usa `buttons.hold()` (no `state()`), el método vigente.

### Quién lo usa
- `Menu` (`_repeat`): en `navigate()` para `MOVE_LEFT`/`MOVE_RIGHT` (§7).
- `MenuDifficulty` (`_repeat`): en `update()` para `MOVE_RIGHT`/`MOVE_LEFT` (§10.3). Se eliminaron `HOLD_REPEAT_DELAY`/`HOLD_REPEAT_TICK`.

---

## 24. `Blink.h` — parpadeo con ancla y detección de flanco

`Blink.h/.cpp`. Helper **por composición** que encapsula un `Stopwatch` (ancla del período) + el recuerdo de la última fase (`_last`, para detectar el flanco). Así el parpadeo deja de repetirse (y divergir) entre `Boot`, `Legend`, `Menu`, `MenuDifficulty` y `MenuSound`, y las ventanas no llevan su propio flag de giro.

### API
```cpp
class Blink {
public:
  void start();                                       // ancla el período y olvida la fase (_last = false)
  bool isVisible(uint32_t period, uint8_t offPct) const;  // fase actual (true = visible)
  bool changed(uint32_t period, uint8_t offPct);      // true solo en el flanco
};
```
- `start()`: `_timer.start()` + `_last = false` (única forma de anclar; en `begin()` y al reanclar tras el vuelo del `Scroller`).
- `isVisible(period, offPct)`: fase cruda del período (`Stopwatch::blinkOn`); visible salvo el primer `offPct%`. No modifica nada (const): para quien repinta cada frame (`MenuDifficulty`, `MenuSound`).
- `changed(period, offPct)`: `now = isVisible(...)`; si `now == _last`, `false`; si no, `_last = now` y `true`. Para quien redibuja solo en el flanco (`Boot`, `Legend`, `Menu`).

### Cómo lo interpreta cada ventana
- **`Boot`** (`_blink`): `changed(PERIOD, OFF)` decide si repintar; `isVisible(PERIOD, OFF)` es "mensaje visible". `blinkMessage()` → `if (!_blink.changed(PERIOD, OFF)) return; drawMessage(_blink.isVisible(PERIOD, OFF))`; `firstPrint()` dibuja con `drawMessage(true)`.
- **`Legend`** (`_blink`): `blinkDiamond()` sale si el `Scroller` vuela (`!_scroller.done()`), si cambió el rombo (`_lastBtn != _btn`) o si `!changed(PERIOD, OFF)`; repinta con `toggleDiamond(_btn, _blink.isVisible(PERIOD, OFF))`. Al terminar el vuelo del pie, `start()`.
- **`Menu`** (`_blink`): `blinkOption()` con `_confirm` sale del menú; si no, `if (!changed(PERIOD, OFF)) return; const bool visible = isVisible(PERIOD, OFF); toggleText(visible); toggleDiamond(_selected, visible);`. Se ancla en `begin()` y se reancla al terminar el vuelo.
- **`MenuDifficulty`**/`MenuSound` (`_blink`): `blinkOption()` consulta `changed(PERIOD, OFF)` y repinta su selector con `isVisible(PERIOD, OFF)` (en `MenuDifficulty`, las flechas —fijas al mantener el botón—; el número no parpadea).

### Decisiones
- `isVisible()` devuelve la fase **visible** (`true` = visible); antes el flag se interpretaba invertido en `Boot`, que ahora pasa la fase directamente a `drawMessage(bool)`.
- `changed()` lleva `_last` internamente: el redibujo en el flanco no necesita que la ventana guarde ningún flag de giro.
- `start()` (única forma de anclar) reancla tras el vuelo del `Scroller` y olvida la fase.

## 25. `Draw.h` — primitivas de dibujo (namespace)

`Draw.h/.cpp`. Namespace con las dos formas que `Menu` y `Legend` tenían **duplicadas** (marcadores de posición): un triángulo direccional y el rombo completo (dos triángulos superpuestos). Sin estado (no es clase): usa el global `display` y `Config::Diamond::SIZE`.

### API
```cpp
namespace Draw {
  enum Direction : uint8_t {
    DIR_UP = 1, DIR_RIGHT, DIR_DOWN, DIR_LEFT
  };
  void triangle(int8_t dir, int16_t centerX, int16_t centerY, bool show = true);
  void diamond(int16_t centerX, int16_t centerY, bool show = true);
}
```
- `triangle(dir, cx, cy, show)`: `display.fillTriangle` con el vértice según `dir` (`Draw::Direction`) y medio lado `Config::Diamond::SIZE` alrededor de `(cx, cy)`; `show` elige blanco (`true`) o negro (`false`).
- `diamond(cx, cy, show)`: un triángulo hacia arriba + uno hacia abajo (mismo centro): `triangle(DIR_UP, …)` + `triangle(DIR_DOWN, …)`.

### Quién lo usa
- **`Menu`**: `toggleDiamond()` → `Draw::diamond(centerX, DIAMOND_Y, show)`; `toggleTriangle()` → `Draw::triangle(Draw::DIR_UP, centerX, TRIANGLE_Y, show)`.
- **`Legend`**: flechas del pad MOVE con `Draw::triangle(Draw::DIR_UP/DIR_RIGHT/DIR_DOWN/DIR_LEFT, …)` y rombo activo con `Draw::diamond(…)` desde `Legend::toggleDiamond`. Se eliminaron `Legend::toggleTriangle` y la constante `SIZE`.

### Decisiones
- **Sin estado → función libre** (no método de instancia): por eso namespace. El único estado que toca (`display`, `SIZE`) es global/compartido.
- Nombre a nivel de **intención** (`triangle`/`diamond`), no de primitiva GFX (`fillTriangle`), para que las ventanas no repitan coordenadas relativas.

## 26. Reposo (light sleep)

`SleepManager.h/.cpp` (clase `SleepManager`), con `Config::Power::IDLE_TIMEOUT_MS` (60 s) y los métodos `Display::power(on)`, `Buttons::anyPhysical()`/`ignoreUntilRelease()` y `Engine::isInGame()`/`repaint()`. Antes era **función libre** del wiring (`enterSleep()`, `idleTimer`, `diagSeen`); se extrajo a su propia clase por ser un subsistema cerrado (política de inactividad + wake + plataforma + diagnóstico). No es ventana: usa los servicios globales (`display`, `buttons`, `sound`) y recibe `Engine` por referencia. El wiring solo llama `sleepManager.begin()` (setup) y `sleepManager.update()` (loop).

### Qué hace
- **Disparador:** en `SleepManager::update()` (cada frame desde `loop()`, tras `buttons.read()`), `_idle` (un `Stopwatch`) cuenta el tiempo sin botones. Cualquier `buttons.anyPhysical()` lo reinicia (`start()`). Se usa el estado **físico** (no `anyHeld()`, que refleja la supresión `ignoreUntilRelease()`): un pulso mantenido tras el wake sigue contando como actividad. Si **no se está en partida** (`!_engine.isInGame()`), **no hay ningún botón presionado** (`!buttons.anyPhysical()`, para no entrar con un pin en HIGH) y llegó a `IDLE_TIMEOUT_MS`, se entra en reposo.
- **Reposo:** `enterSleep()` apaga el OLED (`display.power(false)`, `0xAE`; el framebuffer del panel se conserva) y corta el sonido (`sound.stop()`); luego un bucle de **wake de verificación**: `while (!buttons.anyPhysical()) { ... esp_light_sleep_start(); buttons.read(); }`. No se sale hasta que hay un botón **realmente** presionado; si `esp_light_sleep_start()` vuelve "solo" (GPIO por nivel que ya no re-dispara, o host USB-CDC que impide persistir el light sleep), se vuelve a dormir al instante y la pantalla queda apagada.
- **Diagnóstico del wake espurio:** cada vuelta mide cuánto durmió (`slept = nowMs() - t0`). Un light sleep que no **persistió** vuelve en ~0-5 ms sin botón (`ESP_ERR_SLEEP_REJECT`, o por debajo de `MIN_SLEEP_MS`); en placa sana duerme ~`WAKE_CHECK_MS`. Tras `SPURIOUS_LIMIT` espurios seguidos se muestra un aviso (`screenDiag()`: enciende el OLED y dibuja pantalla completa —"ERROR" (12x16) en el Header, "SLEEP FAIL" (12x16, invertido) en el cuadro del Menu, "The ESP32 is failing" (6x8, invertido) en el pie—, espera `DIAG_MS` y la apaga) y se marca `_diagSeen`. El contador se reinicia con cualquier vuelta "sana".
- **Despertar:** en `begin()` (setup), cada pin de `Config::Pin::BUTTONS` se habilita como fuente de wake con `gpio_wakeup_enable(pin, GPIO_INTR_HIGH_LEVEL)` (botones `INPUT_PULLDOWN`, pulsado = HIGH) y `esp_sleep_enable_gpio_wakeup()` (vía rápida) **más** `esp_sleep_enable_timer_wakeup(WAKE_CHECK_MS)` (garantía: despierta cada 200 ms para re-verificar por polling; con HW sano el botón despierta al instante por GPIO por nivel). Al volver con un botón real: `display.power(true)` (`0xAF`, misma imagen) y `buttons.begin()` para **re-anclar el estado** (el botón que despertó no se lee como "press", no navega). Si hubo diagnóstico (`_diagSeen`), la imagen retenida quedó deformada: se llama `_engine.repaint()` antes de re-anclar los botones.
- **Tras el wake** `_idle.start()` reinstala el contador para no volver a dormir al instante.

### Por qué light sleep (no deep sleep)
- **Conserva la RAM:** ventanas y `Storage` siguen vivas; `loop()` continúa donde quedó, sin reinicializar.
- **Cualquier GPIO despierta:** el wake por matriz GPIO admite todos los pines, incluido `GPIO47` (ACTION_LEFT). El deep sleep solo despierta por pines RTC (`EXT0`/`EXT1`) y perdería la RAM.

### API nueva que aporta
- `SleepManager(Engine&)` — el `.ino` la construye con `Engine` (`SleepManager sleepManager(engine);`) y solo llama `begin()`/`update()`.
- `SleepManager::begin()` — fuentes de wake del light sleep (GPIO por nivel + timer de verificación), antes en `setup()`.
- `SleepManager::update()` — marca el frame (idle + dormir), antes en `loop()`. Quita del `.ino` los globales `idleTimer`/`diagSeen` y las funciones `enterSleep()`/`screenDiag()`.
- `Buttons::anyPhysical()` — algún botón físicamente presionado (actividad del contador; ajeno a la supresión `ignoreUntilRelease()`).
- `Buttons::ignoreUntilRelease()` — supresión de entrada hasta soltar todos los botones (el que despertó no cuenta como pulsación sostenida).
- `Engine::isInGame()` — verdadero si hay partida en curso (el reposo no aplica). Hoy siempre `false` (**AISLADO**); al reactivar: `_state == State::NEW || _state == State::CONTINUE`.
- `Config::Power::IDLE_TIMEOUT_MS` (60 s), `Config::Power::WAKE_CHECK_MS` (200 ms; período del wake de verificación), `Config::Power::MIN_SLEEP_MS` (10 ms; por debajo, un retorno sin botón cuenta como espurio), `Config::Power::SPURIOUS_LIMIT` (3 espurios seguidos → diagnóstico), `Config::Power::DIAG_MS` (2 s; duración del aviso).
- `Engine::repaint()` — **repintado forzado** tras un diagnóstico: `display.clear()` puntual + `firstPrint(true)` de la ventana (el `forceRedraw` salta el guard `_clear`). En `MENU_SOUND` redibuja la base del Menu (`_menu.firstPrint(true)`) y encima el selector (`_menuSound.firstPrint(true)`), porque el `loop()` solo llamará al `print()` del parche.
- `Boot::firstPrint(true)` / `Legend::firstPrint(true)` / `Menu::firstPrint(true)` — piden el full frame (el `forceRedraw` salta `_clear`). Antes eran `forceRedraw()` (ponía `_clear = true`; el del `Menu` reseteaba `_done`): **comentados**.

### Notas
- `esp_timer` (base de `nowMs()`/`Stopwatch`) sigue avanzando durante el light sleep: el RTC no se detiene, el contador no se corrompe.
- El OLED conserva su RAM con `SSD1306_DISPLAYOFF` (0xAE): al volver con `SSD1306_DISPLAYON` (0xAF) se restaura la misma imagen. La versión instalada de Adafruit_SSD1306 no tiene `displayOn()`/`displayOff()`, así que `Display::power()` usa `ssd1306_command()`.
