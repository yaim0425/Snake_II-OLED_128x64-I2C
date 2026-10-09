# Snake_II-OLED_128x64-I2C — ESP32-S3

Proyecto: Snake_II-OLED_128x64-I2C (Snake II, estilo Nokia) para placa ESP32-S3 con
Arduino IDE. Programación orientada a objetos: cada clase en su archivo `.h` y `.cpp`.

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

En desarrollo. `Snake_II-OLED_128x64-I2C.ino` es el **wiring**: define los globales
(`Display`, `Buttons`, `Sound` — servicios; `Storage` — almacén de estado; todos
en `Globals.h`) y crea el `Engine`, que **posee las
ventanas** (`Boot`, `Legend`, `Menu`, `MenuDifficulty`, `MenuSound`,
`MenuCredits`, `Game`) como miembros. Las
constantes compartidas viven en `Config.h` (pines, geometría, dificultad, versión).

**Arquitectura:**
- **`Storage` (sección 22):** almacén global de estado que **no debe vivir en
  ninguna ventana** (hoy: mejor puntaje, sonido activo y dificultad; el resto de
  valores compartidos se añade ahí). Contenedor de datos puro, sin
  `begin()`/`update()`; su **instancia** —no sus métodos— es global
  (`storage`, `Globals.h`). **Solo creado por ahora:** la integración con
  `Menu`/`MenuDifficulty`/`Sound` está pendiente (esas clases siguen con sus
  propias copias).
- `Engine` es el despachador: posee las ventanas (no globales, no anidadas) y
  llama al `begin()` de la entrante al cambiar de estado. `loop()` hace la única
  lectura de botones del frame y vigila el **reposo** (sección 26): sin partida
  en curso y sin botón presionado durante `Config::Power::IDLE_TIMEOUT_MS`, el
  ESP32 duerme en light sleep y despierta con cualquier botón.
- Renderizado sin `clear()` global: cada ventana limpia solo en su primer frame y
  luego redibuja solo zonas dinámicas. `MenuDifficulty` y `MenuSound` son la
  excepción: no limpian nada, se dibujan **sobre el `Menu`** sustituyendo solo
  la banda de los rombos y vuelven con
  `changeState(State::MENU, false)` + `Menu::restoreDiamondBand()`, así que el
  título, el cuadro de la opción y el pie nunca se repintan en el ida y vuelta.
  Hoy solo `MenuSound` está fuera del aislamiento y su regreso hace el
  `begin()` completo del `Menu`: `Menu::restoreDiamondBand()` sigue comentada
  (ver sección 11).
- Flujo de arranque: `Boot` → `Legend` → `Menu`. La `Legend` solo aparece al
  arrancar; al volver al menú se pasa directo a `Menu`.
- **Aislamiento temporal (bloques `AISLADO` en `Engine.h`/`Engine.cpp`):**
  `Engine` posee y despacha `Boot`, `Legend`, `Menu` y `MenuSound` (reactivada
  en la sección 10.3); `Game`, `MenuCredits` y `MenuDifficulty` siguen
  existiendo como clases pero sus estados, miembros y transiciones están
  comentados. El flujo queda `Boot` → `Legend` → `Menu` y ahí se detiene:
  `Menu::confirm()` está comentado (la entrada a `MenuSound` no está cableada),
  `ACTION_RIGHT` solo alterna el resaltado (`_confirm`) y `ACTION_UP` devuelve
  la selección a `New`/`Continue`, así que el menú no sale hacia ninguna
  ventana.

**Clases extraídas de `Game`:**
- `Food` — alimento (normal/especial): estado, spawn en celdas libres, dibujo,
  temporizador.
- `Snake` — lógica pura (sin `Display`/`Sound`/`Food`): buffer circular, giro
  pendiente, wrap, colisión, sprites. `Game` coordina ritmo, input y dibujo.
- `Sprite.h` — tabla de sprites (header-only).
- `Timer.h` / `Timer.cpp` — reloj de 64 bits (`esp_timer_get_time`), `Stopwatch` y `Ticker`.
  Migración completa desde `millis()`.

**Características del juego:**
- Dificultad en caliente: `setDifficulty` recalcula `_moveDelay` al instante.
- Puntaje = nivel de dificultad (`uint16_t` para evitar overflow).
- Festejo de nuevo récord: ciclo "GAME OVER" → "BUT" → "YOU ARE" → "THE BEST"
  con fanfarria solo en "THE BEST".

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

- **Pantalla:** OLED SSD1306 monocromo (amarillo) 128x64, I2C, dirección `0x3C`.
- **Librerías:** Adafruit_SSD1306, Adafruit_GFX.
- **Botones:** configurados como `INPUT_PULLDOWN` en el juego original.

---

## 3. Estructura de archivos

Directorio: `D:\Documents\ESP32S3\Snake_II-OLED_128x64-I2C`

| Archivo | Contenido |
|---------|-----------|
| `Display.h` / `Display.cpp` | Clase `Display` (control del OLED: `begin()`, `clear()`/`show()`, `power(on)` para apagar/encender el panel SSD1306 —ahorro en reposo—, `fillRect`, `fillTriangle`, `drawText`, `drawBitmap`). Completa. |
| `Config.h` | Constantes compartidas del proyecto (namespace `Config`, sección 19): pines (`Config::Pin`: botones, buzzer, SDA/SCL), geometría y regiones de la pantalla (`Config::Screen`: 128×64, celda 8, dirección I2C, Header/Body), límites de la dificultad (`Config::Difficulty`), la versión del firmware (`Config::Version`: VERSION/RELEASE_DATE, la usa el pie del menú; NAME lo dibuja `Boot`), créditos (`Config::Credits`), tiempos de `Legend` (`Config::Legend`: NEXT/PERIOD/OFF, `HOLD` comentado), tamaño de rombo (`Config::Diamond`), repetición de botones (`Config::Button`: DELAY/TICK, **nuevo**), animación del `Scroller` (`Config::Scroller`) y parpadeo genérico (`Config::DefaultTimer`: PERIOD/OFF, `HOLD` comentado). Solo lo verdaderamente compartido; el resto es `static constexpr` en su clase. Sin `#define` para valores (constantes con tipo y ámbito). |
| `Globals.h` | Declara `extern` los **globales del proyecto**: `Display display;`, `Buttons buttons;` y `Sound sound;` (servicios) más `Storage storage;` (almacén de estado, sección 22), todos definidos en `Snake_II-OLED_128x64-I2C.ino` (sección 20). **No** declara el `Buzzer` (es interno de `Sound`). No define las ventanas: esas viven dentro de `Engine`. |
| `Buttons.h` / `Buttons.cpp` | Clase `Buttons` (lectura con debounce, `hold`/`pressed`/`released`/`anyHeld`; `MAX_BUTTONS` es el contador final del enum `Button`; `isSet` declarado en el header y definido en el `.cpp`). Completa. |
| `Boot.h` / `Boot.cpp` | Clase `Boot` (pantalla de arranque: Header con el mensaje "Press any button / to start" que **parpadea sin fase fija** —`PERIOD=500 ms`, oculto el 20 % (`Config::DefaultTimer`), `HOLD` comentado— y Body con el logo `Sprite::LOGO` (bloque 1bpp, 80×48) en blanco, volcado con `Display::drawBitmap(..., true, false)` → `SSD1306_WHITE`/`SSD1306_BLACK`; al entrar (`begin()`) suena la fanfarria `SFX_FANFARE`; se termina con cualquier botón con su sonido según el botón; la animación de bandas de líneas verticales anterior está comentada). Completa. |
| `Legend.h` / `Legend.cpp` | Clase `Legend` (panel de botones: pad MOVE a la izquierda con 4 flechas, 4 rombos completos de ACTION a la derecha en las posiciones de un pad que parpadean MUY rápido uno a la vez en ciclo lento —ciclo `NEXT=2500 ms` con el `Ticker`, **sin fase fija** (`HOLD` comentado), parpadeo `PERIOD=100 ms` al 50 %— y texto de la función del rombo activo en el pie (Back, Select / Pause, None, None), compuesto y mostrado con su propio `Scroller` (`_scroller`) para que **entre deslizándose** al cambiar de rombo; cualquier botón la cierra con un sonido según el botón pulsado: MOVE = CLICK, ACTION_UP = BACK, ACTION_RIGHT = CONFIRM). Completa. |
| `Scroller.h` / `Scroller.cpp` | Clase `Scroller` (scroller de 1 bit: una sola banda de **ancho completo de pantalla**, texto como array de `int8_t` donde cada byte = 1 columna de 8 px; fondo siempre negro y texto blanco; `setTexto()` compone el texto centrado, fija la fila donde se imprimirá, deriva el alto de la franja del tamaño (`8 * size`, máx. `Config::Scroller::MAX_H` = 32), calcula el límite de caracteres según el tamaño y trunca silenciosamente; `startSlide()` arranca en `-timeCurtain()` y limpia 1 px justo sobre/bajo la franja; `update()` avanza 1 px cada 4 ms mientras `print()` vuelca la bandeja **con la cortina activa** —`curtain()` anclada al frente, helper `timeCurtain()` = `6 * _size + 4 * _size - 1`, texto que entra solo cuando `_x >= 0`—; usada por `Menu` con 1 instancia —transición **activa** en `nextOption()`—, por `MenuCredits` con 2 instancias sincronizadas y por `Legend` con 1 instancia para el texto del pie). Completa. |
| `Menu.h` / `Menu.cpp` | Clase `Menu` (menú con scroller de 1 bit —1 banda del `Scroller`—, rombos/triángulos de posición marcados con `toggleDiamond()`/`toggleTriangle()` sobre la geometría centrada en el Body de `Config::MenuStrip`, navegación con repetición y anti-entrada `_holdButtons`). **En iteración**: `begin(showContinue, selected)` fija la lista y la selección; `navigate()` (MOVE con el helper `ButtonRepeat`), `action()` (`ACTION_UP` vuelve a `New`/`Continue`, `ACTION_RIGHT` alterna `_confirm`), `holdButtons()` (bloquea hasta soltar los botones), `blinkOption()` (parpadeo con el helper `Blink`) y `nextOption()` (vuelo lateral del `Scroller`, activo). La API anterior (`setOptions`, `setContinueAvailable`, `setBestScore`, `setSelected`, `setTitle`, `setShowFooter`, `confirm()`, `restoreDiamondBand()`) está **comentada** en `Menu.h`. |
| `MenuDifficulty.h` / `MenuDifficulty.cpp` | Clase `MenuDifficulty` (selector de nivel 1..10, `< N >`, con repetición al mantener presionado; al mantener, solo queda fija la flecha del botón activo). Completa. Ventana hermana: vive sobre el `Menu` ya dibujado, sustituye solo la banda de rombos (45..53) y no hace `clear()`. Guarda el nivel confirmado (`difficulty()`), que el `Engine` pasa a `Game::setDifficulty`. |
| `MenuSound.h` / `MenuSound.cpp` | Clase `MenuSound` (selector On/Off con una flecha en el lado del destino, sobre la misma banda). **Reactivada (fuera del aislamiento)**: el `Engine` ya la posee y despacha (`MENU_SOUND`), pero la entrada desde el `Menu` sigue pendiente. Como `Display::getWidth()`/`getTextWidth()` siguen comentadas en `Display.h`, centra a mano con `Config::Screen::WIDTH` y `strlen * 6`. El valor real se aplica al global `Sound` con `setEnabled`. |
| `MenuCredits.h` / `MenuCredits.cpp` | Clase `MenuCredits` (ventana de créditos con 3 entradas navegables con transición lateral —2 bandas sincronizadas del `Scroller` compartido— y `SFX_CLICK` al navegar, vuelve al menú con `ACTION_UP`). Se llama así, y no `Credits`, para distinguirla de una hipotética ventana de créditos general: esta es la que se abre desde la opción "Credits" del `Menu`. Completa. |
| `Game.h` / `Game.cpp` | Clase `Game` (ventana del juego de la serpiente: estados NEW/CONTINUE del `Engine`). **Coordina**: dificultad/velocidad, lectura de botones (MOVE → `Snake::turn`), `snake.step()` con manejo del resultado, alimento (`Food`), puntaje, sonidos, overlays y volcado del tablero con los segmentos de `Snake` (celda de `Config::Screen::CELL`, sprites escalados con `SPRITE_SCALE`). Completa. |
| `Food.h` / `Food.cpp` | Clase `Food` (alimento del tablero, extraído de `Game`): estado (posición, presencia, tipo normal/especial), generación en celdas libres (`spawn`, que consulta la ocupación al tablero vía `Game::occupied`), dibujo del rombo (normal) o del sprite `SPECIAL_FOOD` (especial) —la celda la toma de `Config::Screen::CELL`, no declara una propia— y el temporizador de la comida especial. Completa. |
| `Snake.h` / `Snake.cpp` | Clase `Snake` (lógica pura de la serpiente, extraída de `Game`): buffer circular de segmentos, dirección commitida + giro pendiente (sin reversa directa), paso con wrap, colisión, comer/crecer y elección de sprites de las partes. **Sin `Display`/`Sound`/`Food`**: `Game` coordina el ritmo, el alimento, los sonidos y el dibujo. Completa. |
| `Engine.h` / `Engine.cpp` | Clase `Engine` (despachador de ventanas, antes `App`). **No anida las ventanas** pero las **posee como miembros** (`_boot`, `_menu`, `_menuDifficulty`, `_menuSound`, `_menuCredits`, `_game`, `_legend`): su estado interno decide qué ventana corre y cuándo cambiar (`changeState()`, que llama al `begin()` de la ventana entrante). Los `begin()` de las ventanas se lanzan desde `setup()` vía `engine.begin()`. `changeState()` admite un `beginWindow = false` para volver al menú desde `MenuDifficulty`/`MenuSound` sin repintar la página entera. `isInGame()` indica si hay partida en curso (el reposo no aplica). Completa. |
| `Snake_II-OLED_128x64-I2C.ino` | Enlace de dependencias (wiring). Define los **servicios globales** (`display`, `buttons`, `sound`) y crea `Engine engine;` (que posee las ventanas). `Sound` se construye con el pin: `Sound sound(Config::Pin::BUZZER);` (el `Buzzer` es suyo). `setup()` llama `display.begin()`, `buttons.begin()`, `sound.begin()` (que inicializa su `Buzzer` interno) y `engine.begin()`, y configura el **despertar por botón del light sleep** (`gpio_wakeup_enable` en cada pin + `esp_sleep_enable_gpio_wakeup`); `loop()` hace la **única lectura de botones del frame** (`buttons.read()`), vigila el **reposo** (`idleTimer` + `enterSleep()` —sección 25—) y llama `engine.update()`, `engine.print()`, `sound.update()` y `display.show()`. |
| `Buzzer.h` / `Buzzer.cpp` | Clase `Buzzer` (capa de hardware de sonido: un tono no bloqueante vía LEDC). **No es un servicio global**: la posee `Sound` por valor (sección 10.2). Completa. |
| `Sound.h` / `Sound.cpp` | Classe `Sound` (secuencias de los efectos del juego sobre su `Buzzer` interno —miembro por valor, inicializado en `begin()`—, con `setEnabled` para silenciar). Completa. |
| `Storage.h` / `Storage.cpp` | Clase `Storage` (almacén de estado compartido **fuera de las ventanas**): mejor puntaje, sonido activo y dificultad, con valores por defecto tomados de `Config` (`_bestScore = 0`, `_soundEnabled = true`, `_difficulty = DEFAULT_LEVEL`) y clamp en `setDifficulty`. Contenedor de datos puro: sin `begin()`/`update()`/`print()` y sin dependencias de hardware. La **instancia** —no los métodos— es global (`storage`, sección 22). Completa; **pendiente de integrar**: todavía no la lee ni la escribe nadie. |
| `Sprite.h` | Namespace `Sprite` (tabla de sprites de la serpiente, estilo Nokia: cola, cuerpo, curvas, cabeza cerrada/abierta y panza; sprites de 4×4 px + sprite de la comida especial de 8×4 px + el logo del arranque `LOGO` de 80×48 px como bloque 1bpp, dibujado con `Display::drawBitmap()`, sección 15). Solo datos (header-only, sin `.cpp`). Adaptada al estilo del proyecto. |
| `Timer.h` / `Timer.cpp` | Reloj de 64 bits y cronómetros compartidos (`nowMs()`, `Stopwatch`, `Ticker`), basados en `esp_timer_get_time()` (sección 21). El header solo declara; las definiciones están en el `.cpp`, porque el `inline` en el header repetía el mismo código en cada `.cpp` que lo incluye. |
| `ButtonRepeat.h` / `ButtonRepeat.cpp` | Helper de **repetición al mantener un botón** (sección 23): primer paso inmediato (`pressed`) y, tras `Config::Button::DELAY` (400 ms), un paso cada `Config::Button::TICK` (100 ms) mientras sigue mantenido (`hold`). Encapsula los dos `Stopwatch` que antes vivían sueltos en `Menu` y `MenuDifficulty` (donde además habían divergido: `hold` vs el viejo `state`). **Sin `reset()`** (YAGNI: hoy no hay call site). Lo usan `Menu` (`_repeat`) y `MenuDifficulty` (`_repeat`). |
| `Blink.h` / `Blink.cpp` | Helper de **parpadeo** (sección 24): encapsula el `Stopwatch` (ancla del período) + el recuerdo de la última fase (`_last`, para el flanco) que cada ventana llevaba por separado. API de 3 métodos: `start()` (ancla y olvida la fase), `isVisible(period, offPct)` (fase actual, `true` = visible) y `changed(period, offPct)` (true solo en el flanco). Lo usan `Boot`, `Legend` y `Menu` (redibujan en el flanco con `changed()`/`isVisible()`) y `MenuDifficulty`/`MenuSound` (solo la fase cruda `isVisible()`). |
| `Draw.h` / `Draw.cpp` | Namespace de **primitivas de dibujo** (sección 25): `triangle(dir, cx, cy, show)` y `diamond(cx, cy, show)`, los marcadores de posición que `Menu` y `Legend` tenían duplicados. Sin estado (usan el global `display` y `Config::Diamond::SIZE`); `dir` = 1↑, 2→, 3↓, 4←. |
| `PROYECTO.md` | Este documento. |
| `LICENSE.md` | Licencia del proyecto: **MIT**, con el texto canónico en inglés (traducirlo haría que GitHub dejara de reconocerlo). Para cambiar el titular basta con editar la línea `Copyright (c) 2026 <nombre>` de ese archivo. |
| `THIRD_PARTY_NOTICES.md` | Aviso de atribución de las dependencias de terceros (Adafruit GFX y SSD1306, BSD-3; core Arduino-ESP32, LGPL-2.1; ESP-IDF, Apache-2.0) y constancia de que **el repositorio no distribuye código ni artwork de terceros** (los sprites de `Sprite.h` son tablas de bits propias). No es una obligación legal —las librerías se enlazan, no se distribuyen— pero se mantiene al día si cambian de versión las del gestor de Arduino. |

Nota: Arduino solo compila el `.ino` del sketch. El respaldo quedó como `.txt`
para que no interfiera en la compilación.

Nota (refactor de servicios globales): desde esta tarea las clases nuestras ya no
reciben `Display`/`Buttons`/`Sound`/`Buzzer` por constructor — las usan
directamente vía `Globals.h` (que se incluye solo en los `.cpp`, no en los
`.h`). `Sound` es la **única** clase que usa `Buzzer`, y lo **posee por valor**
(`Sound(uint8_t pin)` lo construye con el pin y `Sound::begin()` lo inicializa);
ya no quedan `Buzzer buzzer;` global ni el bind `Sound(Buzzer&)`. Se mantienen
constructor los
parámetros de configuración: `Menu(bestScore, version)`, `MenuCredits()`,
`Food(cols, rows, top)` y `Sound(pin)`
(y `Game()`/`Boot()`/
`Legend()` quedan sin parámetros). El constructor propio de cada ventana está
documentado en su sección.

---

## 4. Pantalla: geometría

- Tamaño: **128x64 píxeles**.
- Celda: **8x8 px** → rejilla de **16 columnas × 8 filas**.
- Los métodos usan coordenadas de celda o de píxel según el método.
- `cellSize` configurable por constructor (default 8).

### Fuente / tamaño de texto (Adafruit_GFX)

| Enumerador  | textSize | Dimensiones |
|-------------|----------|-------------|
| `TEXT_6x8`  | 1        | 6×8 px      |
| `TEXT_12x16` | 2       | 12×16 px    |
| `TEXT_18x24` | 3       | 18×24 px    |

---

## 5. Clase `Display` — API

Ubicación: `Display.h` / `Display.cpp`.

### 5.1 Constructor

```cpp
Display(uint8_t sda = Config::Pin::OLED_SDA, uint8_t scl = Config::Pin::OLED_SCL,
        uint8_t address = Config::Screen::ADDRESS, uint8_t width = Config::Screen::WIDTH,
        uint8_t height = Config::Screen::HEIGHT, uint8_t cellSize = Config::Screen::CELL);
```

Los valores por defecto (pines I2C, dirección y geometría) vienen de `Config`
(`Config::Pin` / `Config::Screen`); cualquier parámetro se puede sobreescribir.

La lista de inicialización del constructor sigue **el orden de declaración de los
miembros en `Display.h`** (en C++ los miembros se inicializan en orden de
declaración, no en el de la lista; un desajuste genera el warning `-Wreorder`).
Actualmente: `_screen`, `_sda`, `_scl`, `_address`, `_width`, `_height`,
`_cellSize`, `_columns`, `_rows`.

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

La pantalla se divide en dos regiones para el texto. Las alineaciones (`TextAlign`)
se aplican dentro de la región elegida:

| Región | Zona |
|--------|------|
| `REGION_HEADER` | (0,0)  - (128,15)  |
| `REGION_BODY`   | (0,16) - (128,64)  |
| `REGION_FULL`   | (0,0)  - (128,64)  |

Cualquier texto puede alinearse en cualquiera de las 9 posiciones de cada región
y con cualquier tamaño (`TEXT_6x8`, `TEXT_12x16`, `TEXT_18x24`).

### 5.4 Métodos

| Método | Descripción |
|--------|-------------|
| `void begin()` | `Wire.begin(sda, scl)`, crea el OLED y lo limpia. **Idempotente:** si la pantalla ya quedó inicializada (`_screen != nullptr`) no hace nada, de modo que llamarla dos veces no reasigna el OLED ni filtra memoria (si el primer intento falló, `_screen` quedó en `nullptr` y un segundo llamado reintenta). |
| `void clear()` | Limpia el buffer de la pantalla. |
| `void show()` | Envía el buffer al OLED. |
| `void power(bool on)` | Apaga (`0xAE`) / enciende (`0xAF`) el panel SSD1306. Apagado solo para **ahorro en reposo** (sección 26): el framebuffer del panel se conserva, así que al volver a encender se restaura la misma imagen sin repintar. No afecta al buffer en RAM de `Adafruit_SSD1306`. |
| `void drawPixel(x, y, white=false)` | Dibuja 1 píxel (`true` = blanco, `false` = negro). |
| `void fillRect(x, y, w, h, white=false)` | Rectángulo relleno. Reenvío directo a `Adafruit_SSD1306::fillRect`, con el guard de `_screen == nullptr` del resto de métodos. El color no se propaga como `uint16_t` de la pantalla: la API de `Display` lo expresa como `bool white` (`true` = blanco, `false` = negro), igual que `drawPixel`, para que las ventanas no manejen las constantes de color. |
| `void fillTriangle(x0, y0, x1, y1, x2, y2, white=false)` | Triángulo relleno por sus tres vértices (en cualquier orden). Reenvío directo a `Adafruit_GFX::fillTriangle` con el mismo `bool white` que `fillRect`. Quedan en `Display.cpp` los helpers estáticos `sortByY()`/`edgeAt()` de una versión anterior que rasterizaba el triángulo por filas (ordenar los vértices por `y` y cortar los dos lados activos en cada fila): ya no los usa nadie y están pendientes de borrar. |
| `TextPos getTextPos(texto, align, size=1, region=FULL)` | Devuelve x,y (esquina sup-izq) según alineación y región. |
| `void drawText(texto, x, y, size=1, tColor=SSD1306_WHITE, bgColor=SSD1306_BLACK)` | Imprime texto en posición píxel exacta; `tColor` = color del texto y `bgColor` = color de fondo. |
| `void drawTextInverted(texto, x, y, size=1)` | Imprime texto en negro (sobre cualquier fondo) en posición exacta. |
| `void drawTextAligned(texto, align, size=1, region=FULL)` | Posición según alineación/región + imprime. |
| `void drawHighlight(texto, x, y, size=1)` | Texto resaltado (cuadro blanco + texto invertido) en posición exacta. |
| `void drawHighlightAligned(texto, align, size=1, region=FULL)` | Igual que `drawHighlight`, posicionado por alineación/región. |
| `uint8_t getTextWidth(texto, size=1)` | Ancho del texto = `strlen × 6 × size` (clamp 255). |
| `uint8_t getTextHeight(size=1)` | Alto del texto = `8 × size` (clamp 255). |
| `uint8_t getWidth()` / `getHeight()` | Ancho/alto en píxeles (128/64). |
| `uint8_t getCellSize()` | Píxeles por celda (8). |
| `uint8_t getColumns()` / `getRows()` | Columnas (16) / filas (8) de la rejilla. |
| `Adafruit_SSD1306& screen()` | Acceso directo al objeto OLED. **Referencia segura (no-opcional):** si `begin()` falló devuelve un OLED "mudo" en RAM (inicializado la primera vez) en lugar de desreferenciar `nullptr`; comprobar `isReady()` para saber si hay pantalla real. Precondición: `begin()` ya se llamó. |
| `bool isReady()` | `true` si la pantalla quedó operativa (el OLED respondió en `begin()`); `false` si la inicialización falló (pantalla ausente o sin respuesta por I2C). Tras un fallo, `screen()` sigue devolviendo una referencia válida (fallback en memoria) y el resto de métodos (`clear`, `show`, `drawText`, etc.) no hacen nada. |

### 5.5 Renderizado de texto y resaltado

- **Texto normal (`drawText`/`drawTextAligned`):** blanco sobre fondo negro, sin cuadro.
- **Resaltado (`drawHighlight`/`drawHighlightAligned`):** cuadro blanco (`fillRoundRect`,
  radio 0) con el texto invertido (negro) centrado dentro del cuadro.
- El cuadro **rebasa al texto**: `+2*size` px en X (1/2/3 px por lado) y `+1` px
  arriba y `+1` px abajo (total `+2` px en Y), centrando el texto en el cuadro.

```
size=1 -> texto 6x8      -> cuadro  (6+2)  x (8+2)  = 8x10
size=2 -> texto 12x16    -> cuadro  (12+4) x (16+2) = 16x18
size=3 -> texto 18x24    -> cuadro  (18+6) x (24+2) = 24x26
```

- No existen "botones no seleccionados": se usa texto normal o resaltado.

---

## 6. Decisiones de diseño

- **`char*` en lugar de `String`** en la clase `Display`: evita fragmentación del
  heap en bucles continuos. Las funciones de impresión aceptan `char*` (NULL-terminated).
- **Retornos `uint8_t`** para anchos/altos de texto: suficiente para la pantalla.
  Con clamp a 255 para evitar desbordes.
- **El ancho de texto usa `strlen` (bytes):** con caracteres UTF-8 (á, é, ñ) el ancho
  sería incorrecto. Actualmente se usan solo ASCII.
- **Posición de texto devuelta:** siempre la *esquina superior-izquierda* del área del texto.
- **Clase `Display` encapsula** el OLED; el juego que estaba en `Snake_II-OLED_128x64-I2C.ino` usaba
  `Adafruit_SSD1306` directamente (orden: `Screen`). Al integrar, migrar al objeto `Display`.
- **Pie de archivo "Fin":** todos los archivos fuente (`.h`/`.cpp`) terminan con el
  bloque de comentario `// ===…` + `// Fin` + `// ===…` (con una línea en blanco antes
  y sin salto de línea final), igual que `Snake_II-OLED_128x64-I2C.ino`.
- **Idioma del código: inglés.** Los identificadores son en inglés (clases, métodos,
  variables, constantes y enums; p. ej. `Menu::Option` = `OPT_NEW`/`OPT_CONTINUE`/
  `OPT_DIFFICULTY`/`OPT_SOUND`/`OPT_CREDITS`, `Config::Difficulty::MIN_LEVEL/MAX_LEVEL/DEFAULT_LEVEL` y
  los estados
  del `Engine` `NEW`/`CONTINUE`/`MENU_CREDITS`/`MENU_DIFFICULTY`/`MENU_SOUND`). Los comentarios y la documentación (`PROYECTO.md`)
  se mantienen en español (convención del proyecto).
- **Servicios globales, ventanas internas:** `Display`, `Buttons` y `Sound` son los
  únicos **globales** (`Globals.h`: `extern`, definidos en
  `Snake_II-OLED_128x64-I2C.ino`; el orden de construcción ya no importa porque cada servicio se
  inicializa en su `begin()` desde `setup()`, y así `Scroller`/`Food`
  pueden consultar `display.getWidth()` al construir `Engine`). El `Buzzer` **no**
  es global: lo contiene `Sound` por valor. Las **ventanas no son
  globales**: viven dentro de `Engine` (sección 11), así ninguna clase puede
  llamarlas por fuera del despachador.

---

## 7. Clase `Buttons` — API

Ubicación: `Buttons.h` / `Buttons.cpp`. Basada en el diseño de `GameInput` (referencia
`D:\Documents\ESP32S3\Snake_2\GameInput.{h,cpp}`).

### Pines (orden del enum)

Los pines de los botones viven en `Config::Pin::BUTTONS` (sección 19),
`Snake_II-OLED_128x64-I2C.ino` los pasa al constructor:

```cpp
Buttons buttons(Config::Pin::BUTTONS);   // en Snake_II-OLED_128x64-I2C.ino
```

### Enum

```cpp
enum Button : uint8_t {
  MOVE_UP = 0, MOVE_RIGHT, MOVE_DOWN, MOVE_LEFT,
  ACTION_UP, ACTION_RIGHT, ACTION_DOWN, ACTION_LEFT,

  MAX_BUTTONS   // contador final (= 8); antes era static constexpr MAX_BUTTONS = 8
};
```

### Métodos

| Método | Descripción |
|--------|-------------|
| `Buttons(const int8_t* pins, uint32_t buttonDelay = 30)` | Constructor, recibe los pines y el tiempo de debounce en ms. |
| `void begin()` | Configura `INPUT_PULLDOWN` y lee el estado inicial. |
| `void read()` | Leer físicamente, aplicar debounce y generar eventos. **Se llama una sola vez por `loop()`** (en `loop()`, antes de `engine.update()`); las ventanas solo consultan `hold`/`pressed`/`released` sin volver a leer. |
| `bool hold(index)` / `pressed(index)` / `released(index)` | **Acceso único** por botón, con los valores del enum `Button`: `hold(Buttons::MOVE_LEFT)` = estado actual (mantenido; antes se llamaba `state()`), `pressed(Buttons::ACTION_RIGHT)` = evento de pulso (true solo en el ciclo en que se presiona), `released(...)` = evento de liberación. Los 24 getters con nombre (`moveUp()`/`actionRightPressed()`/`...,Released()`, etc.) se eliminaron: API única sin boilerplate. |
| `bool anyHeld()` | true si **algún** botón está presionado ahora (no distingue cuál). La usa el wiring para el **reposo** (sección 26): cualquier pulso cuenta como actividad y reinicia `idleTimer`. |

### Diseño del debounce (estados agrupados en bytes)

- Lee los 8 botones agrupados en bytes (`uint8_t`, 1 bit por botón,
  HIGH = presionado con `INPUT_PULLDOWN`), igual que el `estados` del
  ejemplo de referencia: `_rawButtons` (físico sin filtrar), `_buttons`
  (confirmado), `_pressed`/`_released` (eventos de 1 ciclo).
- `_lastButtons` ya no existe: los eventos Pressed/Released se detectan
  comparando el bit del botón con su estado confirmado anterior antes de
  actualizarlo (no hace falta guardar todo el estado previo).
- Verificación de bits con el helper privado estático
  `bool isSet(uint8_t states, uint8_t boton)` → `states & (1 << boton)`,
  **declarado en `Buttons.h` y definido en `Buttons.cpp`** (antes era
  `static inline` con cuerpo en el header). **Bug conocido (pendiente):** la
  definición del `.cpp` no lleva el calificador `Buttons::`, así que
  `Buttons::isSet` queda sin definir y el enlace falla con *undefined
  reference* (no corregido: no se toca código hasta que se ordene).
- Detecta el cambio físico por bit (`raw ^ _rawButtons`) y toma nota del
  instante por botón.
- Solo acepta el nuevo estado tras `_buttonDelay` ms de estabilidad.
- Almacenamiento reducido: de 4 arrays `bool[8]` (32 B) + 1 byte a 4 bytes.

### Anticonflicto MOVE

Si **2 o más botones MOVE se confirman a la vez** (2+ bits de la máscara
`MOVE_MASK` = bots 0..3), se **anula la activación de todos**: esa limpieza se
aplica al final de `read()` sobre el estado confirmado y los eventos del frame
(`_buttons`, `_pressed` y `_released` se quedan sin esos bits). Así ningún botón
MOVE queda activo mientras hay simultaneidad (el pad direccional es excluyente);
los botones `ACTION` no participan del anticonflicto (se pueden pulsar a la vez
que un MOVE). Al liberar uno, si queda un único MOVE presionado, ese vuelve a
activarse solo (su `hold()/pressed()` reflejan de nuevo al botón que queda).

---

## 7. Clase `Menu` — API

Ubicación: `Menu.h` / `Menu.cpp`.

### Constructor

```cpp
Menu(uint16_t bestScore = 0);
```

Los servicios `Display`, `Buttons` y `Sound` son globales (sección 20); el
único parámetro de configuración es el puntaje máximo. El `Scroller` interno se
construye con `_scroller()` (constructor por defecto). El texto del pie sale de
`Config::Version::VERSION` (sección 19).

### Métodos

| Método | Descripción |
|--------|-------------|
| `void begin(bool showContinue = false, int8_t selected = OPT_NEW)` | Fija la lista visible (`showContinue`: `true` = 5 opciones con "Continue", `false` = 4 sin él) y la selección inicial (enum `Option`; si "Continue" no está visible la selección efectiva la resuelve `navigate()`), reinicia los flags (`_holdButtons`, `_lastScroll`, `_confirm`, `_done`, `_clear`), arranca el parpadeo (`_blink.start()`) y deja `_repeat` listo. `_clear` provoca el `clear()` completo + `firstPrint()` en el primer `print()` (de paso pone `_bestScore` a 0). |
| `void update()` | `if (_done) return;` y en ese orden: `holdButtons()` (desbloqueo al soltar), `navigate()`, `action()`; si el `Scroller` está volando (`_lastScroll`) lo avanza y **congela el resto del frame** (mientras `update()` devuelva `true` sale sin más), y al terminar el vuelo reancla `_blink` (`_blink.start()`). |
| `void print()` | `if (_done) return;` y encadena `firstPrint()` (solo el primer frame: `clear()` + título, cuadro, texto, marcadores y pie), `blinkOption()`, `nextOption()` y `_scroller.print()` (no-op en reposo, sección 14). |
| `void showOptions()` | Borra la banda entre el cuadro y el pie y dibuja los marcadores de las opciones no seleccionadas + el rombo de la seleccionada; omite `OPT_CONTINUE` si está oculta. |
| `int8_t selected()` | Opción seleccionada (enum `Option`; restaurado). |
| `bool done()` | `true` cuando el menú pide salir. Hoy **nada lo pone** (`_done = true` en `action()` está comentado): con el `Engine` aislado el menú es un callejón. |

**API en pausa (comentada en `Menu.h`):** `setOptions`,
`setContinueAvailable`, `setBestScore`, `setTitle`, `setShowFooter`,
`setSelected`, `confirm()` y `restoreDiamondBand()`. Por eso `Engine` no sale
del estado `MENU` (ver bloque `AISLADO`, sección 11).

### Internos (privados)

| Método | Descripción |
|--------|-------------|
| `void holdButtons()` | **Anti-entrada:** si `_holdButtons` está puesta, solo observa `buttons.hold()` de los 8 botones; en cuanto **todos** se liberen se apaga y quedan libres `navigate()`/`action()`. Así, entrar al menú con un botón aún pulsado (p. ej. el que cerró la ventana anterior) no mueve la selección. (`pressed()` no serviría: es evento de borde y ese pulso ya ocurrió en un frame anterior.) |
| `void navigate()` | Si `_holdButtons` está puesta, sale sin hacer nada. `MOVE_LEFT`/`MOVE_RIGHT` con el helper `ButtonRepeat` (`_repeat.step()`): paso inmediato al pulsar y, tras `Config::Button::DELAY` (400 ms), un paso cada `Config::Button::TICK` (100 ms). Salta `OPT_CONTINUE` si `_showContinue` es `false` y toca `SFX_CLICK` al mover. Límites en `OPT_NEW`/`OPT_COUNT - 1`. |
| `void action()` | Si `_holdButtons` está puesta, sale. `ACTION_UP` → la selección vuelve a `OPT_CONTINUE`/`OPT_NEW` con `SFX_BACK`; `ACTION_RIGHT` → alterna `_confirm` con `SFX_CONFIRM` (solo el resaltado: la salida está comentada). |
| `void blinkOption()` | Parpadeo de la opción: con `_confirm` sale del menú (`_confirm = false; _done = true`); si no, encadena los guards `if (!_scroller.done()) return;`, `else if (_lastSelected != _selected) return;` (mientras la selección cambia no parpadea el rombo de la anterior) y `else if (!_blink.changed(PERIOD, OFF)) return;`. Si el `Scroller` está aún volando (confirmación a media animación) borra el cuadro (`fillRect(0, BOX_TOP-1, WIDTH, BOX_HEIGHT+2)`, blanco) y lo re-compone (`_scroller.begin()`); luego repinta el texto y el rombo (`toggleText` + `toggleDiamond`) con `const bool visible = _done \|\| _blink.isVisible(PERIOD, OFF);`. |
| `void nextOption()` | Si la selección cambió: restaura los marcadores de la anterior (`toggleDiamond(_lastSelected, false)` + `toggleTriangle(_lastSelected, true)`), coloca los de la nueva (`toggleTriangle(_selected, false)` + `toggleDiamond(_selected, true)`), compone `OPTION[_selected]` en el `Scroller` con `setTexto(..., BOX_TOP, _lastSelected > _selected, TEXT_12x16)` y arranca `startSlide()` (dirección según el sentido del salto), marcando `_lastScroll`. |
| `void toggleText(bool show)` | Pinta/borra el texto de la opción dentro del cuadro (`drawText` centrado en `BOX_TOP`, `TEXT_12x16`, texto negro sobre blanco cuando `show`). |
| `void toggleDiamond(int8_t diamond, bool show)` / `void toggleTriangle(int8_t triangle, bool show)` | Pinta/borra el marcador de la opción dada (centro en `(i+1)·_space`, restando 1 si "Continue" está oculta): `toggleDiamond` dibuja un rombo (`Draw::diamond` en `DIAMOND_Y`) y `toggleTriangle` el triángulo superior (`Draw::triangle` en `TRIANGLE_Y`); `toggleTriangle` sale si el índice es `OPT_COUNT`. El dibujo de las formas vive en el namespace `Draw` (sección 25). |
| ~~`bool holdRepeat(uint8_t button)`~~ | **Eliminado**: la repetición por mantención se extrajo al helper `ButtonRepeat` (sección 23), miembro `_repeat`; `navigate()` lo consulta con `_repeat.step(Buttons::MOVE_*)`. Ya no hay parámetros `DELAY`/`TICK` locales en `Menu` (usa los de `Config::Button`). |

### Las opciones "Difficulty" y "Sound" son ventanas

**Estado actual:** `confirm()` está comentado y el `Engine` está aislado (sección
11), así que hoy nada abre estas ventanas; el comportamiento descrito a
continuación es el previsto al reactivarlos.

Al confirmar cualquiera de las dos, el `Engine` abriría `MenuDifficulty` o `MenuSound`
(ventanas hermanas, sección 11) en vez de editar el valor dentro del `Menu`. El
`Menu` solo navega y compone; el estado editable y su dibujo viven ya en sus
propias ventanas. `Menu::confirm()` (comentado) devolvía también esas dos
opciones.

Ambas ventanas **no borran la pantalla**: entran sobre el `Menu` ya dibujado,
sustituyen únicamente la banda de rombos (45..53) por su selector (`< N >` con
sus dos flechas, o `ON`/`OFF` con su flecha única) y, al salir, el `Engine`
devuelve el control al `Menu` **sin llamar a su `begin()`**
(`changeState(State::MENU, false)`), que solo repinta esa misma banda. El resto
de la ventana —título, cuadro de la opción y pie con "Best"— no se vuelve a
tocar en todo el ida y vuelta.

Anteriormente esta edición era *inline* dentro del propio `Menu` (con
`beginSoundEdit`/`beginDifficultyEdit`, `drawSoundSelector`/`drawDifficultySelector`
y la repetición por mantención): todo eso se eliminó al extraerlo a las ventanas.
La repetición, ya como helper compartido, vive en `ButtonRepeat` (sección 23).

### Opciones y enum

```cpp
enum Option : uint8_t {
  OPT_NEW = 0, OPT_CONTINUE, OPT_DIFFICULTY, OPT_SOUND, OPT_CREDITS,

  OPT_COUNT
};
```

El enum documenta las **5 opciones lógicas** (`OPT_COUNT` es su contador) y la
tabla de textos es `OPTION[OPT_COUNT]` (antes `OPTION_TEXT`). La lista visible
varía: con `Continue` disponible (`begin(showContinue = true)`) los índices
coinciden con el enum; sin él, `_showContinue` hace que `navigate()` **salte
`OPT_CONTINUE`** al subir/bajar (`if (!_showContinue && _selected == OPT_CONTINUE)
_selected--/++`). El mapeo interno `optionAt`/`indexOfOption` está comentado.

### Diseño del menú (scroller de 1 bit)

- **Título:** `Config::Version::NAME` ("Snake II"), `TEXT_12x16`, centrado en el
  Header. **Estático:** se dibuja una sola vez al entrar (tras el `clear()`
  completo de `firstPrint()`, protegido por `_clear`) y ya no se redibuja.
- **Pie:** banda blanca `55..63` (`fillRect(0, FOOT_TOP-1, WIDTH, FOOT_H+1)`),
  con "Best " + puntaje a la izquierda y la versión a la derecha, ambos en
  `TEXT_6x8` sobre blanco en la fila `FOOT_TOP = 56`. **Estático** (se dibuja en
  `firstPrint()`; `setBestScore` está comentado, así que no cambia tras entrar).
- **Cuadro de selección:** **fijo**, de **ancho completo** (128 px) y
  **centrado en la mitad del Body**: `BOX_HEIGHT = 16` y
  `BOX_TOP = BODY_MIDDLE - BOX_HEIGHT/2` (= 26), es
  decir el blanco ocupa `25..42` (`fillRect(0, BOX_TOP-1, WIDTH, BOX_HEIGHT+2)`).
  El texto de la opción (`TEXT_12x16`) se pinta en `BOX_TOP` = 26 y solo se toca
  al cambiar de opción (vuelo del scroller) o al parpadear (`blinkOption()` →
  `toggleText`). **No se mueve.**
- **Animación (scroller de 1 bit):** en la clase **`Scroller`** (ver
  sección 14, "Clase `Scroller`"), que `Menu` instancia con **1 instancia**
  (texto 12x16) y `MenuCredits` con **2 instancias sincronizadas** (rol 12x16 +
  nombre 6x8). Cada opción se compone **antes** de mostrarse en un **array de
  `int8_t`** (cada byte = 1 columna de 8 px, `1` = glifo, `0` = fondo)
  **centrada**, mediante `setTexto()` que dibuja el texto en un canvas
  auxiliar y extrae las columnas. La franja es siempre **fondo negro y texto
  blanco**. Al navegar `MOVE_RIGHT` la tira entra por la **derecha** (se mueve
  hacia la izquierda); con `MOVE_LEFT` por la **izquierda**. Arranca **fuera
  de pantalla** y avanza **una columna por cada `ANIM_TICK` ms** (`update`,
  acumulado por tiempo; vuelo total ≈ `128 × 4 ms ≈ 0,5 s`). Si se navega a
  mitad de la animación, la banda conserva lo que había y la nueva tira se
  superpone (pueden verse varias opciones a la vez). Con la tira ya centrada y
  sin navegar, `print()` **no repinta la banda** (bandera `_done` del
  `Scroller`, sección 14). **En `Menu` esta transición está activa:**
  `nextOption()` llama a `setTexto(OPTION[_selected], BOX_TOP, _lastSelected >
  _selected, TEXT_12x16)` + `startSlide()` al cambiar la selección, y mientras
  `Scroller::update()` devuelva `true` el resto de `Menu::update()` queda
  **congelado** (el vuelo manda); al terminar se reinicia `_timer` para el
  parpadeo.
- **Marcadores de posición:** dos filas sobre el pie. El **seleccionado** es un
  **rombo completo** (`Draw::diamond`)
  centrado en `DIAMOND_Y = VALUE_TOP + VALUE_HEIGHT/2` (= 49, filas 46..52),
  dibujado por `toggleDiamond()`;
  los **no seleccionados** son solo el **triángulo superior** con base en
  `TRIANGLE_Y = 53` y vértice en la 50 (`toggleTriangle()` → `Draw::triangle()`). Reparto uniforme:
  centro en `(i+1)·_space`, con `_space = WIDTH / (OPT_COUNT + (showContinue ?
  1 : 0))` y restando 1 a los índices `>= OPT_CONTINUE` si "Continue" está
  oculta. `nextOption()` intercambia los marcadores al navegar y `blinkOption()` hace
  parpadear **el rombo** (el repintado del texto quedó comentado) con el helper `Blink`
  (`_blink.changed(PERIOD, OFF)` marca el flanco y `_blink.isVisible(PERIOD, OFF)`
  da la fase visible; `PERIOD`/`OFF` de `Config::DefaultTimer`), **sin retardo
  inicial** (`HOLD` está comentado); `_blink` se ancla en `begin()` y al terminar
  cada vuelo del scroller. **Zona dinámica:** el parpadeo repinta en sitio (el texto
  con fondo blanco y los triángulos en blanco/negro), sin `clear()` de banda.
- Primera y última opción no conectadas (navegación con límites).

---

## 8. Clase `Boot` — API

Ubicación: `Boot.h` / `Boot.cpp`.

### Constructor

```cpp
Boot();
```

Sin parámetros: usa los servicios globales `Display` y `Buttons` (sección 20).

### Métodos

| Método | Descripción |
|--------|-------------|
| `void begin()` | Reinicia: ancla el parpadeo (`_blink.start()`), `_done` apagado y `_clear` puesto para el primer frame. El antiguo `_holdMessage` (fase fija del mensaje) está **comentado**, junto con su comprobación `expired(HOLD)`. |
| `void update()` | Cualquier botón la termina (`_done = true`) con sonido según el botón (prioridad: MOVE = `SFX_CLICK`, `ACTION_UP` = `SFX_BACK`, `ACTION_RIGHT` = `SFX_CONFIRM`, `ACTION_DOWN`/`ACTION_LEFT` = `SFX_CLICK`). El mensaje **parpadea desde el primer frame**, **sin fase fija inicial** (la constante `HOLD` y su comprobación están comentadas), con `PERIOD = 500 ms` y oculto el `OFF = 20 %` (`Config::DefaultTimer`); ya no actualiza nada en `update()` (la fase la lee `blinkMessage()` con `_blink.changed(PERIOD, OFF)`). **No tiene autoavance** (`TOTAL_MS`/`ANIM_TICK` comentados): espera a un botón. |
| `void print()` | `firstPrint()` (solo el primer frame: `clear()` + mensaje en el Header —`drawMessage(true)`— + logo en el Body) y `blinkMessage()` (repinta el mensaje cuando `_blink.changed(PERIOD, OFF)` con `drawMessage(_blink.isVisible(PERIOD, OFF))`: blanco visible / borrado en negro sobre negro). |
| `bool done()` | `true` cuando se pulsó cualquier botón (`Engine` pasa a `LEGEND`). |

### Pantalla de arranque (mensaje + logo)

- **Header (0..15):** el mensaje `MESSAGE` = `"Press any button"` / `"to start"`
  (2 líneas de `TEXT_6x8` centradas) que **parpadea** con el helper `Blink`
  (`_blink`, sección 24; `drawMessage(bool)` lo dibuja según el `bool` visible y
  `blinkMessage()` lo repinta cuando `_blink.changed(PERIOD, OFF)`). Sin fase fija: empieza a
  parpadear en cuanto entra.
- **Body (16..63):** fondo blanco (`fillRect(0, BODY_TOP, WIDTH, BODY_H, true)`)
  con el logo `Sprite::LOGO` (80×48) centrado, volcado con
  `Display::drawBitmap(..., true, false)` —`true`/`false` equivalen a
  `SSD1306_WHITE`/`SSD1306_BLACK`—: el logo guarda la
  **polaridad original** (1 = fondo, 0 = glifo), así que el glifo sale negro
  sobre el fondo blanco.
- La animación anterior de **bandas de líneas verticales** (`BAR_W`,
  `BAR_SPACING`, `ANIM_TICK`, `TOTAL_MS`, `drawBars()`, rebalse por el borde)
  está **comentada** en `Boot.h`/`Boot.cpp` (lo mismo los helpers
  `drawBar`/`drawBars`/`eraseOldBars`/`eraseBarDiff`).

---

## 9. Clase `Legend` — API

Ubicación: `Legend.h` / `Legend.cpp`.

### Constructor

```cpp
Legend();
```

Sin parámetros: usa los servicios globales `Display`, `Buttons` y `Sound`
(sección 20).

### Métodos

| Método | Descripción |
|--------|-------------|
| `void begin()` | Reinicia la ventana: `_btn`/`_lastBtn = 0` (rombo activo = `Btn1`), arranca `_ticker` y el parpadeo (`_blink.start()`), `_lastScroll`/`_done` apagados y `_clear` puesto (el primer frame hace `clear()` + estáticos). |
| `void update()` | Lee botones y avanza el ciclo. **El sonido depende del botón presionado** (`done() = true`): `MOVE_*` (navegación) = `SFX_CLICK`, `ACTION_UP` (Back) = `SFX_BACK`, `ACTION_RIGHT` (Select / Pause) = `SFX_CONFIRM`, `ACTION_DOWN`/`ACTION_LEFT` (None) = `SFX_CLICK`. El rombo activo avanza cada `NEXT = 2500 ms` (`_ticker`); **no hay fase fija**: el parpadeo se lee cada frame con `_blink.isVisible(PERIOD = 100 ms, OFF = 50 %)`, ~10 Hz (`HOLD` comentado). **Si el texto del pie sigue entrando** (`_scroller.update()` devuelve `true`), el ciclo se **congela** y, al terminar el vuelo, se reanclan `_ticker` y `_blink.start()`. |
| `void print()` | `firstPrint()` (solo el primer frame: `clear()` + banda blanca `3..11` con "Move"/"Action", pad MOVE, los 4 rombos de ACTION y la banda blanca del pie con `BTN_FUNC[0]`) + por frame `blinkDiamond()` (repinta el rombo activo cuando `_blink.changed(PERIOD, OFF)`) y `nextBtn()`/`_scroller.print()` (el texto del pie con vuelo del `Scroller`). |
| `bool done()` | `true` cuando se pidió ir al menú. `Engine` solo cambia de estado; **la Legend ya reprodujo su sonido** (Engine no toca `SFX_BACK` en esta transición). |

### Dibujo (leyenda de botones)

- **Título (Header):** banda blanca `TEXT_Y-1..TEXT_Y+8` con **"Move"** (mitad
  izquierda) y **"Action"** (mitad derecha) en `TEXT_6x8` negro sobre blanco;
  estático.
- **Pad MOVE (izquierda):** las 4 flechas son triángulos sólidos (`Draw::triangle`,
  lado `Config::Diamond::SIZE = 3`) en cruz alrededor de `(PAD_LEFT_X=32, PAD_Y=33)`
  con radio `PAD_RADIO = 10` (↑ en `PAD_Y-10`, → en `PAD_LEFT_X+10`, ↓ y ← análogos).
- **Pad ACTION (derecha):** **4 rombos completos** (`Draw::diamond`: dos triángulos,
  SIEMPRE rombo simétrico de `SIZE = 3`, nunca triángulos) en cruz alrededor de
  `(PAD_RIGHT_X=96, PAD_Y=33)` con radio `PAD_RADIO = 10`: ↑ (`Btn1`), →
  (`Btn2`), ↓ (`Btn3`), ← (`Btn4`).
- **Parpadeo (uno a la vez, MUY rápido):** el rombo activo recorre `Btn1 → Btn4`
  en **ciclo lento** (avanza cada `NEXT = 2500 ms` con el `_ticker`) y
  **parpadea sin espera**: `_blink.isVisible(PERIOD, OFF)` con `PERIOD = 100 ms` y
  oculto el `OFF = 50 %` de cada período (~10 Hz, `HOLD` comentado);
  `blinkDiamond()` repinta con `toggleDiamond(_btn, _blink.isVisible(PERIOD, OFF))` cuando
  `_blink.changed(PERIOD, OFF)` (y sale si `_lastScroll`). Los rombos inactivos se quedan fijos y
  completos; al cambiar se restaura el anterior completo (`toggleDiamond(_lastBtn,
  true)`), evitando que se quede borrado si lo pilló la fase oculta.
- **Texto del pie (centrado):** banda blanca `FOOT_TOP-1..63` con
  `BTN_FUNC[_btn]` en `TEXT_6x8` (negro sobre blanco) centrado en `FOOT_TOP = 56`:
  `Btn1` (↑ = `ACTION_UP`): **"Btn1 / Back"**, `Btn2` (→ = `ACTION_RIGHT`):
  **"Btn2 / Select / Pause"**, `Btn3` (↓ = `ACTION_DOWN`): **"Btn3 / None"**,
  `Btn4` (← = `ACTION_LEFT`): **"Btn4 / None"**. **Entra deslizándose** al
  cambiar de rombo (ver abajo). La línea separadora y `PIE_TOP = 57` del diseño
  anterior están **comentados** en `Legend.h`.
- **Texto del pie con `Scroller`:** la franja la lleva una instancia propia
  `_scroller` (8 px, `printY = FOOT_TOP`, `TEXT_6x8`), no un `drawText` directo:
  `nextBtn()` compone el texto nuevo y lo mete en vuelo con
  `setTexto(BTN_FUNC[_btn], FOOT_TOP, true, TEXT_6x8)` + `startSlide()`
  (**entra por la derecha**, siguiendo el avance `Btn1 → Btn4`). El texto nuevo
  **tapa** al anterior a medida que entra (el `Scroller` rellena la banda antes
  de volcar). Mientras la franja se mueve (`_lastScroll`), el ciclo de rombos
  **se congela** (`_scroller.update() && ...`): el texto no se queda a medio
  entrar cuando el rombo avanza, y al terminar el vuelo se reinician `_ticker` y
  `_blink.start()`.
- **Salida:** cualquier botón cierra la leyenda (`done()`) con su sonido según el
  botón. `Engine` solo la muestra al arranque (después del `Boot`); ya no se repite
  al volver al menú.
- **Renderizado (sin `clear()` por frame):** banda de título, pads, rombos y pie
  se dibujan una sola vez al entrar (`firstPrint()`, tras el `clear()` completo).
  Por frame solo se repintan **el rombo activo** (según `_blink.changed(PERIOD, OFF)`) y **el
  texto del pie** cuando cambia el rombo, a través del `Scroller`.

---

## 10. Clase `Buzzer` — API (capa de hardware)

Ubicación: `Buzzer.h` / `Buzzer.cpp`. Reproduce **un solo tono a la vez**, de forma
**no bloqueante**: `tone()` enciende el tono y marca su duración; `update()` (vía
`Sound::update()`) lo apaga al agotarse el tiempo. Sin `delay()`, el juego nunca se
congela. Usa LEDC del núcleo ESP32 (Core 3.x), como el `GameBuzzer` original.

**No es un servicio global:** es propiedad exclusiva de `Sound`, que lo contiene
como **miembro por valor** (`Buzzer _buzzer;`) y lo construye con el pin recibido.
Solo `Sound` lo usa: no hay `extern Buzzer` en `Globals.h` ni instancia global en
`Snake_II-OLED_128x64-I2C.ino`, y su `begin()` lo llama `Sound::begin()`. La API es pública dentro
de `Buzzer`, pero está encapsulada por la estructura de `Sound`.

### Constructor

```cpp
Buzzer(uint8_t pin = Config::Pin::BUZZER);   // pin 14 (Config)
```

### Métodos

| Método | Descripción |
|--------|-------------|
| `bool begin()` | `ledcAttach(pin, 2000, 10)` y silencia. Devuelve si se pudo adjuntar el canal. |
| `void update()` | Apaga el tono cuando termina su duración. Llamar una vez por `loop()` (ya lo hace `Sound::update()`). |
| `void tone(freq, durMs = 0)` | Emite un tono (no bloqueante). `durMs > 0` lo apaga solo; `0` = suena hasta `stop()`. `freq = 0` = silencio (espera activa). |
| `void stop()` | Silencia el buzzer y cancela la duración pendiente. |
| `bool busy()` | `true` mientras hay un tono/duración en curso. |
| `bool attached()` | `true` si `begin()` adjuntó el canal LEDC. |

---

## 10.2 Clase `Sound` — API (sonidos del juego)

Ubicación: `Sound.h` / `Sound.cpp`. Compone los efectos de Snake II como **secuencias
de tonos** (`Note` = `{freq, durMs}`, `0` = silencio) sobre su **`Buzzer` interno**
(miembro por valor, inicializado en `Sound::begin()`). Todo es no
bloqueante: `play()` arranca el efecto y `update()` lo avanza paso a paso cuando
cada nota termina. Con el sonido desactivado `play()` no hace nada (pensado para la
opción "Sound" del menú, `setEnabled(false)`).

Las 12 secuencias `SEQ_*` son **miembros estáticos privados** declarados en `Sound.h`
y definidos en `Sound.cpp` (los tonos), y se indexan con una **única tabla
`EFFECTS[]` de `{const Note* notes, uint8_t len}`** (orden igual al enum `Sfx`,
`SFX_NONE` incluido, con secuencia vacía): reemplaza a las 12 constantes `LEN_*` (el
largo de cada secuencia se deriva con `sizeof` dentro de la tabla) y al `switch` de
`play()`, que ahora solo lee `EFFECTS[effect]`. Son detalle de la implementación: la
API pública de `Sound` no expone `Note`, `Seq` ni las tablas.

### Constructor

```cpp
explicit Sound(uint8_t pin = Config::Pin::BUZZER);
```

El `Buzzer` se **construye por valor** dentro de `Sound` (miembro `_buzzer`, primer
miembro y primero en la lista de inicialización): ya no hay `Buzzer&` ni instancia
global.

### Enum y efectos

```cpp
enum Sfx : uint8_t {
  SFX_NONE = 0, SFX_CLICK, SFX_CONFIRM, SFX_BACK,
  SFX_EAT, SFX_START, SFX_LEVEL_UP, SFX_GAME_OVER,
  SFX_TICK, SFX_TURN, SFX_PAUSE, SFX_RESUME,
  SFX_NEW_BEST, SFX_FANFARE, SFX_COUNT   // SFX_COUNT = tamaño de EFFECTS (tabla indexada por Sfx)
};
```

| Efecto | Uso | Secuencia (frecuencias Hz / ms) |
|--------|-----|---------------------------------|
| `SFX_CLICK` | Navegar por el menú | 1800/35 |
| `SFX_CONFIRM` | Activar una opción | 700/50, 1000/80 |
| `SFX_BACK` | Volver al menú | 900/50, 600/90 |
| `SFX_EAT` | Comer el alimento | 988/60, 1319/100 |
| `SFX_START` | GO! al iniciar | 800/60, 1100/60, 1500/150 |
| `SFX_LEVEL_UP` | Subir de nivel | 523/60, 659/60, 784/60, 1047/120, 1319/180 |
| `SFX_GAME_OVER` | Muerte de la serpiente | 800/100, 650/100, 500/150, 300/300 |
| `SFX_TICK` | Conteo regresivo 3-2-1 (un pitido por dígito) | 900/40 |
| `SFX_TURN` | Cambio de dirección de la serpiente | 1319/20 |
| `SFX_PAUSE` | Pausar la partida | 600/50, 300/60 |
| `SFX_RESUME` | Reanudar la partida | 500/50, 900/60 |
| `SFX_NEW_BEST` | Festejo de nuevo récord (suena en el letrero "THE BEST") | 523/100, 659/100, 784/100, 1047/140, 784/100, 1047/140, 1319/420 |
| `SFX_FANFARE` | Fanfarria de arranque del programa (suena en Boot al encender) | 523/120, 659/120, 784/120, 1047/180, 784/120, 1047/180, 1319/360 |

Los tonos siguen la paleta del `GameBuzzer` original.

### Métodos

| Método | Descripción |
|--------|-------------|
| `void begin()` | Inicializa la capa de hardware (`_buzzer.begin()`, que adjunta el canal LEDC) y después `stop()` (silencia y reinicia la secuencia). Es el único lugar que inicializa el `Buzzer`. |
| `void setEnabled(bool)` / `enabled()` | Activa/desactiva el sonido (opción "Sound"); al desactivar corta el efecto en curso. |
| `void play(Sfx)` | Arranca la secuencia del efecto (no bloqueante). |
| `void update()` | Avanza a la siguiente nota (llama `_buzzer.update()` antes). Llamar una vez por `loop()`. |
| `void stop()` | Corta el efecto en curso y silencia. |
| `bool playing()` | `true` mientras suena un efecto. |

En `Snake_II-OLED_128x64-I2C.ino`: `Sound sound(Config::Pin::BUZZER);` (instancia única del
servicio global; el `Buzzer` que contiene se inicializa con `sound.begin()` en
`setup()` y `sound.update()` se llama en `loop()`).

---

## 10.3 Ventanas del menú: `MenuDifficulty` y `MenuSound`

Las opciones **"Difficulty"** y **"Sound"** del menú son ventanas del `Engine`,
igual que `MenuCredits` (históricamente fueron una clase `SoundWindow` y luego
edición *inline* dentro del propio `Menu`; ambas formas se eliminaron). El `Menu`
las devuelve como cualquier otra opción en `confirm()` y el `Engine` abre la
ventana que corresponda (`OPT_DIFFICULTY` → `MENU_DIFFICULTY`, `OPT_SOUND` →
`MENU_SOUND`). **Estado actual:** `MenuSound` ya está **fuera del aislamiento**
(el `Engine` la posee y despacha su estado, sección 11), pero `Menu::confirm()`
y la entrada desde el `Menu` siguen comentadas, así que la ventana **no se abre
todavía**; `MenuDifficulty` sigue aislada por completo.

Ambas comparten el mismo esqueleto de ventana (`begin`/`update`/`print`/`done`),
la misma banda de dibujo (**45..53**, la de los rombos del `Menu`) y la misma
regla de renderizado: **no borran la pantalla**, solo sustituyen esa franja. Al
salir, el `Engine` llama a `Menu::restoreDiamondBand()` y vuelve al menú con
`changeState(State::MENU, false)`, que evita el `clear()` completo de
`Menu::begin()`; el `Menu` repinta la banda y la línea del pie. (Hoy, al estar
`restoreDiamondBand()` aún comentada, el regreso de `MenuSound` hace el
`begin()` completo: `changeState(MENU, false)` cae en la llamada incondicional
a `_menu.begin()`.)

### `MenuDifficulty` — nivel 1..10

| Miembro | Descripción |
|---------|-------------|
| `MenuDifficulty()` | `_difficulty = Config::Difficulty::DEFAULT_LEVEL` (5) y el valor en edición igual. |
| `void begin()` | Copia `_difficulty` a `_edit` y ancla el parpadeo (`_blink.start()`). **No dibuja ni borra nada** (entra sobre el `Menu`). |
| `void update()` | `MOVE_RIGHT` +1 / `MOVE_LEFT` −1 con repetición al mantener (helper `ButtonRepeat`, `_repeat.step()`), tope en `Config::Difficulty::MIN_LEVEL`/`MAX_LEVEL`, cada paso con `SFX_CLICK`. `ACTION_RIGHT` **aplica** (`_difficulty = _edit`) y marca `done()`; `ACTION_UP` cancela. |
| `void print()` | Borra la banda 45..53 y llama a `drawSelector()` (cada frame: las flechas parpadean, el número no). |
| `bool done()` | `true` tras confirmar o cancelar. |
| `uint8_t difficulty()` | Nivel **confirmado**: lo lee el `Engine` en `changeState(NEW/CONTINUE)` para pasárselo a `Game::setDifficulty` (antes vivía en `Menu::difficulty()`). |
| `void drawSelector()` | Número 1..10 en `TEXT_6x8` centrado con **ancho constante** (`" 5"` mide lo mismo que `"10"`, 12 px) y **dos flechas** a los lados (`"< 5 >"`), **pegadas** al número (hueco `ARROW_GAP = 6` px, grosor `ARROW_W = 6`), que **parpadean juntas** (visible 75% / oculto 25% de `ARROW_BLINK_PERIOD = 500` ms) con la fase cruda de `Blink` (`_blink.isVisible(...)`). En el **límite** la flecha de ese lado se oculta. **Al mantener** `MOVE_LEFT`/`MOVE_RIGHT` el parpadeo se detiene: solo queda fija la flecha del botón activo y la contraria se oculta; al llegar al límite se dibuja como siempre (sin marcar la repetición). |
| ~~`bool holdRepeat(uint8_t)`~~ | **Eliminado**: la repetición se extrajo al helper `ButtonRepeat` (sección 23). |
| `_blink` | `Blink`: fase del parpadeo de las flechas (`isVisible()`). Antes era un `Stopwatch`. |
| `_repeat` | `ButtonRepeat`: repetición de `MOVE_LEFT`/`MOVE_RIGHT` al mantener (usa `Config::Button::DELAY`/`TICK`; se eliminaron las constantes locales `HOLD_REPEAT_DELAY`/`HOLD_REPEAT_TICK`). |

### `MenuSound` — On/Off

| Miembro | Descripción |
|---------|-------------|
| `MenuSound()` | `_enabled = true`, `done()` en `false`. |
| `void begin()` | `_enabled = sound.enabled()` (parte del valor real) y ancla el parpadeo (`_blink.start()`). **No dibuja ni borra nada.** |
| `void update()` | Solo actúa la tecla del lado de la flecha: `MOVE_LEFT` apaga si está ON y `MOVE_RIGHT` enciende si está OFF (cada uno con `SFX_CLICK`). `ACTION_RIGHT` **aplica** (`sound.setEnabled(_enabled)`, con `SFX_CONFIRM` solo si queda encendido) y marca `done()`; `ACTION_UP` cancela. |
| `void print()` | Borra la banda 45..53 y llama a `drawSelector()` (cada frame: la flecha parpadea, la palabra no). |
| `bool done()` | `true` tras confirmar o cancelar. |
| `void drawSelector()` | `"ON "` o `"OFF"` en `TEXT_6x8` centrado (ambos miden 18 px, así el centrado no se desplaza) y **una sola flecha** pegada a la palabra, en el **lado del destino**: `"OFF >"` (apunta a `MOVE_RIGHT`, que enciende) y `"< ON"` (apunta a `MOVE_LEFT`, que apaga). Parpadea visible 75% / oculto 25% de `ARROW_BLINK_PERIOD = 500` ms (fase cruda de `Blink`, `_blink.isVisible(...)`); la palabra no. |
| `_blink` | `Blink`: fase del parpadeo de la flecha (`isVisible()`). Antes era un `Stopwatch`. |

El valor del sonido **no es propio de la ventana**: el estado real vive en el
servicio global `Sound` (`setEnabled`/`enabled`), así que `MenuSound` solo lleva
el valor en edición y no necesita recordarlo entre entradas (a diferencia de
`MenuDifficulty`, que sí guarda el nivel confirmado).

Las constantes de geometría y parpadeo de ambas ventanas (`SEL_TOP = 45`,
`SEL_SIZE = 8`, `ARROW_*`) son `static constexpr` de cada clase,
siguiendo la convención de `Config.h` (allí solo va lo compartido de verdad); la
banda 45..53 es la misma que los rombos del `Menu`, y su erase incluye la fila
inferior de la banda, que por eso
`Menu::restoreDiamondBand()` vuelve a pintar (con la línea del pie
`Config::Screen::FOOT_LINE`, hoy **comentada** en `Config.h`). La repetición al
mantener ya no es constante de clase: `ButtonRepeat` usa `Config::Button`.

---

## 11. Clase `Engine` — despachador de ventanas

Separada del `.ino` en `Engine.h` / `Engine.cpp` (antes `App`). **No anida las
ventanas** pero las **posee como miembros**: `Boot`, `Legend`, `Menu`, `MenuCredits`,
`Game` son clases independientes (hermanas) declaradas como miembros `_boot`,
`_menu`, `_menuDifficulty`, `_menuSound`, `_menuCredits`, `_game`, `_legend`. No son
globales ni reciben las
ventanas por referencia. **Estado actual (aislado):** en `Engine.h` están activos `_boot`, `_legend`,
`_menu` y `_menuSound`; los `#include` y miembros `_game`, `_menuCredits` y
`_menuDifficulty` y los estados `NEW`, `CONTINUE`, `MENU_DIFFICULTY` y
`MENU_CREDITS` del `enum State` están **comentados** (bloques `AISLADO`).
`MENU_SOUND` está activo, pero la entrada desde el `Menu` sigue sin cablear
(`Menu::confirm()` comentado).

### Responsabilidad

`Engine` es el **despachador puro**: su **estado interno** (`enum class State`)
decide qué ventana corre y cuándo cambiar (`changeState()`, que llama al `begin()`
de la ventana entrante). `loop()` no participa en las transiciones: solo llama a
`engine.update()` y `engine.print()`. `setup()` inicia el hardware y llama a
`engine.begin()` (primera transición → `boot.begin()`).

### Constructor

```cpp
Engine();
```

Sin parámetros: los servicios (`Display`, `Buttons`, `Sound`) los consume como
globales (sección 20) y las ventanas son sus propios miembros. En `Snake_II-OLED_128x64-I2C.ino`
los globales se definen **antes** de `Engine engine;`, así los constructores de
los miembros (p. ej. `Scroller`/`Food` que consultan `display.getWidth()`) ven
los globales ya construidos (misma TU, orden de definición).

### Reglas de esta arquitectura

1. **Todas las clases se inician en `setup()`** y se usan en `loop()`/`Engine`.
   Los constructores son livianos (los de las ventanas ya no guardan
   referencias: usan los servicios globales); el trabajo real va
   en `begin()`/`update()`. Los `begin()` de las ventanas los llama `changeState()`
   al entrar (la primera se lanza dentro de `setup()` vía `engine.begin()`).
2. **Las ventanas NO se anidan entre sí, el `Engine` las posee:** cada ventana es
   una clase propia con el patrón `begin()/update()/print()/done()` (no código
   inline dentro de `Engine`), y `Engine` solo las despacha. Desde el refactor de
   servicios globales son **miembros del `Engine`** (no globales): ninguna otra
   clase puede invocarlas.
3. **El estado determina qué se ve** (solo `Engine` conoce `State`) **y los valores
   se conservan**: las ventanas son instancias persistentes (hermanas, no se
   recrean), así sus miembros sobreviven entre transiciones; el `begin()` solo
   reinicia lo que se requiere al entrar.
4. **La lectura de botones está centralizada:** el hardware se lee **una sola vez
   por frame** en `loop()` (`buttons.read()`, principio de responsabilidad única).
   Ninguna ventana llama a `read()` en su `update()`: todas consumen los eventos
   `pressed`/`released` de esa misma lectura, así es seguro que varias partes del
   sistema (ventana activa + HUD futuro, etc.) compartan el estado del mismo frame
   sin que ninguno "se coma" los eventos de un ciclo.
5. **Las ventanas "capa" no limpian la pantalla:** `MenuDifficulty` y `MenuSound`
   no son ventanas de página completa, sino capas sobre el `Menu` ya dibujado.
   Por eso su `begin()` no dibuja nada (su `print()` borra y repinta cada frame
   solo la banda de rombos) y su regreso previsto es
   `menu.restoreDiamondBand()` + `changeState(State::MENU, false)` —el `false`
   salta el `Menu::begin()`, que sí provocaría el `clear()` de página completa.
   **Estado actual:** `MenuSound` ya está **activa**, pero `restoreDiamondBand()`
   y la guarda `if (beginWindow)` siguen **comentadas**, así que hoy el regreso
   de `MenuSound` (`changeState(MENU, false)`) llama igual a `_menu.begin()` con
   `clear()` de página completa. `MenuDifficulty` sigue aislada.

### Estados internos

```cpp
enum class State : uint8_t {
  BOOT = 0,
  LEGEND,
  MENU,
  MENU_SOUND
  // NEW,            // AISLADO: comentados junto con sus ventanas
  // CONTINUE,
  // MENU_DIFFICULTY,
  // MENU_CREDITS
};
```

| Estado | Ventana | Notas |
|--------|---------|-------|
| `BOOT` | `Boot` | Pantalla de arranque: logo + mensaje que parpadea. Al terminar (`done()`, cualquier botón) pasa a `LEGEND`. |
| `LEGEND` | `Legend` | Panel de botones: pad MOVE con 4 flechas + 4 rombos completos de ACTION en las posiciones de un pad que parpadean MUY rápido uno a la vez (ciclo lento) con la función del rombo activo en el pie, que entra deslizándose al cambiar de rombo (`Scroller` propio) y congela el ciclo mientras se mueve. Cualquier botón la cierra → menú (suena el efecto según el botón —CLICK/BACK/CONFIRM—; `Engine` no añade `SFX_BACK`). Solo se muestra tras el arranque. |
| `MENU` | `Menu` | **Aislado:** `ACTION_RIGHT` no sale del menú (`_menu.confirm()` y el `switch` de destinos —`NEW`/`CONTINUE`/`MENU_DIFFICULTY`/`MENU_SOUND`/`MENU_CREDITS`— están comentados). Hoy `Menu` solo navega con MOVE (`SFX_CLICK`), `ACTION_RIGHT` alterna el estado `_confirm` (`SFX_CONFIRM`, la opción queda marcada/parpadeando en `blinkOption()`) y `ACTION_UP` devuelve la selección a `New`/`Continue` (`SFX_BACK`). |
| `NEW` | `Game` | **Comentado (aislado).** Previsto: nueva partida `setDifficulty(_menuDifficulty.difficulty())` + `begin(true)`. Arranca con el conteo regresivo 3-2-1 (un `SFX_TICK` por dígito). Al salir (`done()`) suena `SFX_BACK`, el `Engine` sincroniza el récord (`menu.setBestScore(game.bestScore())`), **oculta/muestra "Continue" al volver** (`menu.setContinueAvailable(resumable)`, donde `resumable = !game.isGameOver() && game.score() > 0`: partida en curso **y** con puntos), deja la selección del menú en `Continue` si `resumable`, o en `New` en caso contrario (`menu.setSelected(...)`) y pasa a `MENU`. |
| `CONTINUE` | `Game` | **Comentado (aislado).** Previsto: reanudar la partida anterior (`begin(false)`): queda en pausa y se retoma con `ACTION_RIGHT` (Btn2, "Select / Pause") o `ACTION_LEFT`; si no hay partida en curso arranca una nueva. Al salir (`done()`) igual que `NEW`. |
| `MENU_DIFFICULTY` | `MenuDifficulty` | **Comentado (aislado).** Previsto: selector de nivel 1..10 **sobre el `Menu` ya dibujado**: sustituye solo la banda de rombos (45..53) por `< N >`, sin `clear()`. `MOVE_LEFT`/`MOVE_RIGHT` editan con repetición (`SFX_CLICK` por paso), `ACTION_RIGHT` aplica y `ACTION_UP` cancela. Al salir (`done()`) el `Engine` llama `menu.restoreDiamondBand()` y hace `changeState(MENU, false)`: **no** vuelve a llamar `Menu::begin()` (que haría `clear()`), solo repinta esa banda. El nivel queda en `MenuDifficulty::_difficulty` (sección 10.3). |
| `MENU_SOUND` | `MenuSound` | **Activo (fuera del aislamiento)**; la entrada desde el `Menu` sigue sin cablear (`Menu::confirm()` comentado). Selector On/Off **sobre el `Menu` ya dibujado**, misma banda que `MENU_DIFFICULTY`. `MOVE_LEFT` apaga / `MOVE_RIGHT` enciende (`SFX_CLICK`), `ACTION_RIGHT` aplica con `sound.setEnabled()` (y `SFX_CONFIRM` solo si queda encendido), `ACTION_UP` cancela. Al salir (`done()`) vuelve al `Menu` con `changeState(MENU, false)`; hoy el regreso hace el `begin()` completo porque `restoreDiamondBand()` sigue comentada. El valor real vive en el servicio `Sound`, no en la ventana. |
| `MENU_CREDITS` | `MenuCredits` | **Comentado (aislado).** Previsto: 3 entradas navegables con `MOVE_LEFT`/`MOVE_RIGHT` y transición lateral (rol tamaño 2 **seleccionado con cuadro de borde a borde** y centrado en el alto restante del Body; nombre tamaño 1 plano en el pie). La transición usa el **mismo `Scroller` que el menú** pero con **2 instancias sincronizadas** (`_scrollerRol` 12x16 + `_scrollerNombre` 6x8): cada banda tiene su **propia tira**, que se compone **solo al entrar y al navegar** (`loadEntry()` → `Scroller::setTexto`, que además fija la fila donde se imprimirá: `roleY()` y `nameY()`). Cada `Scroller` **rellena su propia banda** antes de volcar la franja (fondo negro, texto blanco), así que la entrada anterior se mantiene hasta que la nueva la cubre (superposición al navegar rápido) y `MenuCredits::print()` ya no pinta el fondo ni llama a `redraw()` tras su `clear()`. El deslizamiento **arranca desde el borde** (fuera de escena: `setTexto(..., true, ...)` fija la dirección —entra por la derecha— y `startSlide()` lo lanza) y avanza **1 px cada 4 ms con acumulador por tiempo** (`update()`, ≈0,5 s). Al navegar suena `SFX_CLICK` y al salir (`done()`) suena `SFX_BACK` (lo toca el `Engine`) y pasa directo a `MENU`. |

### Métodos

| Método | Descripción |
|--------|-------------|
| `void begin()` | Primera transición: entra al test de píxeles (`changeState(State::BOOT)`). Se llama desde `setup()`. |
| `void update()` | Lee/actualiza la ventana activa y gestiona las transiciones de estado. Hoy recorren `BOOT → LEGEND → MENU` y despachan `MENU_SOUND` (solo se alcanza si se entra por código: la entrada desde el `Menu` sigue comentada; los demás casos —`NEW`, `CONTINUE`, `MENU_DIFFICULTY`, `MENU_CREDITS`— están comentados). Reproduce los efectos del sonido: `SFX_CONFIRM` al confirmar una opción del menú y `SFX_BACK` al volver a `MENU` desde cualquier ventana (excepto desde `Legend`, que toca su propio sonido según el botón, y desde `MENU_DIFFICULTY`/`MENU_SOUND`, que **no** los añaden: cada selector toca su propio `SFX_CONFIRM` al aplicar o `SFX_BACK` al cancelar) — **esa entrada sigue comentada** por el aislamiento. |
| `void print()` | Despacha el dibujo a la ventana activa. **Ya no limpia la
  pantalla (`display.clear()`)**: cada ventana la usa solo en su primer frame tras
  `begin()` y luego limpia/redibuja solo sus zonas dinámicas (sección 13). |
| `bool isInGame()` | `true` si hay una partida en curso (el **reposo** no aplica, sección 26). Hoy devuelve siempre `false` (**AISLADO**: sin `Game` activo); al reactivarlo debe devolver `_state == State::NEW \|\| _state == State::CONTINUE`. Queda indicado en el comentario del `.cpp`. |
| `void setBestScore(uint16_t)` | **Comentado** (aislado): reenviaría al menú para conservar el puntaje máximo entre sesiones. |
| `void changeState(State, bool beginWindow = true)` | Transición común: guarda el estado y llama `begin()` de la ventana destino. `MENU_SOUND` ya la usa (`MenuSound::begin()`); `NEW`/`CONTINUE`/`MENU_DIFFICULTY`/`MENU_CREDITS` siguen comentados. **Hoy `_menu.begin()` es incondicional** (la guarda `if (beginWindow)` está comentada): el parámetro `beginWindow = false` —usado al volver de `MENU_DIFFICULTY`/`MENU_SOUND` para repintar solo la banda de rombos con `menu.restoreDiamondBand()` en vez del `clear()` de `Menu::begin()`— está deshabilitado, así que el regreso de `MenuSound` hace el `clear()` completo. El resto del cableado de la transición (p. ej. `setDifficulty` al entrar en `Game`) también está comentado. No reproduce sonidos: cada ventana toca el suyo al confirmar o cancelar. |

### Patrón de ventana

Toda ventana implementa:

| Método | Descripción |
|--------|-------------|
| `begin()` | Restablece la ventana al entrar. `Engine` lo llama solo en `changeState()`. |
| `update()` | Maneja los eventos de botones (leídos una sola vez por `loop()` en `buttons.read()`; la ventana no llama a `read()`). |
| `print()` | Dibuja la ventana. Al entrar hace un `clear()` completo en el primer frame y luego solo limpia/redibuja sus zonas dinámicas. |
| `done()` | `true` cuando la ventana pide volver al menú. |

### Reglas del despachador

1. Solo `Engine` cambia de estado (nadie más conoce `State`).
2. Una ventana nunca cambia de estado ni conoce a las demás: expone `done()`.
3. `ACTION_UP` es el botón común "volver al menú" en todas las ventanas.
4. `ACTION_RIGHT` activa la opción del menú (su `confirm()`) — **hoy
   `Menu::confirm()` está comentado**: `ACTION_RIGHT` solo alterna el estado
   `_confirm` dentro del propio `Menu`.
5. `begin()` de cada ventana se llama desde `changeState()`, nunca desde `loop()`.

---

## 12. Cómo compilar/probar

- IDE: Arduino IDE, placa `ESP32-S3 (Dev Module)` (verificar puerto).
- Librerías: Adafruit GFX + Adafruit_SSD1306.
- **IntelliSense en VS Code** (solo está instalada la extensión Microsoft C/C++, sin
  extensión de Arduino ni PlatformIO): el `<Arduino.h>` marcado como error lo
  resuelven las rutas del core, y **dónde van depende de cómo se abra el
  proyecto**:
  - El proyecto se abre con el workspace **`D:\Documents\ESP32S3\ESP32S3.code-workspace`**
    (carpeta raíz `.`, es decir `D:\Documents\ESP32S3`, que contiene
    `Snake_II-OLED_128x64-I2C` y `blink`). En un `.code-workspace` **no** se lee el
    `c_cpp_properties.json` de
    una subcarpeta: la configuración va en el bloque `settings` del propio
    `.code-workspace` con claves `C_Cpp.default.*`
    (`compilerPath`, `includePath`, `defines`, `cppStandard`, `cStandard`,
    `intelliSenseMode`). Ese archivo está **fuera del repo**, así que no se versiona.
  - Si se abre la carpeta `Snake_II-OLED_128x64-I2C` sola (File > Open Folder), entonces
    sí manda `.vscode/c_cpp_properties.json` (una configuración con nombre,
    `ESP32-S3 Dev Module (arduino-esp32 3.3.12)`, equivalente a las
    `C_Cpp.default.*` del workspace). Es la copia que sí queda en el repo.
  - Contenido de las rutas:
  - `compilerPath` al toolchain real del core
    (`packages/esp32/tools/esp-x32/2601/bin/xtensa-esp-elf-g++.exe`); de ahí la
    extensión saca sola los includes del sistema (libstdc++, newlib, xtensa).
  - Rutas del core: `cores/esp32` (Arduino.h), `variants/esp32s3` (pins_arduino.h),
    `libraries/Wire/src` y `libraries/SPI/src` (los incluye `Adafruit_SSD1306.h`).
  - SDK de ESP-IDF: `tools/esp32s3-libs/3.3.12/qio_qspi/include` (el `sdkconfig.h`
    que define `CONFIG_XTAL_FREQ`, `CONFIG_MMU_PAGE_SIZE`...) y
    `tools/esp32s3-libs/3.3.12/include/**` recursivo, que es la forma de
    reproducir en VS Code los ~200 `-iwithprefixbefore` que pasa el core.
  - Librerías del sketchbook `D:\Documents\Arduino\libraries`: `Adafruit_GFX_Library`,
    `Adafruit_SSD1306` y `Adafruit_BusIO` (esta última la necesita
    `Adafruit_GFX.h`, no `Adafruit_SSD1306.h`).
  - `cppStandard: c++20` (el core compila con `-std=gnu++2a`) y los `-D` del
    recipe: `F_CPU`, `ARDUINO`, `ARDUINO_ARCH_ESP32`, `ARDUINO_ESP32S3_DEV`,
    `ARDUINO_BOARD`, `ARDUINO_VARIANT`, `ARDUINO_USB_CDC_ON_BOOT=0`, `ESP_PLATFORM`...
    **Sin `ESP32=ESP32` no arranca:** viene de `build.extra_flags` del
    `platform.txt` y es el que hace que `Adafruit_SPIDevice.h` tome su rama de
    ESP32; sin él cae en el `typedef BitOrder BusIOBitOrder` y el core 3.3.12 ya
    no define ese tipo (ahora usa `SPI_MSBFIRST`).
  - En el workspace, `${workspaceFolder}` es `D:\Documents\ESP32S3`, así que la
    raíz del proyecto va como `${workspaceFolder}/Snake_II-OLED_128x64-I2C`.
  - `Snake_II-OLED_128x64-I2C/.vscode/settings.json` deja el `.aider-venv` (y su
    caché) fuera de la indexación y de las búsquedas; sin eso el workspace se indexa
    entero.
  - Rutas absolutas: al actualizar el core (o el toolchain) hay que revisar los
    `3.3.12` y `2601`, y hay que tocarlas **en los dos sitios**.
- **Warnings de compilación C++ activados** en la máquina de desarrollo vía
  `platform.local.txt` del core ESP32 (`compiler.cpp.extra_flags=-Wall -Wreorder`)
  para que desajustes como el orden de inicialización de miembros salten a la vista
  en la compilación (no entra en el repo: se configura a nivel del paquete del core).
- La demo actual (`Snake_II-OLED_128x64-I2C.ino`) usa `Display`, `Buttons` y el sonido integrado:
  el menú arranca **sin la opción "Continue"** (no hay partida en curso) y solo
  aparece al volver del juego con la partida viva **y con puntos** (score > 0);
  al navegar el menú y los
  créditos suena `SFX_CLICK`, al confirmar `SFX_CONFIRM`,
  al volver al menú `SFX_BACK`, la `Legend` suena según el botón pulsado (MOVE =
  CLICK, ACTION_UP = BACK, ACTION_RIGHT = CONFIRM) y en la opción "Sound" el
  **On/Off se edita en su propia ventana** `MenuSound` (selector con flechas en
  la banda de los rombos; `MOVE_LEFT`/`MOVE_RIGHT` cambian el valor,
  `ACTION_RIGHT` lo aplica y `ACTION_UP` cancela). En la opción "Dificultad" el
  **nivel 1..10 se edita en `MenuDifficulty`** (selector `< N >` con dos flechas
  parpadeantes que se
  ocultan en los límites; `MOVE_RIGHT` +1, `MOVE_LEFT` -1 con repetición al
  mantener presionado —al mantener, el parpadeo se detiene y solo queda fija la
  flecha del botón activo, la contraria se oculta; al llegar al límite se
  procesa igual que haber soltado el botón (vuelve el parpadeo normal)—,
  `ACTION_RIGHT` lo aplica y
  `ACTION_UP` cancela). En el juego (`Game`): `SFX_TICK` en cada dígito del
  conteo 3-2-1, `SFX_START` (jingle GO!) al arrancar la partida después del "1",
  `SFX_TURN` al girar, `SFX_PAUSE`/`SFX_RESUME` al pausar/reanudar,
  `SFX_EAT` al comer, `SFX_GAME_OVER` al morir y `SFX_BACK` al volver al menú.
- Con SDA=8 y SCL=9, dirección 0x3C.

---

## 13. Esquema de renderizado y limpieza (sin `clear()` global)

El juego ya no limpia la pantalla completa en cada frame: `Engine::print()` **no**
llama a `display.clear()`, lo decide cada ventana.

### Reglas

1. **Clear completo solo al entrar:** cada ventana hace `display.clear()` en su
   **primer `print()`** después de su `begin()` (flag `_clear` puesto en
   `begin()` y apagado tras ese primer dibujo; el flag antiguo `_redraw` está
   comentado). Ese primer clear elimina la resaca
   de la ventana anterior y deja el fondo listo. Los métodos que lo reactivaban
   (`Menu::setOptions/setSelected/setContinueAvailable`, `setBestScore` si
   cambia) están **comentados**; hoy solo lo activa el `begin()`.
2. **Estáticos una sola vez:** títulos, pies, línea separadora, cuadro de
   selección, rótulos y pads se dibujan en ese primer frame y **ya no se vuelven a
   dibujar**; persisten en el buffer (que `display.show()` vuelca completo cada
   frame).
3. **Dinámicos por frame, borrando solo lo necesario:**

   | Ventana | Estáticos (una vez) | Dinámicos por frame |
   |---------|---------------------|---------------------|
   | `Boot` | Primer frame: clear completo + `firstPrint()` (banda blanca del mensaje en el Header + logo en el Body) | Solo el mensaje del Header cuando `_blink.changed(PERIOD, OFF)` (`blinkMessage()`: se repinta con `TEXT_6x8` blanco o se borra en negro); el logo no se toca |
     | `Menu` | Cuadro blanco `25..42`, texto de la opción en `26..41`, título y pie (banda blanca `55..63` con "Best" + versión) | Banda de la opción (`Scroller::print` con vuelo **activo** en `nextOption()`) + marcadores de posición (rombo del seleccionado en `46..52` y triángulos superiores en `50..53` de los no seleccionados) + parpadeo del rombo y el texto (`blinkOption()`: repinta el marcador y el texto según `_blink.changed(PERIOD, OFF)`/`_blink.isVisible(PERIOD, OFF)`, y reafirma visible si `_confirm` está puesto) |
    | `MenuDifficulty` | — (entra sobre el `Menu` ya dibujado, no borra nada) | Selector `< N >`: borra y redibuja **cada frame** la banda 44..54 (número centrado estático con ancho constante + dos flechas laterales que parpadean juntas, ocultas en su límite; al mantener un botón el parpadeo se detiene y solo queda fija la flecha del botón activo, ocultándose la contraria; al llegar al límite se procesa igual que haber soltado el botón, volviendo el parpadeo normal) |
    | `MenuSound` | — (entra sobre el `Menu` ya dibujado, no borra nada) | Selector ON/OFF: borra y redibuja **cada frame** la banda 44..54 (palabra centrada estática + flecha única en el lado del destino, que parpadea) |
| `MenuCredits` | Título del Header (el cuadro blanco del rol lo pinta ahora el propio `Scroller` al rellenar su banda) | Bandas rol/nombre (`Scroller`, 2 instancias sincronizadas; cada una rellena su banda y se vuelca juntas en el mismo frame, y solo si hay algo nuevo que pintar, sección 14) |
| `Legend` | Primer frame: clear + banda de título con "Move"/"Action", pad MOVE, los 4 rombos de ACTION y la banda del pie con `BTN_FUNC[0]` | Rombo activo (repintado cuando `_blink.changed(PERIOD, OFF)`, `blinkDiamond()`/`toggleDiamond`) + texto del pie (banda `FOOT_TOP-1..63`) por medio de su propio `Scroller` (`_scroller`): al cambiar de rombo **entra deslizándose** (`setTexto()` + `startSlide()`); mientras la franja se mueve, el ciclo de rombos se congela (`_scroller.update()` devuelve `true`) y al terminar se reanclan `_ticker` y `_blink.start()`. Al cambiar, se restaura completo el rombo que deja de ser activo (evita que quede borrado si el cambio lo pilló en su fase oculta) |
   | `Game` | Primer frame: clear completo + Header (puntaje 12x16 izq., segundos restantes de la comida especial 12x16 der.) y alimento y serpiente | Header solo si cambia el puntaje o `_food.specialTime()` (banda 0..15); tablero (Body 16..63) solo si `_dirtyBoard` (movimiento, comida nueva, transición de estado): borra el Body, redibuja alimento + serpiente; overlay "3-2-1"/"PAUSA"/"GAME OVER"/festejo de récord (texto invertido sobre banda blanca: cuadro centrado para el conteo, de lado a lado para PAUSA, GAME OVER y los letreros del festejo "BUT"/"YOU ARE"/"THE BEST") en cada frame según el estado —en el conteo, al final de cada dígito el número y su cuadro se ocultan (`COUNT_HIDE_MS`), marcando `_dirtyBoard` una sola vez para restaurar el tablero —; al morir superando el récord, el "GAME OVER" es un ciclo "GAME OVER" → "BUT" → "YOU ARE" → "THE BEST" (`NEW_BEST_SIGN_MS` cada uno) que se repite hasta que se presiona un botón, y el `SFX_NEW_BEST` suena solo la primera vez que aparece el letrero "THE BEST" |

4. Las ventanas `MenuDifficulty` y `MenuSound` **no hacen `clear()`**: son
   capas sobre el `Menu` ya dibujado. Comparten la banda de los rombos del menú
   (ver sección 7): desde su primer frame la borran y la redibujan **cada
   frame** con su selector (`< N >` o ON/OFF, con las flechas parpadeantes).
   Todo lo demás del `Menu` (título, cuadro de la opción, pie con "Best")
   queda intacto detrás. El regreso previsto es `menu.restoreDiamondBand()` +
   `changeState(State::MENU, false)` (marca `_diamondsDirty` y repinta solo esa
   banda sin `Menu::begin()`), pero **hoy está comentado** junto con esas
   ventanas (sección 11). Los flags `_redrawSound` y `_redrawDifficulty` ya no
   existen.

---

## 14. Clase `Scroller` — API (scroller de 1 bit)

Encapsula la animación "scroller de 1 bit": compone un texto en un **array de
`int8_t`** donde cada byte representa **1 columna de 8 píxeles** (bit 0 = fila 0,
bit 7 = fila 7) y lo desliza lateralmente. **Una sola banda** por instancia.
La franja es siempre **fondo negro y texto blanco** (sin parámetros de color).

- **Constantes:** `WIDTH = Config::Screen::WIDTH` (128, **ancho completo de
  pantalla**), `STRIP_H = Config::Scroller::MAX_H` (32, máx.
  altura de texto 18x24), `ANIM_TICK = 4` ms por píxel (vuelo ≈ 0,5 s).
- **Constructor:** `Scroller()` — sin parámetros. No reserva memoria dinámica
  (el array `_strip` es un miembro por valor), así que la copia está permitida
  aunque se mantiene bloqueada por coherencia con el resto del proyecto.
- **API:**
  - `void begin()` — **comentado** (no existe en la versión actual).
  - `void setTexto(const char* text, int16_t y, bool toLeft = true, uint8_t size = 2)`
    — compone el texto centrado en el array de `int8_t`
    (cada byte = 1 columna de 8 px), **fija la fila de pantalla donde se
    imprimirá** (`y`, se guarda tal cual: `_y = y`), **la dirección** (`toLeft`:
    `true` entra por la derecha) y **el alto de la franja**
    (`_height = 8 * size`), calcula el **límite de caracteres**
    (`floor(WIDTH / (6 * size))`) y **trunca silenciosamente** si el texto
    excede el límite. El texto se compone centrado en un canvas auxiliar
    (`GFXcanvas8(WIDTH, _height)`, cursor en la fila 0) y luego se extraen las
    columnas. Acaba poniendo
    `_done` (texto nuevo ya centrado, sin vuelo).
  - `void startSlide()` — **sin parámetros**: la dirección ya la fijó
    `setTexto()`. Arranca **fuera de pantalla**, baja `_done`, limpia el
    `_isStopNow` y arranca el reloj. Además borra en blanco la **fila de 1 px**
    justo encima (`_y - 1`) y debajo (`_y + _height`) de la franja (si quedan
    dentro del Body), para que no quede resto de la banda anterior al asentarse.
  - `bool update()` — avanza 1 px por `ANIM_TICK` ms (acumulador por tiempo); a
    llamar en `update()` de la ventana. Devuelve `true` mientras la animación
    sigue su curso (la franja aún entra en pantalla) y `false` en cuanto
    terminó. Con la animación terminada **no consume el reloj** (las
    llamadas siguientes devuelven `false` sin avanzar) hasta que
    `setTexto()`/`startSlide()` la reinicien.
  - `bool print()` — vuelca la franja a la pantalla en la fila `_y` fijada
    por `setTexto()`, **rellenando la banda con el fondo antes de pintar el
    texto**; a llamar en `print()` de la ventana. Devuelve `false` **sin
    repintar nada** cuando `_done` está puesta (la animación ya terminó y su
    frame final está en pantalla). Pone `_done` en el frame en que la tira
    llega a su sitio, garantizando que ese frame final se pinta.
- **Bandera `_done` (reposo):** sustituye al antiguo `_dirty`. `print()` es un
  no-op en cuanto la banda queda asentada —el caso normal—, así que no repinta
  la franja en reposo. La bajan `setTexto()` y `startSlide()`.
- **Relleno previo (no es un lujo):** mientras la franja entra desde el borde
  solo cubre parte del ancho; las columnas que aún no cubre conservarían lo que
  hubiera debajo (el texto anterior). `print()` recorre la pantalla columna a
  columna calculando su equivalente en la franja (`screenX - _slideX`) y pinta
  los 8 px de cada byte, con lo que el texto nuevo **tapa** al anterior a medida
  que entra.
- **Iteración actual:** la **cortina** está **activa**: `curtain()` (triángulos que
  tapan la franja por delante) se ancla al **frente** del movimiento (`front`, con
  un helper `timeCurtain()` que devuelve `6 * _size + 4 * _size - 1` px) y se dibuja
  al llegar cada frame. `startSlide()` arranca en `-timeCurtain()` (fuera de
  pantalla, del lado correcto según `toLeft`) y, mientras `_x < 0`, `print()` solo
  vuelca la cortina (el texto no entra hasta que la cortina ya se asomó): la cortina
  **arrastra al texto** y el recorrido total pasa a ser `WIDTH + timeCurtain` px.
- **Usos:**
  - `Menu` = 1 instancia (16 px, fila `BOX_TOP` = 26, fijada por
    `setTexto()`): **transición activa**. `nextOption()` compone la opción con
    `setTexto(OPTION[_selected], BOX_TOP, _lastSelected > _selected,
    TEXT_12x16)` y llama `startSlide()` al cambiar la selección; mientras
    `update()` devuelve `true` el resto de `Menu::update()` queda **congelado**
    y al terminar se reinicia `_timer` (parpadeo).
  - `MenuCredits` = 2 instancias sincronizadas (`_scrollerRol` 16 px +
    `_scrollerNombre` 8 px, ambas reciben `startSlide()` y `update()` en el
    mismo frame). Cada una rellena su propia banda, así que `print()` ya no
    necesita pintar el fondo ni llamar a `redraw()` tras su `clear()`.
  - `Legend` = 1 instancia para el **texto del pie** (`_scroller`, 8 px,
    `_printY = Config::Screen::FOOT_TOP`): al cambiar de rombo el texto entra
    deslizándose en lugar de aparecer de golpe.
- **Detalle de diseño:** `setTexto()` usa `display.getTextWidth()` para el
  centrado y `display.getWidth()` para el límite de caracteres. El array
  `_strip[STRIP_H / 8][WIDTH]` almacena las columnas: `_strip[row][col]` es
  un byte con los 8 píxeles de esa columna en ese grupo de filas. `Boot`
  **no** usa `Scroller`: su mensaje y su logo son estáticos con parpadeo
  propio (sección 8).

---

## 15. Namespace `Sprite` — API (sprites de la serpiente y comida especial)

Ubicación: `Sprite.h`. Tabla estática con los sprites de las partes de la
serpiente (estilo Nokia) y el sprite de la **comida especial**. Es un
**namespace de solo datos** (no una clase): no necesita instancia
ni archivo `.cpp`; los sprites se leen con `Sprite::pixel(part, x, y)` y
`Sprite::specialPixel(x, y)`. El **logo** del arranque (`LOGO`, sección 8) se
dibuja por bloques con `Display::drawBitmap()`.

**Empaquetado a 1 bit por píxel.** Ninguna tabla guarda un byte por píxel: cada
sprite se escribe **directamente como un literal binario** del tamaño exacto de sus
px (16 bits = un `uint16_t` para los 4×4, 8 bits = un `uint8_t` por fila para la
comida especial), así que en memoria solo queda la versión empaquetada y no hay
ningún paso de conversión ni tabla intermedia.

| Tabla | Antes | Ahora | Bytes |
|-------|-------|-------|-------|
| Sprites de la serpiente (4×4) | `uint8_t SPRITES[COUNT][4][4]` | `uint16_t SPRITES[COUNT]` (16 px = 16 bits) | 432 → **54** |
| Comida especial (8×4) | `uint8_t SPECIAL_FOOD[4][8]` | `uint8_t SPECIAL_FOOD[4]` (1 byte por fila: 8 px = 8 bits) | 32 → **4** |

Los literales binarios llevan separadores `'` para que las filas del dibujo se sigan
leyendo: en los 4×4, cuatro grupos de 4 bits (uno por fila, de arriba abajo); en la
comida especial, dos grupos de 4 por fila. El px `(0,0)` (esquina superior izquierda)
es el bit más significativo y el último px, el bit 0; el acceso a un píxel concreto lo
hacen las funciones `constexpr` `pixel()` / `specialPixel()` (el juego nunca indexa
los bits a mano).

```cpp
constexpr uint16_t SPRITES[COUNT] = {
  0b0110'0110'0100'0100,  // TAIL_TO_UP
  0b0000'0011'1111'0000,  // TAIL_TO_RIGHT
  ...
};

```cpp
constexpr uint8_t SIZE = 4;                 // sprite de 4×4 px
constexpr uint8_t BITS = SIZE * SIZE;       // px por sprite = bits de su uint16_t

enum Part : uint8_t {
  TAIL_TO_UP = 0, TAIL_TO_RIGHT, TAIL_TO_DOWN, TAIL_TO_LEFT,   // cola
  BODY_TO_UP,    BODY_TO_RIGHT,  BODY_TO_DOWN,   BODY_TO_LEFT, // cuerpo
  CORNER_RIGHT_UP, CORNER_RIGHT_DOWN, CORNER_LEFT_UP, CORNER_LEFT_DOWN, // curvas
  HEAD_UP_CLOSE, HEAD_RIGHT_CLOSE, HEAD_DOWN_CLOSE, HEAD_LEFT_CLOSE,     // cabeza: fauces cerradas
  HEAD_UP_OPEN,  HEAD_RIGHT_OPEN,  HEAD_DOWN_OPEN,  HEAD_LEFT_OPEN,      // cabeza: fauces abiertas
  BELLY_TO_RIGHT, BELLY_TO_LEFT,                  // panza recta (1 sprite por par de direcciones)
  BELLY_RIGHT_UP, BELLY_RIGHT_DOWN, BELLY_LEFT_UP, BELLY_LEFT_DOWN,      // panza curva
  EMPTY,
  COUNT
};
```

### Miembros

| Miembro | Contenido |
|---------|-----------|
| `SIZE` | Lado del sprite en píxeles (4). |
| `BITS` | Píxeles por sprite de la serpiente (16) = bits de su `uint16_t`. |
| `SPRITES[COUNT]` | Tabla de sprites **empaquetados** (1 bit por píxel) indexada por `Part`: cada sprite es un literal binario de 16 bits, en cuatro grupos de 4 (una fila por grupo). Rango: 0..3 cola, 4..7 cuerpo, 8..11 curvas, 12..15 cabeza cerrada, 16..19 cabeza abierta, 20..25 panza, 26 `EMPTY`, 27 `COUNT`. |
| `pixel(Part, x, y)` | `constexpr`: píxel del sprite (`x` = columna, `y` = fila) desde el `uint16_t` empaquetado. |
| `EMPTY` | Sprite vacío (todo fondo). |
| `COUNT` | Cantidad de sprites de la tabla. |
| `SPECIAL_FOOD_W` / `SPECIAL_FOOD_H` | Ancho (8) y alto (4) en píxeles del sprite de la comida especial. |
| `SPECIAL_FOOD[SPECIAL_FOOD_H]` | Sprite de 1 bit de la **comida especial** (8×4 px) **empaquetado por fila**: un literal binario de 8 px por fila (px `(0,0)` = bit 7) = 4 bytes en total, tabla aparte por no encajar en los 4×4 de la serpiente. |
| `specialPixel(x, y)` | `constexpr`: píxel de la comida especial (`x` = columna, `y` = fila). |
| `LOGO_W` / `LOGO_H` | Ancho (80) y alto (48) en píxeles del logo del arranque. |
| `LOGO[LOGO_H * (LOGO_W / 8)]` | Logo de 1 bit **empaquetado por fila** en el formato de bloques de `drawBitmap()` (1bpp, MSB primero: el px `(0,0)` de cada byte es su **bit 7**): 10 bytes por fila = **480 bytes** (frente a 3840 si cada px ocupase un byte). **Polaridad inversa al resto del archivo** (conserva la del original: **1 = fondo, 0 = glifo**), por eso `Boot` lo pinta con `Display::drawBitmap(..., true, false)` y **no tiene** función de lectura (`logoPixel` eliminado). |

La panza recta comparte sprite por par de direcciones: `BELLY_TO_RIGHT` =
`BELLY_TO_UP` y `BELLY_TO_LEFT` = `BELLY_TO_DOWN`.

`Sprite.h` (antes `SnakeSprites.h`) forma parte del respaldo del juego original
adaptado al estilo
del proyecto (pie `// Fin`, cabecera descriptiva, comentarios de los sprites
corregidos). Los consume `Snake` (la parte de cada segmento
–cola/cuerpo/curva/panza/cabeza– se deriva de la geometría de sus vecinos y de
la dirección de la cabeza, ver sección 18) y `Game` (los dibuja en el tablero,
ver sección 16); `Food` usa `SPECIAL_FOOD` (ver sección 17).

---

## 16. Clase `Game` — API (ventana del juego)

Ubicación: `Game.h` / `Game.cpp`. Ventana del juego de la serpiente
(estado `NEW`/`CONTINUE` del `Engine`), reemplaza al placeholder `InfoWindow`
(eliminado). **`Game` coordina**: la lógica de la serpiente (buffer circular,
giro pendiente, colisiones, elección de sprites) vive en la clase `Snake`
(sección 18), que **no toca `Display`/`Sound`/`Food`**. `Game` decide el ritmo
(dificultad), lee los botones y traduce MOVE a `Snake::Dir`, llama
`snake.step()` y maneja el resultado (`Result` MOVED/ATE/DIED) con puntaje,
sonidos y regeneración del alimento, y dibuja el tablero volcando los segmentos
que `Snake` expone (`length`/`segment`/`headPart`).

### Constructor

```cpp
Game();
```

Sin parámetros: consume los servicios globales (`Display`, `Buttons`, `Sound`,
sección 20). Los miembros `_food`, `Snake _snake;` y `Menu`-independientes se
construyen solos en su lista de inicialización.

### Constantes

| Constante | Valor | Significado |
|-----------|-------|-------------|
| `Config::Difficulty::MIN_LEVEL` / `MAX_LEVEL` / `DEFAULT_LEVEL` | 1 / 10 / 5 | Nivel de dificultad acotado (mismo rango y fuente única que el menú; ya no se duplica en `Game`). |
| `SPRITE_SCALE` | 2 | Px por lado de cada píxel del sprite al dibujarlo: los sprites de `Sprite::SIZE` (4×4) se dibujan como bloques 2×2 para llenar la celda (`Sprite::SIZE * SPRITE_SCALE = Config::Screen::CELL` = 8). Antes era el `2` literal de `drawSprite`. |
| `COUNTDOWN_MS` | 3000 | Duración del conteo regresivo inicial (3 s, un dígito por segundo: 3-2-1). |
| `COUNT_HIDE_MS` | 250 | Fase de parpadeo al final de cada dígito: el número (y su cuadro) se ocultan antes de que aparezca el siguiente. |

La geometría del tablero (`COLS`=16, `ROWS`=6, `MAX_LENGTH`=96, rejilla de 16×6
celdas en el Body de 128×48) vive en `Snake` (sección 18); `Game` la
usa vía `Snake::COLS`/`Snake::ROWS` (p. ej. para construir `Food`). **El tamaño de
la celda no se declara en `Game`**: se toma de `Config::Screen::CELL` (8 px), la
fuente única que también usa `Food` al dibujar el alimento; lo propio de `Game` es
solo el factor de escala del sprite (`SPRITE_SCALE`).

### Métodos

| Método | Descripción |
|--------|-------------|
| `void begin(bool newGame)` | `true` = nueva partida (reinicia todo y arranca el conteo regresivo 3-2-1). `false` = reanudar la partida anterior en pausa; si no hay partida en curso arranca una nueva. Conserva el récord (`_bestScore`) entre partidas. Llama `snake.clearPending()` (ningún giro pendiente al entrar). |
| `void setDifficulty(uint8_t level)` | Nivel 1..10 (clamp). **Se aplica EN CALIENTE, también con la partida iniciada**: recalcula `_moveDelay` al instante, por lo que una partida en curso (PLAY o PAUSE) sigue el nuevo ritmo al cambiar el nivel desde el menú; también vale para la próxima partida nueva (`reset()` la vuelve a derivar). |
| `void update()` | Estado `START`: pre-gira con MOVE (giro pendiente), entra a `PLAY` con `ACTION_RIGHT` o al agotarse `COUNTDOWN_MS` (al arrancar la partida suena `SFX_START`, el jingle GO! después del "1"). `PLAY`: gira con MOVE (sin reversa directa, queda un único giro pendiente que se aplica en el siguiente paso), avanza un paso cada `_moveDelay` ms y `ACTION_RIGHT` (Btn2, "Select / Pause") pausa. `PAUSE`: reanuda con `ACTION_RIGHT` (o `ACTION_LEFT`). `GAME_OVER`: cualquier ACTION vuelve al menú. `ACTION_UP` (Btn1, "Volver") sale en cualquier estado menos `GAME_OVER`. |
| `void print()` | Renderizado por zonas (ver sección 13). |
| `bool done()` | `true` al pedir volver al menú. |
| `bool isGameOver()` | `true` si al salir (`done()`) la partida terminó en `GAME OVER`; lo usa el `Engine` (junto con `score() > 0`) para dejar la selección del menú en `New` (Game Over o sin puntos) o `Continue` (partida en curso con puntos). |
| `uint16_t score()` / `bestScore()` | Puntaje actual / récord (el récord solo se actualiza al terminar en GAME OVER, ver "Comer"/"Colisión"). El `Engine` sincroniza `bestScore()` con el menú al salir. |
| `bool occupied(uint8_t x, uint8_t y)` | ¿Una celda está ocupada por la serpiente? Lo consulta `Food` (al colocar el alimento en una celda libre). **Delega en `Snake::occupied`.** |

### Reglas del juego (qué coordina Game)

- **Movimiento:** la cabeza avanza 1 celda por paso con **wrap en X y en Y**
  (sale por un borde, aparece por el opuesto, estilo Nokia). La velocidad
  (`_moveDelay` ms por paso) es lineal con la dificultad con **pasos alternados
  de 101/102 ms** (102 en los niveles 1, 4 y 7): `1000 - (nivel-1)*101 -
  (nivel+1)/3`. Los 9 saltos suman 912 ms, de **1000 ms en el nivel 1 a 88 ms
  en el nivel 10** (secuencia 1000, 898, 797, 696, 594, 493, 392, 290, 189, 88;
  101,33 ms por nivel no es entero, por eso se alternan 6 saltos de 101 y 3 de
  102). La fórmula vive en `Game::speedFor`.
- **Paso (`Game::step`):** llama `snake.step(_food.x(), _food.y())` y maneja el
  resultado de `Snake::Result`:
  - `MOVED` → solo repintar (`_dirtyBoard`).
  - `ATE` → **puntaje suma el nivel de dificultad actual** (`_score += _difficulty`,
    no +1 fijo; al poder cambiar la dificultad en caliente, vale la del momento de
    comer), suena `SFX_EAT`, regenera el alimento
    (`_food.spawn(Food::Type::NORMAL, *this)`; si no hay celdas libres → tablero
    lleno → `die()`, la partida **se gana**/termina).
  - `DIED` → `die()`: `GAME_OVER` (`SFX_GAME_OVER`), `_hasGame = false`.
- **Comer/crecer, colisión, giro, sprites:** los resuelve `Snake` (sección 18);
  `Game` solo consume el resultado y repinta.
- **Alimento:** rombo simétrico centrado en la celda (dos `fillTriangle`), como
  el rombo del menú; lo dibuja `Food` (sección 17). La **semilla del generador aleatorio** se
  fija en `Game::reset()` (lo consume `Food::spawn`) con `randomSeed(esp_random()
  ^ (uint32_t)nowMs())`: `esp_random()` es el RNG de hardware del ESP32 (entropía
  real, no predecible como `micros()`) y se combina con el reloj de 64 bits.
- **Colisión con el cuerpo:** al mover, la celda destino es ilegal si coincide
  con el cuerpo **salvo la celda de la cola cuando NO come** (la cola se libera
  ese paso, como en el Nokia original; la cola es bloqueante solo cuando come);
  la resuelve `Snake::step` (ver sección 18). Si colisiona: `GAME_OVER`
  (`SFX_GAME_OVER`), informa `SFX_BACK` al volver y `_hasGame = false` (un
  `Continue` posterior arranca de nuevo). **El récord (`_bestScore`) se
  verifica/actualiza solo aquí, en el GAME OVER** (o al terminar el tablero
  lleno, que también pasa por `die()`): partidas abandonadas con `ACTION_UP`
  no cuentan. Si el puntaje **supera el récord**, se activa el
**festejo de nuevo récord**: en vez del letrero estático, la secuencia
   "GAME OVER" → "BUT" → "YOU ARE" → "THE BEST" se repite en ciclo
   (`NEW_BEST_SIGN_MS` por letrero, `_newBest`/`_gameOverMs`/`_celeSfx`) hasta que
se presiona un botón, y la fanfarria (`SFX_NEW_BEST`) suena solo la primera vez
  que aparece el letrero "THE BEST" (YOU ARE ya no la dispara; el "GAME OVER" ya sonó en `die()` con `SFX_GAME_OVER`).
- **Pausa y salida:** `ACTION_RIGHT` (Btn2, "Select / Pause") durante `PLAY`
  pausa la partida (panel "PAUSA", suena `SFX_PAUSE`); en `PAUSE` retoma con el
  mismo botón o con `ACTION_LEFT` (suena `SFX_RESUME`). `ACTION_UP` (Btn1, "Volver")
  durante la partida vuelve al menú
  **sin perderla** (`_hasGame` mantiene el tablero; `Continue` la reanuda en
  pausa).
  `GAME_OVER` deja `_hasGame = false`. Al salir, el `Engine` deja la selección
  del menú en **`Continue`** si la partida siguió en curso **y con puntos** (sale
  con `ACTION_UP` tras haber comido) o en **`New`** si terminó en `GAME OVER` o la
  partida no tiene puntos (`resumable = !game.isGameOver() && game.score() > 0`;
  lo aplica con `menu.setSelected(...)` antes de pasar a `MENU`); además la
  opción **"Continue" en el menú se oculta cuando no hay partida que reanudar o
  cuando la partida no tiene puntos**
  (`menu.setContinueAvailable(resumable)`): al arrancar, tras un
  `GAME OVER` o al salir sin puntos la lista queda con 4 opciones (New, Dificultad, Sound, Créditos).
- **Header:** puntaje en `TEXT_12x16` (izq., NO se mueve) y segundos restantes
  de la **comida especial** en `TEXT_12x16` (der., `_food.specialTime()`, por
  ahora valor fijo 60 solo para el layout). Se redibuja solo cuando cambian.
  Ya no muestra el récord `HI` (el "Best: N" queda solo en el menú).
- **Overlays:** al iniciar el **conteo regresivo 3-2-1** (un dígito por segundo,
  `COUNTDOWN_MS/3` ms por dígito, texto centrado) y "PAUSA" / "GAME OVER" como
  **banda blanca de lado a lado** (todo el ancho del Body). `drawOverlay(title,
  fullWidth)` dibuja **cuadro centrado** alrededor del texto (`fullWidth = false`,
  el conteo) o **banda de borde a borde** (`fullWidth = true`, PAUSA y GAME OVER).
  La banda sobresale del texto **2 px por arriba y 0 px por abajo** y está
  **centrada como rectángulo a lo alto del Body** (16..63), no el texto: `y` se
  deriva del alto de la banda (`bandH = h + 2`), y el texto queda 2 px dentro;
  siempre `fillRoundRect` blanco + texto invertido negro `TEXT_12x16`
  (`drawTextInverted`) centrado en el rectángulo. **Festejo de nuevo récord:** si al
  morir se supera el "Best", el "GAME OVER" pasa a un **ciclo** de los cuatro letreros
  ("GAME OVER" → "BUT" → "YOU ARE" → "THE BEST", banda de borde a borde, `NEW_BEST_SIGN_MS`
  cada uno) que se repite hasta que se presiona un botón; el `SFX_NEW_BEST` (festejo)
  suena solo la **primera** vez que aparece el letrero "THE BEST" (YOU ARE ya no la dispara). **Parpadeo del conteo:** al
  final de cada dígito (los últimos `COUNT_HIDE_MS = 250` ms de su segundo) el
  número **y su cuadro desaparecen** antes de que aparezca el siguiente: el cambio
  es como un parpadeo. Al ocultarlo se marca `_dirtyBoard` una sola vez (restaura
  el tablero debajo del cuadro; flag `_overlayHidden`, reiniciado en `reset()`).
  **Pitido por dígito:** cada dígito suena `SFX_TICK` al aparecer (flag
  `_lastCount`, reiniciado en `reset()`; el jingle `SFX_START` ya no suena en
  `reset()`: se reserva para el arranque real de la partida). Al pasar de
  `START` a `PLAY` —por `ACTION_RIGHT` o al agotarse el conteo— suena
  `SFX_START` (GO!), justo después del "1", para iniciar la partida.
  Al volver a `PLAY` se marca
  `_dirtyBoard` (borra el overlay bajo el tablero).

### Validación en host (MinGW)

Antes de escribir el código se simuló en el PC la lógica núcleo (misma aritmética
de ring buffer, colisiones y selección de sprites): comer/crecer (cola se
mantiene, score, longitud), longitud estable sin comida (la cola avanza),
colisión real detectada (la cabeza no avanza), la cabeza **puede** ocupar la
celda de la cola (anillo casi cerrado), wrap horizontal y vertical, y cargo de
integridad de 400 pasos con giros (celdas únicas + adyacencia sin romper).
Ese núcleo ahora vive en `Snake` (sección 18), que por no depender de
`Display`/`Buttons`/`Sound`/`Food` se puede compilar y probar en el PC
directamente (sin stubs de esas clases).

---

## 17. Clase `Food` — API (alimento del tablero)

Ubicación: `Food.h` / `Food.cpp`. Encapsula el estado y la lógica del alimento
de la serpiente, extraídos de `Game` (antes `_food`/`_hasFood`/`_specialTime` y
los métodos `spawnFood()`/`drawFood()` vivían en `Game`). Es un componente del
juego (no una ventana): no tiene `begin()`/`update()` propios sino que es usado
por `Game` (miembro `_food`).

### Constructor

```cpp
Food(uint8_t cols, uint8_t rows, uint8_t top);
```

Recibe la geometría del tablero (dibuja con la `Display` global, sección 20):
rejilla de
`cols`×`rows` celdas de `Config::Screen::CELL` px a partir de la fila `top` (el
Body). `Game` la
construye así: `_food(Snake::COLS, Snake::ROWS, Config::Screen::BODY_TOP)` (las
constantes del tablero viven en `Snake`).

**El tamaño de la celda no se declara en `Food`:** el viejo `static constexpr
uint8_t CELL = 8` privado duplicaba `Config::Screen::CELL`; ahora ambas clases
(`Food` y `Game`) toman la celda de `Config` (sección 19), que es la fuente
única. `Food.cpp` incluye `Config.h`.

### Enum y constantes

```cpp
enum class Type : uint8_t { NORMAL = 0, SPECIAL };
static constexpr uint8_t SPECIAL_TIME_DEFAULT = 60;   // segundos iniciales de la especial (layout)
```

| Tipo | Significado |
|------|-------------|
| `NORMAL` | Comida común, dibujada como rombo simétrico centrado en la celda (como el rombo seleccionado del menú). |
| `SPECIAL` | Comida especial, dibujada con el sprite `Sprite::SPECIAL_FOOD` (8×4 px, centrado verticalmente en la celda) y con temporizador (`specialTime`, los segundos que el Header muestra a la derecha). Por ahora el temporizador es solo layout (valor fijo). |

### Métodos

| Método | Descripción |
|--------|-------------|
| `void begin()` | No hay alimento. |
| `bool spawn(Type, const Game&)` | Coloca un alimento del tipo dado en una **celda libre al azar**. La ocupación la responde el tablero (`Game::occupied`, que quedó público para esto): se cuentan las libres y se elige la `pick`-ésima (dos recorridos, sin buffer). Devuelve `false` si no hay celdas libres (tablero lleno → partida ganada), en cuyo caso queda sin alimento. |
| `void clear()` | Quita el alimento (no hay). |
| `bool has()` | `true` si hay alimento en el tablero. |
| `Type type()` | Tipo del alimento actual. |
| `uint8_t x()` / `y()` | Columna (0..`COLS`-1) / fila (0..`ROWS`-1). |
| `uint8_t specialTime()` | Segundos restantes de la comida especial. |
| `void setSpecialTime(uint8_t s)` | Actualiza el temporizador de la especial. |
| `void draw() const` | Dibuja según el tipo: `NORMAL` → rombo (`drawNormal`, dos `fillTriangle`); `SPECIAL` → sprite (`drawSpecial`, píxel a píxel del sprite de 1 bit). Si no hay alimento no dibuja nada. |

`Game` usa `_food` así: lo genera en `reset()` y al comer (`_food.spawn(Food::Type::NORMAL, *this)`; si devuelve `false` se muere), consulta `_food.has()/_food.x()/_food.y()` para detectar comida y boca abierta, dibuja `_food.draw()` al volcar el tablero y muestra `_food.specialTime()` en el Header. La construye en su lista de inicialización:
`_food(Snake::COLS, Snake::ROWS, Config::Screen::BODY_TOP)` (las constantes del tablero viven en `Snake`, sección 18).

---

## 18. Clase `Snake` — API (lógica pura de la serpiente)

Ubicación: `Snake.h` / `Snake.cpp`. Encapsula toda la lógica de la serpiente,
extraída de `Game` (refactor de la rama principal del proyecto; antes vivía en
las secciones Insecto/16 de este README). Es un componente del juego (no una
ventana): **no depende de `Display`, `Sound` ni `Food`** —solo de `Arduino.h`
(`delay`, `random`) y de `Sprite.h` (los enums de partes/conexiones, para elegir
los sprites que `Game` dibujará)—, de modo que se puede compilar y probar en el
PC sin el resto del proyecto.

### Qué hace y qué no hace

`Snake` mantiene el **buffer circular** de segmentos, la dirección commitida, el
giro pendiente (sin buffer ni reversa directa), el avance con wrap, la detección
de colisión, comer/crecer y la elección del sprite de cada parte. `Game` es quien
**coordina**: le pasa el alimento en cada paso, aplica puntaje/sonidos con el
resultado, regenera la comida y dibuja. `Game` también mantiene la **dificultad
(el ritmo)** y la **velocidad** aquí no intervienen: `Snake::step` se llama una
vez por paso y `Game` controla la cadencia.

### Constructor

```cpp
Snake();
```

Deja la serpiente en estado vacío (longitud 0, dirección `NONE`); hay que llamar
`reset()` antes de usarla.

### Constantes

| Constante | Valor | Significado |
|-----------|-------|-------------|
| `COLS`, `ROWS` | 16, 6 | Tablero: rejilla de 16×6 celdas de 8 px en el Body (128×48). Antes vivían en `Game`; `Game` y `Food` las usan vía `Snake::COLS`/`Snake::ROWS`. |
| `MAX_LENGTH` | 96 | Cantidad máxima de segmentos (una celda por segmento; 16×6 celdas caben, y nunca se repite celda viva). |
| `Dir::NONE` / `UP` / `RIGHT` / `DOWN` / `LEFT` | 0 / 1 / 2 / 3 / 4 | Direcciones. El orden (UP=1..LEFT=4) coincide con el orden de los sprites por dirección (`dir-1`). `NONE` = sin dirección (estado vacío). |
| `Result::MOVED` / `ATE` / `DIED` | — | Resultado de `step`: avanzó sin comer / comió (crece, la cola NO avanza ese paso) / colisión (muerte, sin moverse). |

### Métodos

| Método | Descripción |
|--------|-------------|
| `void reset()` | Inicializa la serpiente: células `(1,2)..(4,2)`, cabeza a la derecha, longitud 4, giro pendiente `NONE`. |
| `void clearPending()` | Descarta el giro pendiente (`_nextDir = NONE`). Lo llama `Game::begin()` al entrar a la ventana (START) para no reanudar con un giro acumulado del menú. |
| `bool turn(Dir d)` | Maniobra con **único giro pendiente, sin buffer ni reversa directa**. Se evalúa **siempre** desde la dirección actual de la cabeza (`_dir`, la COMMITIDA, la que usará en el próximo paso): desde ella solo hay 3 posibilidades —seguir, giro a la izquierda, giro a la derecha— y la contraria (180°) se **ignora**. Devuelve `true` si el giro quedó pendiente (el último válido pisa al anterior) para que `Game` toque `SFX_TURN` solo al aceptar. Así, al girar varias veces entre dos pasos la cabeza no puede volverse sobre la dirección con la que avanzará realmente y no se genera un GAME OVER espurio por una reversa falsa del último MOVE. |
| `Result step(uint8_t fx, uint8_t fy)` | **Avanza un paso** (aplica el giro pendiente si lo hay). Coordenadas `(fx, fy)` del alimento (las pasa `Game` desde `_food`). Devuelve `Result`; `Game` interpreta: `ATE` → crece (+1 segmento, la cola NO avanza ese paso porque come), `DIED` → no se mueve (colisión). |
| `bool occupied(uint8_t x, uint8_t y)` | `true` si la celda `(x,y)` está ocupada por algún segmento. Lo consulta `Food` (al colocar el alimento) a través de `Game::occupied`, que delega aquí. |
| `uint8_t length()` | Longitud actual (cantidad de segmentos vivos). |
| `const Seg& segment(uint8_t i)` | Segmento `i` (0 = cola, `length()-1` = cabeza) para que `Game` lo dibuje. |
| `Sprite::Part headPart(bool hasFood, uint8_t fx, uint8_t fy)` | Sprite de la cabeza según la dirección commitida: `HEAD_<dir>_OPEN` **una casilla antes** de llegar al alimento (la comida está en la próxima celda según `_dir`) y `HEAD_<dir>_CLOSE` al colisionar con él (o al no haber comida). Lo consulta `Game` cada frame. |

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
// montaje del sprite del cuerpo (boca arriba/abajo/izquierda/derecha), panza y cola
Sprite::Part bodyPartFor(Dir in, Dir out);
Sprite::Part bellyPartFor(Dir in, Dir out);
```

- **Ring buffer:** cola en `_tailIx`, cabeza en `_headIx`. Cada paso **agrega una
  parte nueva** (la cabeza) y **elimina la última** (la cola) salvo al comer (la
  cola NO avanza ese paso). El nuevo segmento se escribe en `slot()` = `_headIx+1`
  módulo `MAX_LENGTH`, y la cabeza nueva pasa a apuntar a ese slot.
- **El cuerpo NO se mueve:** la casilla que la cabeza deja se convierte en cuerpo
  con su `part` persistente (`bodyPartFor`/`bellyPartFor`), según por qué lado
  entra y sale la tubería: recta `BODY_TO_<dir>` (`in == out`) o esquina
  `CORNER_<horizontal>_<vertical>`/`BELLY_RIGHT_UP`.. al girar (`in != out`).
  La panza recta comparte sprite por par de direcciones: `BELLY_TO_RIGHT` =
  `BELLY_TO_UP` y `BELLY_TO_LEFT` = `BELLY_TO_DOWN`.
- **Comer (`bodyPartFor` en modo `EAT`):** con `in == out` se toma la panza recta
  de la dirección combinada `in`, y con giro (comer y girar a la vez) la panza
  curva `BELLY_<horizontal>_<vertical>`.
- **Colisión (`step`):** la celda destino es ilegal si coincide con el cuerpo
  **salvo la celda de la cola cuando NO come** (la cola se libera ese paso, como
  en el Nokia original; la cola es bloqueante solo cuando come). El giro pendiente
  se evalúa con una **copia local** (`dir`) y `_dir` (la COMMITIDA) solo se
  actualiza si el destino resulta legal, por lo que al morir la cabeza conserva la
  orientación real de su último movimiento (su sprite se dibuja según `_dir`).
- **Wrap:** `x = (x + 1) % COLS` etc.: sale por un borde, aparece por el opuesto.

### Validación en host

Por no depender de `Display`/`Buttons`/`Sound`/`Food`, `Snake` se puede compilar
y probar en el PC (MinGW) sin stubs: los casos del núcleo original (comer/crecer,
longitud estable sin comida, colisión real sin avanzar, cabeza sobre la celda de
la cola, wrap X/Y, cargo de integridad con giros) se conservan y pasan mirando a
esta clase directamente.

---

## 19. `Config.h` — constantes compartidas (namespace `Config`)

Ubicación: `Config.h`. **Un solo archivo pequeño para lo verdaderamente
compartido**: constantes que usan varias clases o que son propias del
hardware/placa. No es un cajón de sastre: lo específico de una clase se queda
como `static constexpr` dentro de ella (p. ej. `ANIM_TICK` en `Scroller`,
`NEW_BEST_SIGN_MS` en `Game`, `ARROW_BLINK_PERIOD` en `Menu`).

- **Sin `#define` para valores:** las constantes llevan tipo y viven en
  namespaces, de modo que no contaminan el ámbito global ni chocan con nombres
  de librerías (Adafruit / core ESP32). `#define` queda solo para lo que necesita
  el preprocesador (`#ifdef DEBUG`). Header: `#pragma once` + `#include <Arduino.h>`.
- **`constexpr` no ocupa RAM** y compila sin problemas en Arduino IDE con el core
  de ESP32 (C++17).

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

Estilo del archivo: los namespaces internos (`Pin`, `Screen`, `Difficulty`,
`Version`, `Credits`, `Legend`, `Diamond`, `Button`, `Scroller`,
`MenuStrip`, `DefaultTimer`) van en
columna 0, sin indentar respecto de `namespace Config {`.

### Qué contiene y quién lo usa

| Namespace | Constantes | Las usan |
|-----------|------------|----------|
| `Config::Pin` | `BUTTONS`, `BUZZER`, `OLED_SDA`, `OLED_SCL` | `Snake_II-OLED_128x64-I2C.ino` (pasa `Config::Pin::BUTTONS` a `Buttons` y `Config::Pin::BUZZER` a `Sound`, que construye su `Buzzer`), `Buzzer` (pin por defecto), `Display` (pines I2C por defecto) |
| `Config::Screen` | `WIDTH`/`HEIGHT`/`CELL`/`ADDRESS` y regiones `HEADER_*`/`BODY_*`/`FOOT_*` | `Display` (defaults del constructor y `regionBounds`), `Boot` (bandas TITULO=Header/CUERPO=Body), `Game` (tablero en el Body, celda de los sprites y `BODY_TOP` al volcar el tablero), `Food` (celda del alimento: centro del rombo y sprite especial), `MenuCredits` (rol del Body), `Legend` (línea y fila del pie: `FOOT_LINE`/`FOOT_TOP`/`FOOT_H`), `Scroller` (`WIDTH` como ancho de la tira) |
| `Config::Difficulty` | `MIN_LEVEL`/`MAX_LEVEL`/`DEFAULT_LEVEL` | `MenuDifficulty` (selector de dificultad) y `Game` (`setDifficulty`/velocidad): antes duplicadas en ambas clases. El sufijo `_LEVEL` y `DEFAULT_LEVEL` evitan la macro `DEFAULT` del core ESP32 (`Arduino.h`). |
| `Config::Version` | `VERSION`/`RELEASE_DATE` | `Menu` (valor por defecto del parámetro `version` de su constructor, que es el texto del pie; antes el literal `"v0.1"` estaba en la firma). `RELEASE_DATE` todavía no lo usa nadie: queda para la pantalla de créditos o el pie. `NAME` ("Snake II") lo dibuja `Boot` como título. |
| `Config::Button` | `DELAY` (400 ms) / `TICK` (100 ms) | Helper `ButtonRepeat` (secciones 23): mantiene un botón `DELAY` ms para empezar a repetir y luego un paso cada `TICK` ms; lo usan `Menu` y `MenuDifficulty` a través suyo. (`Menu` ya no copia `DELAY`/`TICK` a constantes propias.) **Nuevo** en este cambio. |
| `Config::Legend` | `NEXT` (2500 ms) / `PERIOD` (100 ms) / `OFF` (50 %); `HOLD` **comentado** | `Legend` (ciclo de rombos con `_ticker` y parpadeo con `_blink`, helper `Blink`): `HOLD` pasó de 900 ms (visible fija) a estar comentado, y `NEXT` subió de 2200 a 2500 ms. |
| `Config::Diamond` | `SIZE` (3 px) | `Menu` (rombo marcador y triángulos: `Menu::SIZE`) y `Legend` (rombos de ACTION: `Legend::SIZE`). |
| `Config::Scroller` | `MAX_H` (32) / `ANIMATION` (4 ms/px); `TOP` **comentado** | `Scroller` (`STRIP_H = MAX_H`, alto máximo de la franja; `ANIM_TICK = ANIMATION`, ms por píxel de desplazamiento). `TOP` (fila por defecto de la franja del menú) quedó **comentado**. |
| `Config::MenuStrip` | `BODY_H` (39, alto del Body sin el pie) / `BODY_MIDDLE` (34, mitad del Body) / `BOX_HEIGHT` (16) / `BOX_TOP` (= `BODY_MIDDLE - BOX_HEIGHT/2` = 26) / `VALUE_TOP` (44) / `VALUE_HEIGHT` (11) / `DIAMOND_Y` (= `VALUE_TOP + VALUE_HEIGHT/2` = 49) / `TRIANGLE_Y` (= `Screen::FOOT_TOP - 3` = 53) | `Menu` (cuadro de la opción y marcadores, sección 7) y `MenuSound` (banda del selector ON/OFF). Geometría **centrada en la mitad del Body** (`B`/`C` son auxiliares para `BODY_MIDDLE`). |
| `Config::DefaultTimer` | `PERIOD` (500 ms) / `OFF` (20 %); `HOLD` **comentado** | Parpadeo de "cualquier ventana" sin cronómetro propio: hoy `Boot` (mensaje) y `Menu` (rombo de la opción seleccionada). `HOLD` está comentado (sin fase fija inicial). |
| `Config::Power` | `IDLE_TIMEOUT_MS` (60000 ms) | Reposo (sección 25): tiempo sin actividad (sin botón presionado) fuera de partida antes de dormir en light sleep. Lo usa `Snake_II-OLED_128x64-I2C.ino` (cronómetro `idleTimer` en `loop()`). |

Las regiones `HEADER_TOP/H` y `BODY_TOP/H` reemplazan las constantes repetidas
`BODY_TOP`/`BODY_H`/`TITLE_TOP`/`TITLE_H` de `Menu`, `Game`, `Boot` y `MenuCredits`.
Los `static constexpr` de esas clases se eliminaron; `Boot` tenía además sus
constantes propias de la animación de bandas (`BAR_W`, `BAR_SPACING`,
`ANIM_TICK`, `TOTAL_MS`), hoy **comentadas** al estar esa animación desactivada.

La banda del pie tiene su geometría en `Config::Screen`: `FOOT_LINE` (línea
separadora, `HEIGHT - (FOOT_H + 1 + 1) = 54`) está **comentado** y hoy no lo
dibuja nadie; `FOOT_TOP` = 56 con `FOOT_H` = 8 para el texto, todo derivado de
`HEIGHT` igual que `HEADER_TOP/H` y `BODY_TOP/H`. Antes eran `FOOT_TOP` =
`HEIGHT - 9` y `FOOT_H` = 7. Hoy los usan **`Legend`** (banda del pie y fila
donde su `Scroller` imprime el texto) y **`Menu`** (su pie: banda blanca
`55..63` con "Best" y la versión).

`CELL` sigue la misma regla: es la **única** medida de px por celda. Reemplaza al
`static constexpr uint8_t CELL = 8` que `Food` declaraba por su cuenta y a los `8`
literales de `Game::drawSprite`. Lo que **no** es compartido (el factor de escala
con que un sprite de 4×4 llena la celda) queda como constante propia con nombre en
la clase que la usa: `Game::SPRITE_SCALE = 2` (`Sprite::SIZE * SPRITE_SCALE ==
Config::Screen::CELL`).

---

## 20. `Globals.h` — globales (servicios y estado)

Ubicación: `Globals.h`. Declara con `extern` los **globales** que comparten
todas las clases nuestras: los servicios de hardware (`Display`, `Buttons`,
`Sound`) y el almacén de estado compartido (`Storage`, sección 22). Desde este
refactor las clases ya no reciben
los servicios por constructor (ver secciones 3, 6 y 11): el único lugar donde se
**instancian** es `Snake_II-OLED_128x64-I2C.ino`.

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

1. **Se instancian solo en `Snake_II-OLED_128x64-I2C.ino`**, y cada servicio se inicializa en su
   `begin()` desde `setup()` (`Storage` no tiene `begin()`: bastan sus valores
   por defecto del constructor) (por eso el **orden de definición ya no importa**:
   el `Engine`, declarado al final, se construye sobre globales que solo
   necesitan su constructor —`Scroller` (`display.getWidth()`) y `Food` son
   seguros— y el trabajo real va en los `begin()`). **No** se usa
   `static order/fiasco` ni factories.
2. **Son globales los servicios de hardware y `Storage`.** Las **ventanas**
   (`Boot`, `Legend`, `Menu`, `MenuCredits`, `Game`) NO: son miembros del
   `Engine` (sección 11) y por eso no aparecen aquí. El **`Buzzer` tampoco**: es
   una pieza de hardware interna de `Sound` (miembro por valor), no un servicio
   global. `Storage` sí es global porque es el almacén de estado compartido
   (sección 22): ni ventana ni hardware.
3. **`Globals.h` se incluye solo desde los `.cpp`** (las cabeceras no lo
   incluyen): evita acoplar los `.h` al global y mantiene el orden de includes
   predecible. Un `.cpp` que usa un servicio global o `storage` debe incluir
   `Globals.h`. `Sound.h` sí incluye `Buzzer.h`, porque su miembro es un
   `Buzzer` por valor.

---

## 21. `Timer.h` — reloj de 64 bits y cronómetros

Ubicación: `Timer.h` / `Timer.cpp`. Antes `Timer.h` era header-only; ahora las
definiciones están en el `.cpp` (el patrón `.h`/`.cpp` del resto del proyecto).
Motivo: `Stopwatch` y `Ticker` los usan casi todas las ventanas y, como métodos
`inline` dentro de la clase, el compilador repetía el mismo código en cada
unidad de traducción. **Nada se declara `inline` en el header**: una función
`inline` definida en otro `.cpp` no genera símbolo y daría *undefined
reference*.

Ante los límites de `millis()`
(32 bits: da la vuelta cada ~49,7 días, y `elapsed % period` sobre un instante
absoluto salta de fase una vez por giro), se centraliza el tiempo en el **reloj de
64 bits del ESP32**: `esp_timer_get_time()` (microsegundos desde el arranque, no
envuelve en ~292.000 años). Con 64 bits las restas (`ahora - inicio`) y los
módulos son seguros sin pensar en el desbordamiento.

**`esp_timer.h` vive solo en el `.cpp`** (único consumidor: `nowMs()`).
`<Arduino.h>` **se mantiene en el header** a propósito: además de los tipos
enteros, otras unidades de traducción lo heredan de ahí y no lo incluyen
(`Buttons.cpp` usa `pinMode`/`digitalRead`, `Game.cpp` usa `Serial`). Quitarlo
exigiría añadirlo explícitamente en esos `.cpp`: es deuda conocida, no un descuido.

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
  uint32_t consume();                  // pasos completos desde la última consulta
};                                     // (conserva el residuo)
```

### Reglas de uso

1. **Medir con restas, nunca contra un instante absoluto:** `nowMs() - inicio`,
   que con enteros sin signo de 64 bits da bien aunque el reloj se mueva.
2. **`Stopwatch` para plazos** (una fecha en la que algo vence): `COUNTDOWN_MS`/
   `_moveDelay`/`NEW_BEST_SIGN_MS` en `Game`, `_durationMs` en `Buzzer`, y el
   interior de los helpers `ButtonRepeat`/`Blink` (secciones 23 y 24), que lo
   encapsulan para `Menu`, `MenuDifficulty`, `MenuSound`, `Boot` y `Legend`. El
   parpadeo de `Boot`/`Menu`/`Legend` ya no usa `expired(HOLD)`: la fase fija está
   comentada. `start()` se llama en el `begin()` de la ventana o al fijar la
   zona que parpadea: así el tiempo inactivo no entra.
3. **`Ticker` para pasos periódicos:** `ANIM_TICK` en `Scroller` y `NEXT` en
   `Legend` (avance del ciclo de rombos; `consume()` devuelve los pasos de una
   vez; el `while` del acumulador desapareció). En `Boot` y `Menu` el `Ticker`
   está **comentado** (sus animaciones cambiaron).
4. **`blinkOn(period, offPct)`** reemplaza al `millis() % período` absoluto de
   los parpadeos. Ya no lo llaman las ventanas directamente: lo encapsula el
helper `Blink` (sección 24), que las ventanas usan igual que antes (`Boot`:
    mensaje con `PERIOD=500`/`OFF=20` de `Config::DefaultTimer`; `Menu`: rombo
    de la opción seleccionada con las mismas; `MenuDifficulty`/`MenuSound`: flechas de sus
   selectores; `Legend`: parpadeo del rombo activo con `PERIOD=100`/`OFF=50`). El
   `Blink` se ancla al entrar en la ventana (`begin()`) y se reancla
   (`start()`) al terminar el vuelo del `Scroller`, así la fase del parpadeo no
   salta nunca.

### Quién lo usa

| Clase | Reloj/cronómetro | Cambio |
|-------|------------------|--------|
| `Boot` | `Blink _blink` (parpadeo del mensaje) | El mensaje parpadea con `Blink` (`changed(PERIOD, OFF)` / `on(PERIOD, OFF)`), anclado al `begin()` (`PERIOD`/`OFF` de `Config::DefaultTimer`; `HOLD` comentado). El antiguo `Stopwatch _timer` quedó encapsulado en el helper. El antiguo `Ticker _ticker` + plazo `TOTAL_MS` de las bandas están **comentados**. |
| `Menu` | `Blink _blink` (parpadeo de la opción) + `ButtonRepeat _repeat` (repetición por mantención) | El parpadeo pasa al helper `Blink` (sección 24), anclado a `begin()` y al final de cada vuelo del scroller; la navegación con mantención usa el helper `ButtonRepeat` (sección 23, `expired(DELAY)`/`expired(TICK)` internamente). El `Ticker` de la animación vieja está comentado. |
| `MenuDifficulty` | `Blink _blink` (flechas), `ButtonRepeat _repeat` (repetición por mantención) | Extraído de `Menu` con el selector: `Blink` (`on()`) y `ButtonRepeat`; se eliminaron las constantes locales `HOLD_REPEAT_DELAY`/`HOLD_REPEAT_TICK` y el viejo `holdRepeat`. |
| `MenuSound` | `Blink _blink` (flecha) | Extraído de `Menu` con el selector: `Blink` (`on()`). |
| `Legend` | `Ticker _ticker` (`NEXT`) + `Blink _blink` (`PERIOD`/`OFF`) | El ciclo de rombos avanza con `ticker.consume()` cada `NEXT = 2500 ms` y el parpadeo usa el helper `Blink`, anclado al `begin()` y reanclado (`start()`) al terminar el vuelo del scroller. `HOLD` (fase fija) y los viejos `DWELL_MS`/`HOLD_MS` ya no existen. |
| `Scroller` | `Ticker _timer` (`ANIM_TICK`) | El acumulador `_colAcc`/`_animLast` pasa a `consume()` (misma cadencia, sin `while`). |
| `Game` | `nowMs()` en `_moveLast`/`_startMs`/`_gameOverMs` (uint64_t) | Plazos y módulos del conteo/festejo sobre el reloj de 64 bits (mismo comportamiento; sin techo de 49,7 días). |
| `Buzzer` | `_startMs` (uint64_t) + `nowMs()` | El fin de la duración se compara con resta de 64 bits. |
| `Buttons` | `_buttonLast` (uint64_t) + `nowMs()` | El debounce se mide sobre el reloj de 64 bits. |

`Sound` no tiene reloj propio: delega las duraciones en `Buzzer`. `Snake` sigue
siendo lógica pura (solo `Arduino.h`: `delay`/`random`) y no usa el reloj.

---

## 22. Clase `Storage` — API (estado compartido)

Ubicación: `Storage.h` / `Storage.cpp`. Almacena la información que **no debe
vivir en ninguna ventana** y que varias partes del sistema necesitan leer y
escribir: hoy el **mejor puntaje**, el **sonido activo** y la **dificultad**.
Cualquier otro valor compartido que surja (p. ej. si hay partida en curso) se
añade aquí.

Es un **contenedor de datos puro**: no depende de `Display`/`Buttons`/`Sound`,
no tiene `begin()`/`update()`/`print()` ni relojes; solo getters y setters. Sus
valores por defecto salen del constructor (de `Config` donde aplica).

### Instancia global

Lo global es la **instancia** —no métodos ni propiedades estáticas—: igual que
los servicios, `extern Storage storage;` vive en `Globals.h` (sección 20) y
`Storage storage;` se define en `Snake_II-OLED_128x64-I2C.ino`. Cualquier
clase la usa directamente desde su `.cpp` (`storage.bestScore()`) incluyendo
`Globals.h`.

### Constructor

```cpp
Storage();
```

Sin parámetros. Valores por defecto: `_bestScore = 0`, `_soundEnabled = true` y
 `_difficulty = Config::Difficulty::DEFAULT_LEVEL`.

### Métodos

| Método | Descripción |
|--------|-------------|
| `uint16_t bestScore() const` | Mejor puntaje (récord). |
| `void setBestScore(uint16_t)` | Fija el récord tal cual (no compara: decidir si algo es récord es cosa del llamador). |
| `bool soundEnabled() const` | ¿Sonido activo? |
| `void setSoundEnabled(bool)` | Activa/desactiva el sonido. |
| `uint8_t difficulty() const` | Nivel de dificultad (1..10). |
| `void setDifficulty(uint8_t)` | Fija el nivel con **clamp** a `Config::Difficulty::MIN_LEVEL`/`MAX_LEVEL`. |

### Estado actual (pendiente de integrar)

La clase y su instancia global **existen pero todavía no las consume nadie**:
`Menu` sigue guardando su propio `_bestScore` (sección 7), `MenuDifficulty` su
`_difficulty` (sección 10.3) y `Sound` su `_enabled` (sección 10.2). Mover esas
copias a `storage` queda como tarea pendiente (decisión del usuario: en esta
tarea solo se creó la clase).

---

## 23. `ButtonRepeat.h` — repetición al mantener un botón

Ubicación: `ButtonRepeat.h` / `ButtonRepeat.cpp`. Helper **por composición** (no
hereda de nadie ni nadie hereda de él): encapsula el patrón "paso inmediato al
pulsar y, si se mantiene, un paso cada `tick` ms tras `delay` ms". Sustituye los
dos `Stopwatch` (`_repeat`/`_repeatTick`) que `Menu` y `MenuDifficulty` llevaban
sueltos, que además habían **divergido**: `Menu` repetía con `buttons.hold()`
mientras que `MenuDifficulty` usaba el viejo `buttons.state()`. Aquí se unifica
en `hold()` (el método vigente).

### API

```cpp
class ButtonRepeat {
public:
  explicit ButtonRepeat(uint32_t delay = Config::Button::DELAY,
                        uint32_t tick  = Config::Button::TICK);

  bool step(uint8_t button);   // ¿toca aplicar un paso en este frame?
};
```

- `step(button)`: devuelve `true` si el botón pasó a presión en este frame
  (`pressed`, reinicia los dos cronómetros) o si sigue mantenido (`hold`) y ya
  vencieron `delay` y `tick` (entonces reinicia solo el cronómetro de cadencia).
  Devuelve `false` en cualquier otro caso.

### Decisiones

- **Sin `reset()`**: no hay hoy ningún call site que necesite reiniciar la
  repetición desde fuera, así que no se añade (YAGNI). Si aparece, se agrega.
- **Tiempos por defecto de `Config::Button`** (`DELAY = 400`, `TICK = 100`): así
  las ventanas no repiten constantes propias.
- Usa `buttons.hold()` (no `state()`), que es el método vigente de `Buttons`.

### Quién lo usa

- `Menu` (`_repeat`): en `navigate()` para `MOVE_LEFT`/`MOVE_RIGHT` (sección 7).
- `MenuDifficulty` (`_repeat`): en `update()` para `MOVE_RIGHT`/`MOVE_LEFT`
  (sección 10.3). Se eliminaron sus constantes `HOLD_REPEAT_DELAY`/
  `HOLD_REPEAT_TICK`.

---

## 24. `Blink.h` — parpadeo con ancla y detección de flanco

Ubicación: `Blink.h` / `Blink.cpp`. Helper **por composición** que encapsula un
`Stopwatch` (ancla del período) + el recuerdo de la última fase (`_last`, para
detectar el flanco). Así el parpadeo deja de repetirse (y de divergir) entre
`Boot`, `Legend`, `Menu`, `MenuDifficulty` y `MenuSound`, y las ventanas ya no
llevan su propio flag de giro.

### API

```cpp
class Blink {
public:
  void start();                                       // ancla el período y olvida la fase (_last = false)
  bool isVisible(uint32_t period, uint8_t offPct) const;  // fase actual (true = visible)
  bool changed(uint32_t period, uint8_t offPct);      // true solo en el flanco
};
```

- `start()`: `_timer.start()` + `_last = false` (única forma de anclar; se usa en
  `begin()` y al reanclar tras el vuelo del `Scroller`).
- `isVisible(period, offPct)`: fase cruda del período (`Stopwatch::blinkOn`); visible salvo
  el primer `offPct%`. No modifica nada (const): para quien repinta cada frame
  (`MenuDifficulty`, `MenuSound`).
- `changed(period, offPct)`: `now = isVisible(period, offPct)`; si `now == _last` devuelve
  `false`; si no, guarda `_last = now` y devuelve `true`. Quien redibuja solo en el
  flanco (`Boot`, `Legend`, `Menu`) consulta esta.

### Cómo lo interpreta cada ventana

- **`Boot`** (`_blink`): `changed(PERIOD, OFF)` decide si repintar y `isVisible(PERIOD, OFF)`
  es "mensaje visible". `blinkMessage()` → `if (!_blink.changed(PERIOD, OFF)) return;
  drawMessage(_blink.isVisible(PERIOD, OFF))`; `firstPrint()` dibuja con `drawMessage(true)`.
- **`Legend`** (`_blink`): `blinkDiamond()` sale si `_lastScroll` o si
  `!changed(PERIOD, OFF)`, y repinta con `toggleDiamond(_btn, _blink.isVisible(PERIOD, OFF))`.
  Al terminar el vuelo del texto del pie se reancla con `start()`.
- **`Menu`** (`_blink`): `blinkOption()` con `_confirm` sale del menú; si no, `if
  (!changed(PERIOD, OFF)) return; const bool visible = isVisible(PERIOD, OFF);
  toggleText(visible); toggleDiamond(_selected, visible);`. Se
  ancla en `begin()` (`start()`) y se reancla al terminar el vuelo (`start()`).
- **`MenuDifficulty`** y **`MenuSound`** (`_blink`): solo usan la **fase cruda**
  `isVisible(period, offPct)` para sus flechas.

### Decisiones

- `isVisible()` devuelve la fase **visible** (`true` = visible); antes el flag de la ventana
  se interpretaba invertido en `Boot`, que ahora pasa la fase directamente a
  `drawMessage(bool)`.
- `changed()` lleva internamente `_last`, así que el redibujo en el flanco no necesita
  que la ventana guarde ningún flag de giro.
- `start()` (la única forma de anclar) reancla tras el vuelo del `Scroller` y olvida
  la fase (`_last = false`).

## 25. `Draw.h` — primitivas de dibujo (namespace)

Ubicación: `Draw.h` / `Draw.cpp`. Namespace con las dos formas que `Menu` y
`Legend` tenían **duplicadas** (los marcadores de posición): un triángulo
direccional y el rombo completo (dos triángulos superpuestos). No tiene estado
(no es una clase): usa el global `display` y el lado `Config::Diamond::SIZE`.

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

- `triangle(dir, cx, cy, show)`: `display.fillTriangle` con el vértice según `dir`,
  que es un `Draw::Direction` (`DIR_UP = 1`, `DIR_RIGHT = 2`, `DIR_DOWN = 3`, `DIR_LEFT = 4`), y
  medio lado `Config::Diamond::SIZE` alrededor de `(cx, cy)`; `show` elige blanco
  (`true`) o negro (`false`).
- `diamond(cx, cy, show)`: un triángulo hacia arriba + uno hacia abajo (mismo
  centro), es decir `triangle(DIR_UP, …)` + `triangle(DIR_DOWN, …)`.

### Quién lo usa

- **`Menu`**: `toggleDiamond()` → `Draw::diamond(centerX, DIAMOND_Y, show)` y
  `toggleTriangle()` → `Draw::triangle(Draw::DIR_UP, centerX, TRIANGLE_Y, show)`.
  (Reintroducidos en esta iteración en lugar de `focused`/`unfocused`.) La
  constante `SIZE` se eliminó.
- **`Legend`**: las flechas del pad MOVE con `Draw::triangle(Draw::DIR_UP/DIR_RIGHT/DIR_DOWN/DIR_LEFT, …)`
  y el rombo activo con `Draw::diamond(…)` desde `Legend::toggleDiamond`. Se eliminaron
  `Legend::toggleTriangle` y la constante `SIZE`.

### Decisiones

- **Sin estado → función libre**, no método de instancia: por eso es un namespace
  (no encaja como método de `Menu`/`Legend`, que solo lo reusaban). El único
  estado que toca (`display`, la constante `SIZE`) es global/compartido.
- El nombre es a nivel de **intención** (`triangle`/`diamond`), no de primitiva GFX
  (`fillTriangle`), para que las ventanas no repitan las coordenadas relativas.

## 26. Reposo (light sleep)

Ubicación: `Snake_II-OLED_128x64-I2C.ino` (`enterSleep()`, `idleTimer` y el
despertar en `setup()`), con `Config::Power::IDLE_TIMEOUT_MS` (60 s) y los
métodos `Display::power(on)` y `Engine::isInGame()`. Es una **función libre**
del wiring, no una ventana: depende de los globales (`display`, `buttons`,
`sound`) y del estado del `Engine`.

### Qué hace

- **Disparador:** en `loop()`, `idleTimer` (un `Stopwatch`) cuenta el tiempo sin
  botones presionados. Cualquier `buttons.anyHeld()` lo reinicia (`start()`).
  Si **no se está en partida** (`!engine.isInGame()`) y llegó a
  `Config::Power::IDLE_TIMEOUT_MS`, se entra en reposo.
- **Reposo:** `enterSleep()` apaga el OLED (`display.power(false)`, comando SSD1306
  `0xAE`; el framebuffer del panel se conserva) y corta el sonido
  (`sound.stop()`); luego bloquea en `esp_light_sleep_start()` hasta que un
  botón despierta.
- **Despertar:** en `setup()`, cada pin de `Config::Pin::BUTTONS` se habilita como
  fuente de wake con `gpio_wakeup_enable(pin, GPIO_INTR_HIGH_LEVEL)`
  (los botones son `INPUT_PULLDOWN`, pulsado = HIGH) y `esp_sleep_enable_gpio_wakeup()`.
  Al volver: `display.power(true)` (`0xAF`, misma imagen) y `buttons.begin()`
  para **re-anclar el estado** y que el botón que despertó no se lea como un
  "press" (no navega el menú al despertar).
- **Tras el wake** `idleTimer.start()` reinstala el contador para no volver a
  dormir al instante y `loop()` **retorna** sin procesar el resto del frame (así
  el frame del despertar no ejecuta `engine.update()`/`print()`/`display.show()`).

### Por qué light sleep (no deep sleep)

- **Conserva la RAM:** las ventanas y `Storage` (récord, sonido, dificultad)
  siguen vivas; `loop()` continúa donde quedó, sin reinicializar nada.
- **Cualquier GPIO despierta:** el wake por matriz GPIO (`esp_sleep_enable_gpio_wakeup`)
  admite todos los pines, incluido `GPIO47` (ACTION_LEFT). El deep sleep solo
  despierta por pines RTC (`EXT0`/`EXT1`), y además perdería la RAM.

### API nueva que aporta

- `Display::power(bool on)` — apaga/enciende el panel OLED (ahorro en reposo).
- `Buttons::anyHeld()` — true si algún botón está presionado (actividad del contador).
- `Engine::isInGame()` — true si hay partida en curso (el reposo no aplica).
  Hoy devuelve siempre `false` (**AISLADO**: sin `Game` activo); al reactivarlo,
  volver a `_state == State::NEW || _state == State::CONTINUE`.
- `Config::Power::IDLE_TIMEOUT_MS` — timeout configurable (60 s por defecto).

### Notas

- `esp_timer` (base de `nowMs()`/`Stopwatch`) sigue avanzando durante el light
  sleep: el RTC no se detiene, así que el contador no se corrompe.
- El OLED conserva su RAM con el comando `SSD1306_DISPLAYOFF` (0xAE): al volver
  con `SSD1306_DISPLAYON` (0xAF) se restaura la misma imagen sin repintar. La
  librería instalada (Adafruit_SSD1306, esta versión) no tiene los helpers
  `displayOn()`/`displayOff()`, así que `Display::power()` usa
  `ssd1306_command()`.



