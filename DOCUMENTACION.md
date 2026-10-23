# DOCUMENTACIÓN TÉCNICA — Snake II (ESP32-S3, OLED 128x64 I2C)

Este documento describe **cómo está construido y cómo se modifica** el proyecto
`Snake_II-OLED_128x64-I2C`. Está escrito a partir del código fuente; si algo aquí
no coincide con el código, **manda el código**.

---

## Índice

1. [Descripción general](#1-descripción-general)
2. [Estructura del proyecto](#2-estructura-del-proyecto)
3. [Arquitectura](#3-arquitectura)
4. [Clases](#4-clases)
5. [Funciones relevantes](#5-funciones-relevantes)
6. [Configuración](#6-configuración)
7. [Sistema de entrada](#7-sistema-de-entrada)
8. [Sistema de sonido](#8-sistema-de-sonido)
9. [Sistema gráfico](#9-sistema-gráfico)
10. [Temporizadores](#10-temporizadores)
11. [Flujo de una partida](#11-flujo-de-una-partida)
12. [Cómo modificar el juego](#12-cómo-modificar-el-juego)
13. [Cómo agregar nuevas funcionalidades](#13-cómo-agregar-nuevas-funcionalidades)
14. [Dependencias](#14-dependencias)
15. [Convenciones del proyecto](#15-convenciones-del-proyecto)
16. [Problemas conocidos](#16-problemas-conocidos)

---

## 1. Descripción general

- **Qué es:** recreación del juego **Snake II** (estilo Nokia) sobre hardware
  propio. `-- >B:` es una serpiente que avanza por una rejilla, come, crece, sube
  de dificultad al llenar el tablero y termina al chocar consigo misma.
- **Objetivo:** reproducir esa experiencia con controles físicos, sonido, menú
  (Nueva partida / Continuar / Dificultad / Sonido / Créditos) y sin depender de
  Internet. Todo el código es original (ver `THIRD_PARTY_NOTICES.md`).
- **Plataforma:** **ESP32-S3** (Arduino IDE, core *esp32* de Espressif **3.3.12**),
  OLED **SSD1306** monocroma **128x64** por **I2C**.
- **Arquitectura general:** un `.ino` mínimo (*raíz de composición*) crea los
  servicios globales y un **`Engine`** que es una **máquina de estados** dueña de
  las **ventanas** (pantallas). Cada ventana es una clase independiente con la
  misma interfaz (`begin/update/print/firstPrint/done`).
- **Cómo se relacionan los componentes:**
  - El `Engine` **posee** todas las ventanas; las ventanas **no** se conocen
    entre sí y **no** cambian de estado: solo exponen `done()`.
  - Todos hablan a través de los **globales** (`display`, `buttons`, `sound`,
    `storage`, definidos en `Globals.h` e instanciados en el `.ino`).
  - `Game` coordina `Snake` (cuerpo/colisiones/dibujo) y dos `Food` (normal y
    especial). `Snake` y `Food` no conocen al `Engine`.
  - `SleepManager` es un subsistema aparte (reposo por inactividad) que consulta
    al `Engine` si puede dormir.
- **Regla de oro del diseño:** una ventana nunca limpia toda la pantalla cada
  frame; cada una decide qué borrar/repintar (§3.5).

---

## 2. Estructura del proyecto

El sketch está dividido en subcarpetas por responsabilidad. En la **raíz** solo
queda el .ino (raíz de composición) y la documentación; **todo el código** se
agrupa bajo src/: los archivos compartidos (Config.h, Globals.h) y las
carpetas core/, devices/, graphics/ y windows/. Una **clase por
.h/.cpp** (cuando tiene .cpp).
> **¿Por qué `src/`?** El IDE de Arduino solo compila recursivamente los `.cpp`
> que están dentro de la subcarpeta `src/` del sketch. Los `.h` se pueden incluir
> desde cualquier carpeta, pero los `.cpp` fuera de `src/` se ignoran en el
> enlazado. Por eso todo el código vive bajo `src/`.

```text
Snake_II-OLED_128x64-I2C/
├── Snake_II-OLED_128x64-I2C.ino   raíz de composición
├── README.md                      presentación del proyecto
├── DOCUMENTACION.md               este documento
├── LICENSE.md                     licencia
├── THIRD_PARTY_NOTICES.md         atribuciones de terceros
└── src/
    ├── Config.h                   constantes compartidas
    ├── Globals.h                  servicios globales
    ├── core/                      núcleo de la aplicación
    ├── devices/                   hardware y servicios base
    ├── graphics/                  dibujo y animación
    └── windows/                   pantallas y juego
```

### Raíz

| Archivo | Responsabilidad |
|---------|-----------------|
| `Snake_II-OLED_128x64-I2C.ino` | *Raíz de composición*: instancia los globales y `Engine`/`SleepManager`; `setup()` y `loop()`. |

### `src/` — archivos compartidos

| Archivo | Responsabilidad |
|---------|-----------------|
| `src/Config.h` | Namespace `Config`: **todas** las constantes compartidas (pines, pantalla, tablero, dificultad, versión, geometría de menús). Único lugar donde se ajustan valores. |
| `src/Globals.h` | Declara (`extern`) los globales `display`, `buttons`, `sound`, `storage`. Solo lo incluyen los `.cpp`. |

### `src/core/` — núcleo

| Archivo | Responsabilidad |
|---------|-----------------|
| `src/core/Engine.h/.cpp` | Máquina de estados: posee las ventanas y gestiona las transiciones. |
| `src/core/SleepManager.h/.cpp` | Reposo (*light sleep*) por inactividad; apaga/enciende el OLED y habilita el *wake* por GPIO. |

### `src/devices/` — hardware y servicios base

| Archivo | Responsabilidad |
|---------|-----------------|
| `src/devices/Display.h/.cpp` | Capa sobre `Adafruit_SSD1306`: bus I2C y primitivas de dibujo. |
| `src/devices/Buttons.h/.cpp` | 8 botones con antirrebote; estados por flanco/continuo. |
| `src/devices/Buzzer.h/.cpp` | Capa de hardware PWM (`ledc`): un tono no bloqueante. **No es global**: lo posee `Sound`. |
| `src/devices/Sound.h/.cpp` | Reproductor de efectos (secuencias `{freq, durMs}`), con bucle y on/off. |
| `src/devices/Storage.h/.cpp` | Estado compartido en RAM: récord, sonido on/off, dificultad y escala del tablero. |
| `src/devices/Timer.h/.cpp` | `nowMs()` (reloj de 64 bits), `Stopwatch`, `Ticker`. |
| `src/devices/Blink.h/.cpp` | Parpadeo con ancla y detección de flanco de visibilidad. |
| `src/devices/ButtonRepeat.h/.cpp` | Repetición al mantener un botón. |

### `src/graphics/` — dibujo y animación

| Archivo | Responsabilidad |
|---------|-----------------|
| `src/graphics/Draw.h/.cpp` | Namespace `Draw`: `triangle` y `diamond`. |
| `src/graphics/Scroller.h/.cpp` | Desplazamiento de texto con "cortina" (horizontal/vertical). |
| `src/graphics/Sprite.h` | Tablas de bits (namespaces `Sprite`, `Special`, `Logo`) y `pixel()`/`specialPixel()`. |
| `src/graphics/Playfield.h/.cpp` | Namespace `Playfield`: geometría viva del área jugable (celda, columnas, filas y origen) según la escala activa. |

### `src/windows/` — pantallas y juego

| Archivo | Responsabilidad |
|---------|-----------------|
| `src/windows/Boot.h/.cpp` | Arranque: logo + mensaje "Press any button / to start" parpadeando. |
| `src/windows/Legend.h/.cpp` | Ayuda: leyenda de botones (pad MOVE + 4 rombos ACTION). |
| `src/windows/Menu.h/.cpp` | Menú principal (New / Continue / Difficulty / Sound / Scale / Credits). |
| `src/windows/MenuDifficulty.h/.cpp` | Selector de dificultad (nivel 1..10); capa sobre `Menu`. |
| `src/windows/MenuSound.h/.cpp` | Selector de sonido ON/OFF; capa sobre `Menu`. |
| `src/windows/MenuScale.h/.cpp` | Selector de escala del tablero (4 / 8 / 12 px por celda); capa sobre `Menu`. |
| `src/windows/MenuCredits.h/.cpp` | Roll vertical de créditos / GAME OVER / YOU WIN. |

#### `src/windows/game/` — la partida

| Archivo | Responsabilidad |
|---------|-----------------|
| `src/windows/game/Game.h/.cpp` | La partida: estados, movimiento, puntuación, comida y overlays. |
| `src/windows/game/Snake.h/.cpp` | Cuerpo de la serpiente, colisiones, giro y dibujo propio. |
| `src/windows/game/Food.h/.cpp` | Comida normal y especial: aparición, temporizador y dibujo. |

> Los `.md` (`README.md`, `DOCUMENTACION.md`, `LICENSE.md`,
> `THIRD_PARTY_NOTICES.md`) son documentación, no código.

> **Inclusión de archivos:** cada `#include "..."` se resuelve con el nombre pelado
> dentro de su propia carpeta, o con la ruta relativa a la raíz al cruzar carpetas
> (p. ej. `#include "src/windows/game/Game.h"`). Los archivos compartidos se incluyen por
> su ruta desde la raíz (`#include "src/Config.h"`, `#include "src/Globals.h"`).

---

## 3. Arquitectura

### 3.1 Flujo general

```
setup()  →  loop()  { buttons.read() → SleepManager.update() →
                       Engine.update() → Engine.print() →
                       sound.update() → display.show() }
```

No hay hilos ni interrupciones de usuario: todo el trabajo ocurre en `loop()`,
un paso por iteración (**no bloqueante**, salvo el *light sleep*).

### 3.2 Inicialización (`setup()`)

El `.ino` define los globales **antes** de `Engine`:

```cpp
Display display;
Buttons buttons(Buttons::PINS, 30);   // 30 ms de antirrebote
Sound sound;
Storage storage;
Engine engine;
SleepManager sleepManager(engine);
```

```cpp
void setup() {
  Serial.begin(115200);
  display.begin();        // Wire + panel; si falla, bucle infinito
  buttons.begin();
  sound.begin();
  engine.begin();         // primera transición → Boot
  sleepManager.begin();   // habilita el wake por GPIO
}
```

### 3.3 `loop()`

```cpp
void loop() {
  buttons.read();                             // única lectura física de botones

  if (!sleepManager.update()) {               // ¿dormido? corta el frame
    sound.update();
    return;
  }

  engine.update();      // lógica del estado activo
  engine.print();       // dibujo del estado activo
  sound.update();       // avanza la secuencia de sonido
  display.show();       // vuelca el buffer al panel
}
```

Puntos clave:

- **Una sola lectura de botones por frame** (`buttons.read()`). Nadie más lee los
  pines; el resto consulta el resultado (`pressed`, `heldDown`, `anyHeld`,
  `anyPhysical`).
- **Separación lógica/dibujo:** `update()` no dibuja; `print()` no hace lógica.
- **`display.show()` una vez por frame**, al final.
- Si `SleepManager::update()` devuelve `false`, el frame se corta (panel apagado).

### 3.4 Flujo de actualización (`Engine::update`)

`Engine` guarda un `State` privado y delega en la ventana activa; consulta
`done()` y, según el estado, transiciona con `changeState(...)`:

```
BOOT    → (Boot.done)    → LEGEND
LEGEND  → (Legend.done)  → MENU
MENU    → (Menu.done)    → según Menu.selected():
            OPT_NEW → NEW, OPT_CONTINUE → CONTINUE,
            OPT_DIFFICULTY → MENU_DIFFICULTY, OPT_SOUND → MENU_SOUND,
            OPT_CREDITS → MENU_CREDITS, (default) → LEGEND
MENU_SOUND / MENU_DIFFICULTY / MENU_CREDITS → (done) → MENU
NEW / CONTINUE → (Game.done) → MENU_CREDITS si isGameOver(), si no → MENU
```

### 3.5 Flujo de renderizado (`Engine::print`)

Delega en `print()` de la ventana activa. **No** limpia la pantalla: cada ventana
hace `display.clear()` solo cuando toca (normalmente en su primer frame tras
`begin()`, con el flag `_clear`).

| Ventana | Estáticos (primer frame) | Dinámicos (cada frame) |
|---------|--------------------------|------------------------|
| `Boot` | clear + banda del mensaje (Header) + logo (Body) | Mensaje del Header cuando `_blink.changed` |
| `Legend` | clear + títulos, pad MOVE, 4 rombos ACTION, banda del pie | Rombo activo (parpadea) + texto del pie (se desliza al cambiar) |
| `Menu` | clear + título, cuadro de opción, pie ("Best" + versión) | Banda de la opción (vuelo), marcadores y parpadeo del rombo/texto |
| `MenuDifficulty` | — (entra sobre el `Menu`) | Selector `< N >` y sus dos flechas |
| `MenuSound` | — (entra sobre el `Menu`) | Selector ON/OFF y su flecha |
| `MenuCredits` | clear + título (CREDITS / YOU WIN / GAME OVER) | Roll del tramo actual |
| `Game` | clear + puntaje + comidas + serpiente | Comidas, serpiente (al mover), puntaje (al comer), overlay de estado |

### 3.6 Sistema de estados

- `Engine::State` (privado): `BOOT, LEGEND, MENU, MENU_SOUND, MENU_SCALE, NEW,
  CONTINUE, MENU_DIFFICULTY, MENU_CREDITS` (`BOOT = 0`).
- `Game::State` (público): `START, PLAY, PAUSE, GAME_OVER`.
- `SleepManager::State` (privado): `AWAKE, SLEEPING`.

`changeState(s)` llama al `begin()` de la ventana destino **y después** guarda
`_state = s`; así la ventana entrante puede leer el estado anterior (p. ej.
`MENU_CREDITS` decide `fromGame` mirando si venía de `NEW`/`CONTINUE`).

`MENU_SCALE` es la única transición que **reinicia la partida**: si la escala
confirmada difiere de la activa (`MenuScale::changed()`) y había una partida
continuable (`Game::showContinue()`), el `Engine` salta a `NEW` en lugar de
volver a `MENU`, porque la rejilla nueva invalida el estado guardado de la
serpiente.

### 3.7 Comunicación entre clases

- Ventanas → servicios: por los globales (`display`, `buttons`, `sound`,
  `storage`).
- `Engine` → ventanas: las posee como miembros y llama a sus métodos.
- `Game` → `Snake`/`Food`: composición; pasa referencias/valores
  (`Snake::step(food, specialFood)`, `Food::spawn(game)`).
- `Food::spawn` → `Game`: recibe `const Game&` para consultar `occupied()`.
- `SleepManager` → `Engine`: recibe `Engine&` para `sleepBlocked()`.

### 3.8 Dependencias importantes

- El **orden de declaración de miembros en el `.h` manda**: las listas de
  inicialización de los constructores deben respetarlo (el core se compila con
  `-Wreorder`).
- Todo `.cpp` que use globales debe incluir `Globals.h`.

---

## 4. Clases

### `Engine` — despachador de ventanas

- **Responsabilidad:** máquina de estados; dueño de las ventanas; despacho de
  `update()`/`print()` y de las transiciones.
- **Datos internos:** `_state` y una instancia por ventana (`_boot`, `_legend`,
  `_menu`, `_menuSound`, `_menuScale`, `_menuCredits`, `_menuDifficulty`, `_game`).
- **Públicos:** `Engine()`, `begin()`, `update()`, `print()`, `inGame()`,
  `inCredits()`, `sleepBlocked()`.
- **Privados:** `changeState(State)`.
- **Dependencias:** todas las ventanas; `SleepManager` lo usa desde fuera.
- **Interacción:** llama al `begin()` de la ventana destino al cambiar de estado
  y a `update()`/`print()` de la activa cada frame. `sleepBlocked()` lo consulta
  `SleepManager` para no dormir en una partida activa o durante el roll.

### `Game` — ventana de la partida

- **Responsabilidad:** la partida (movimiento, puntuación, comida, overlays,
  subida de nivel y muerte).
- **Datos internos:** `_state`, `_win`, `_done`, `_clear`, `_score`, `_timer`
  (`Ticker`), `_blink`, `_countdown`, `_blinkChanged`, `_ateFood`,
  `_ateSpecialFood`, `_snake`, `_food`, `_specialFood`, `_countFood`.
- **Públicos:** `Game()`, `begin(bool newGame)`, `update()`, `print()`,
  `firstPrint()`, `done()`, `occupied()`, `showContinue()`, `isGameOver()`,
  `state()`, `win()`, `speedFor(uint8_t level)`.
- **Privados:** `step()`, `startPlay()`, `die()`, `drawOverlay()`, `spawnFood()`,
  `printScore()`.
- **Dependencias:** `Snake`, `Food`, `Ticker`, `Blink`, globales.
- **Interacción:** el `Engine` lo abre en `NEW`/`CONTINUE` y lo cierra con
  `done()`; `Food::spawn` lo recibe como `const Game&`.

### `Snake` — serpiente

- **Responsabilidad:** cuerpo (buffer circular), colisiones, giro y render.
- **Datos internos:** `_body[MAX_LENGTH]` (`Seg`), `_headIndex`, `_tailIndex`,
  `_length` (los tres `uint16_t`), `_dir`, `_nextDir`, `_bellyPending`,
  `_tailDropped`, `_droppedSeg`.
- **Públicos:** `Snake()`, `step(food, specialFood)` → `Result`, `occupied()`,
  `print()`, `reset()`, `clearPending()`, `turn(dir)` → `bool`, `length()`,
  `headPart(food, specialFood)` → `Sprite::Part`.
- **Privados:** `slot()`, `advance()`, `opposite()`, `bodyPartFor()`,
  `bellyPartFor()`, `drawSprite()`.
- **Dependencias:** `Food`, `Draw` (`Direction`), `Sprite`, `Playfield`,
  `Display`.
- **Interacción:** `Game::step()` llama a `Snake::step()`; `Food::spawn` evita
  celdas ocupadas vía `Game::occupied` → `Snake::occupied`.
- **`Result`:** `MOVED`, `ATE_FOOT` ("FOOD" con un typo histórico), `ATE_SPECIAL`,
  `DIED`.
- **`advance()`** es el único punto donde se calcula el *wrap*: toma la celda y la
  dirección y devuelve la celda siguiente, envolviendo en los bordes según las
  columnas y filas vivas de `Playfield`.

### `Food` — comida

- **Responsabilidad:** comida normal y especial (aparición aleatoria, cuenta
  atrás de la especial y dibujo).
- **Datos internos:** `_type`, `_x`, `_y`, `_has`, `_time`, `_lastTime`, `_score`
  (`uint16_t`).
- **Públicos:** `Food(Type)`, `clear()`, `print()`, `occupied()`, `lessTime()`,
  `spawn(const Game&)`, `printTime(bool)`, `score()`.
- **Privados:** `specialTime()`, `printNormal(bool)`, `printSpecial(bool)`.
- **Dependencias:** `Game` (celdas libres), `Storage` (puntuación), `Draw`,
  `Special`, `Playfield`, `Display`.
- **Interacción:** `Game` tiene dos instancias (`_food` normal, `_specialFood`
  especial); `Snake::step` las consulta para comer.
- **`Type`:** `NORMAL`, `SPECIAL`.

### Ventanas del menú: `Menu`, `MenuDifficulty`, `MenuSound`, `MenuScale`, `MenuCredits`

- **`Menu`:** opciones en `Option` (`OPT_NEW, OPT_CONTINUE, OPT_DIFFICULTY,
  OPT_SOUND, OPT_SCALE, OPT_CREDITS, OPT_COUNT`). Navega con `ButtonRepeat`
  (`MOVE_LEFT/RIGHT`); `ACTION_RIGHT` pone `_confirm` y `blinkOption()` lo
  convierte en `_done`. Públicos locales: `showOptions()`, `selected()`.
  Datos comunes: `_isScrolling, _selected, _lastSelected, _blink, _repeat,
  _scroller, _done, _clear`; locales: `_showContinue, _space, _confirm`.
- **`MenuDifficulty`:** capa sobre `Menu`; nivel 1..10 con flechas `< >`. Datos:
  `_selected, _lastSelected, _done, _blink, _clear, _repeat`. `clear()` y
  `nextOption(bool)` son públicos.
- **`MenuSound`:** capa sobre `Menu`; ON/OFF con flechas. Datos comunes:
  `_done, _selected(bool), _lastSelected(bool), _blink, _clear`; locales:
  `_startX[], _endX[]`.
- **`MenuScale`:** capa sobre `Menu`; escala del tablero (4 / 8 / 12 px por
  celda) con flechas y `ButtonRepeat`. Confirma con `ACTION_RIGHT` (fija la
  escala en `Storage` y marca `changed()` si difiere de la activa) y cancela con
  `ACTION_UP` (deja la escala intacta). Datos comunes: `_done, _selected,
  _lastSelected, _blink, _clear, _changed`; locales: `_startX[], _endX[],
  _label[][], _repeat`.
- **`MenuCredits`:** roll vertical; título según `fromGame`/`winner`
  (CREDITS / YOU WIN / GAME OVER), tarjeta final `Thanks for playing!` / datos de
  versión. Público local: `card()`. Datos comunes: `_clear, _done, _scroller,
  _isScrolling, _card, _pause`; locales: `_fromGame, _winner, _blockH, _rest,
  _stay, _line`. Privados: `startStay()`, `startExit()`, `startCard()`,
  `startBlock()`, `drawRoll()`.

### Ventanas de arranque/ayuda: `Boot`, `Legend`

- **`Boot`:** logo (`Logo::LOGO`, 80x48) + `MESSAGE[2]` = {"Press any button",
  "to start"} en el Header, parpadeando. Datos: `_blink, _done, _clear`.
- **`Legend`:** pad MOVE (4 triángulos) + 4 rombos ACTION; el rombo activo rota
  cada `NEXT` ms (`Ticker`) y su nombre se desliza en el pie (`Scroller`). Datos
  comunes: `_selected, _lastSelected, _scroller, _blink, _isScrolling, _done,
  _clear`; local: `_ticker`. Público local: `toggleDiamond()`.

### Servicios: `Display`, `Buttons`, `Sound`/`Buzzer`, `Storage`, `SleepManager`

- **`Display`:** envuelve `Adafruit_SSD1306`. `begin()` monta Wire+panel (400 kHz)
  y, si el panel no responde, imprime error y se queda en bucle. Expone `clear`,
  `show`, `power`, `drawPixel`, `centerX`, `fillRect`, `fillTriangle`, `drawText`,
  `drawBitmap`. Los pines y la dirección I2C son los alias estáticos
  `Display::SDA`/`SCL`/`ADDRESS` (tomados de `Config`).
- **`Buttons`:** antirrebote por botón. `read()` muestrea, agrupa los cambios y
  confirma tras `_buttonDelay` ms estables. Datos: `_pins, _rawButtons, _buttons,
  _pressed, _released, _ignore, _buttonLast, _buttonDelay`.
- **`Buzzer`:** PWM (`ledcAttach`/`ledcWriteTone`). **No** es global: vive como
  `_buzzer` dentro de `Sound`. Un tono con duración; `update()` lo apaga al
  vencer; `busy()`. El pin es el alias estático `Buzzer::PIN` (de `Config`).
  Datos: `_attached, _busy, _startMs, _durationMs`.
- **`Sound`:** reproductor de `Seq` (`EFFECTS[]`); posee el `Buzzer`, avanza
  notas en `update()`, soporta `loop`, respeta `storage.soundEnabled()`. Datos:
  `_buzzer, _seq, _len, _step, _loop`.
- **`Storage`:** getters/setters de `_bestScore` (`uint32_t`), `_soundEnabled`,
  `_difficulty` y `_scale` (con recorte a `MIN/MAX_LEVEL` y `MIN/MAX_CELL`, y
  normalizado a múltiplos de `CELL_STEP`). Sin persistencia: vive en RAM.
- **`SleepManager`:** ver §3.3 y §16. `update()` devuelve `bool`. En reposo,
  `esp_light_sleep_start()` bloquea hasta que un botón físico despierta.

### Helpers: `Timer`, `Blink`, `ButtonRepeat`, `Draw`, `Playfield`, `Scroller`

- **`Stopwatch`/`Ticker`:** §10.
- **`Blink`:** `start()` ancla; `isVisible(period, offPct)`; `changed(...)` da el
  flanco. Datos: `_timer` (Stopwatch), `_last`.
- **`ButtonRepeat`:** `step(button)` → `true` en la pulsación y luego cada `TICK`
  ms tras `DELAY` ms mantenido.
- **`Draw`:** `triangle(dir, cx, cy, show)` y `diamond(cx, cy, show)` (rombo =
  triángulo arriba + abajo). `Direction`: `DIR_NONE, DIR_UP, DIR_RIGHT, DIR_DOWN,
  DIR_LEFT`.
- **`Playfield`:** namespace sin estado; traduce la escala activa
  (`storage.scale()`) a la geometría del área jugable: `cell()` (px por celda),
  `pixel()` (px por punto de sprite = `cell()/4`), `cols()`, `rows()`,
  `originX()` y `originY()` (esquina superior izquierda de la rejilla, centrada
  en el cuerpo). La rejilla reserva `Config::Board::MARGIN` px por lado para el
  perímetro.
- **`Scroller`:** buffer de 1 bit (`_strip`) y vuelo con cortina; horizontal
  (texto) y vertical (bloques). §4.1.

#### 4.1 `Scroller` en detalle

- Buffer `_strip[HEIGHT/8][WIDTH]` (1 bit/píxel; cada byte = 1 columna de 8 px).
- Un **bloque** de hasta `MAX_LINES` (12) líneas; cada línea lleva texto (puntero,
  **no** copia), `size` (1..3), color de letra y de fondo.
- `begin(dir, EndMode)`: `EndMode::STAY` (reposa en el destino) o `EXIT` (sale;
  solo vertical).
- `setRow(y)` (horizontal) o `setVertical(to, from)` (vertical); `addText(...)`;
  `startSlide()`; `update()`; `print()`; `done()`.
- El fondo ocupa el ancho completo y el texto va centrado; en vertical se recorta
  a la banda del Body.

---

## 5. Funciones relevantes

### `src/devices/Timer.h`

- `uint64_t nowMs()` — reloj del sistema en ms (`esp_timer_get_time()/1000`).
  Base de todos los cronómetros; 64 bits (no desborda).
- `Stopwatch::start()` — ancla el instante actual.
- `Stopwatch::elapsed()` — ms desde `start()`.
- `Stopwatch::expired(ms)` — ¿pasaron ≥ `ms`?
- `Stopwatch::blinkOn(period, offPct)` — fracción "encendida" del ciclo (lo usa
  `Blink`).
- `Ticker::consume()` — pasos de `_period` ms desde la última llamada.

### `src/devices/Buttons.cpp`

- `read()` — muestrea los 8 pines, antirrebote por botón, rellena `_buttons`
  (estable) y `_pressed`/`_released` (flancos del frame). Si `_ignore` está
  activo, suprime todo hasta que los pines estén en bajo.
- `ignoreUntilRelease()` — ignora botones hasta soltarlos todos (anti-entrada).
- `anyHeld()` — algún botón *estable* presionado.
- `anyPhysical()` — algún pin físicamente alto (`_rawButtons`), sin antirrebote ni
  supresión. Lo usa `SleepManager`.

### `src/windows/game/Game.cpp`

- `begin(bool newGame)` — fija velocidad (`speedFor(storage.difficulty())`),
  `_state = PAUSE`, limpia giro pendiente. `newGame=true`: `reset()`, siembra RNG
  (`esp_random() ^ nowMs()`), coloca la especial y luego la normal, arranca
  `START` (conteo) y `_blink`. `newGame=false`: queda en `PAUSE` (conserva tablero).
- `update()` — por estado: `START` (conteo; `ACTION_RIGHT` arranca), `PLAY`
  (giros con MOVE, pausa con `ACTION_RIGHT`, avance por `Ticker`), `PAUSE`
  (reanudar), `GAME_OVER`. `ACTION_UP` sale en cualquier estado salvo
  `GAME_OVER` (la partida no se pierde).
- `step()` — un tick: `_snake.step(food, specialFood)`; `DIED` → `die()`. Si no,
  `_specialFood.lessTime()`, marca comido, `SFX_EAT`, suma puntaje, limpia y
  respawnea la comida correspondiente; cada `FOOD_COUNT` (10) comidas normales
  respawnea la especial.
- `spawnFood(Food&)` — `clear()` + `spawn(*this)`; si no hay celda libre,
  `_snake.reset()` y, si no está en `MAX_LEVEL`, sube la dificultad y respawnea;
  si ya está en el máximo, `die()`.
- `speedFor(level)` — recorta `level` a `[MIN_LEVEL, MAX_LEVEL]` y hace
  `_timer = Ticker(1000 - (level-1)*101 - (level+1)/3)`.
- `die()` — `GAME_OVER`; `_win = _score > storage.bestScore()`; guarda el récord
  si procede; `_done = true` (el `Engine` salta a `MENU_CREDITS`).
- `occupied(x, y)` — celda ocupada por serpiente o cualquiera de las comidas.
- `showContinue()` — `!isGameOver() && _score > 0`.
- `print()` / `firstPrint()` — comida, serpiente, puntaje y overlays.

### `src/windows/game/Snake.cpp`

- `step(food, specialFood)` — siguiente celda (con *wrap* en X/Y); colisión
  contra el cuerpo (ignorando la cola si no come); avanza cabeza/cola en el
  buffer circular; devuelve `MOVED`/`ATE_FOOT`/`ATE_SPECIAL`/`DIED`.
- `advance(col, row, dir, &nextCol, &nextRow)` — celda siguiente con *wrap*;
  único punto del *wrap* (lo usan `step()` y `headPart()`).
- `turn(dir)` — guarda el giro pendiente si no es `DIR_NONE`, ni el mismo rumbo,
  ni el opuesto. Devuelve `true` si lo aceptó (para `SFX_TURN`).
- `occupied(cellX, cellY)` — ¿hay segmento en esa celda?
- `reset()` — serpiente inicial (4 segmentos hacia la derecha).
- `headPart(food, specialFood)` — sprite de cabeza (abierta si va a comer,
  cerrada si no) según la dirección.
- `print(food, specialFood)` — borra la cola soltada y dibuja cada segmento.

### `src/windows/game/Food.cpp`

- `spawn(const Game&)` — cuenta celdas libres y elige una al azar (la especial
  ocupa 2 de ancho); `_score = storage.difficulty()` (×10 si especial) y arranca
  `_time`. Devuelve `false` si no cabe.
- `specialTime()` — vida de la comida especial = columnas + filas de la
  rejilla viva (`Playfield::cols() + Playfield::rows()`).
- `occupied(x, y)` — normal ocupa 1 celda; especial 2 (x y x+1).
- `lessTime()` — decrementa la vida de la especial.
- `printTime(bool)` — dibuja la cuenta atrás en el Header; al llegar a 0 se llama
  `clear()` (efecto lateral).
- `print()` / `printNormal()` / `printSpecial()` — dibujan la comida.

### `src/windows/Menu.cpp`

- `begin(bool showContinue)` — fija la lista visible (6 con Continue, 5 sin él),
  reinicia flags, reancla `_blink`, ignora botones hasta soltar, y decide si
  limpia todo (opciones "de página": NEW/CONTINUE/CREDITS) o solo repinta la
  banda (`showOptions()`, para DIFFICULTY/SOUND).
- `update()` — `navigate()` + `action()`; mientras el `Scroller` vuela, congela.
- `navigate()` — `MOVE_LEFT/RIGHT` con `ButtonRepeat`, saltando `OPT_CONTINUE` si
  no está visible.
- `action()` — `ACTION_UP` → salta a Continue/New; `ACTION_RIGHT` → `_confirm`.
- `blinkOption()` — convierte `_confirm` en `_done`; parpadea texto y rombo.
- `selected()` — opción confirmada (la lee el `Engine`).

### `src/graphics/Scroller.cpp`

- `begin(dir, end)`, `addText(...)`, `setRow(y)`, `setVertical(to, from)`,
  `setEndMode(...)`, `startSlide()`, `update()`, `print()`.

### `src/core/Engine.cpp`

- `update()` / `print()` — despacho por estado (§3.4/§3.5).
- `changeState(State)` — llama `begin()` de la ventana destino y guarda el estado.
- `inGame()` — `(NEW || CONTINUE) && _game.state() == Game::State::PLAY`.
- `inCredits()` — `MENU_CREDITS && !_menuCredits.card()`.
- `sleepBlocked()` — `inGame() || inCredits()`.

---

## 6. Configuración

Todo lo compartido se ajusta en **`Config.h`** (namespace `Config`).

| Qué cambiar | Archivo | Constante |
|-------------|---------|-----------|
| Resolución / pantalla | `Config.h` | `Config::Screen::WIDTH`, `HEIGHT`, `ADDRESS` |
| Tamaño de fuente base | `Config.h` | `Config::Screen::CHAR_W`, `CHAR_H` |
| Regiones (cabecera/cuerpo/pie) | `Config.h` | `Config::Screen::HEADER_TOP`, `BODY_TOP`, `BODY_H`, `BODY_MIDDLE`, `FOOT_H`, `FOOT_TOP` |
| Rejilla del tablero | `Config.h` | `Config::Board::CELL_MIN`, `CELL_MAX`, `CELL_DEFAULT`, `CELL_STEP`, `MARGIN` (derivan `MAX_LENGTH` y la geometría de `Playfield`) |
| Banda central de los menús | `Config.h` | `Config::MenuStrip::*` |
| Pines (botones, buzzer, I2C) | `Config.h` | `Config::Pin::*` |
| Rango de dificultad | `Config.h` | `Config::Difficulty::MIN_LEVEL`, `MAX_LEVEL`, `DEFAULT_LEVEL` |
| Velocidad de la serpiente | `src/windows/game/Game.cpp` | fórmula en `Game::speedFor()` |
| Tiempo de reposo | `Config.h` | `Config::Screen::SLEEP_MIN` (minutos) |
| Identidad / versión | `Config.h` | `Config::Version::NAME`, `VERSION`, `RELEASE_DATE` |
| Parpadeo estándar | `src/devices/Blink.h` | `Blink::PERIOD`, `Blink::OFF` |
| Repetición de botón | `src/devices/ButtonRepeat.h` | `ButtonRepeat::DELAY`, `ButtonRepeat::TICK` |
| Antirrebote | `.ino` | `Buttons buttons(Buttons::PINS, 30)` (30 ms) |
| Velocidad I2C | `src/devices/Display.h` | `Display::CLOCK_HZ` (400 kHz) |
| Frecuencia del buzzer | `src/devices/Buzzer.h` | `ledcAttach(PIN, 2000, 10)` en `begin()` |
| Sonidos / frecuencias | `src/devices/Sound.h` | tablas `SEQ_*` y `EFFECTS[]` |
| Comidas para comida especial | `src/windows/game/Game.h` | `Game::FOOD_COUNT` (10) |
| Conteo 3-2-1 | `src/windows/game/Game.h` | `Game::COUNT`, `PERIOD`, `OFF` |
| Vida de la comida especial | `src/windows/game/Food.cpp` | `Food::specialTime()` (= columnas + filas de `Playfield`) |
| Escala activa del tablero | `src/devices/Storage.h/.cpp` | `Storage::scale()` / `setScale()` (rango `MIN_CELL`..`MAX_CELL`) |
| Textos de menú | `src/windows/Menu.h` | `Menu::OPTION[]` |
| Créditos y títulos | `src/windows/MenuCredits.h` | `CREDITS[]`, `TITLE_*`, `PAUSE_MS` |
| Leyenda de botones | `src/windows/Legend.h` | `BTN_FUNC[]`, `NEXT` |
| Estado inicial de la serpiente | `src/windows/game/Snake.h` | `Snake::START_COL/ROW/LENGTH/DIR` |
| Sprites | `src/graphics/Sprite.h` | `Sprite::SPRITES`, `Special::SPECIAL_FOOD`, `Logo::LOGO` |

Ejemplo:

```text
Para cambiar la velocidad base del nivel:
  Archivo: src/windows/game/Game.cpp
  Función: Game::speedFor(uint8_t level)

Para cambiar el rango de dificultad:
  Archivo: Config.h
  Constante: Config::Difficulty::MAX_LEVEL

Para cambiar el tamaño de celda del tablero:
  Archivo: Config.h
  Constante: Config::Board::CELL_MIN / CELL_MAX / CELL_DEFAULT

Ojo: la geometría viva la calcula el namespace Playfield
(src/graphics/Playfield.h/.cpp), no el tablero directamente.
```

> Convenio: lo **verdaderamente compartido** va en `Config.h`; lo específico de
> una clase queda como `static constexpr` dentro de ella y, si viene de `Config`,
> se reexpone con un **alias** en su `.h` (los `.cpp` no usan `Config::` directo).

---

## 7. Sistema de entrada

- **8 botones**, en dos grupos de 4: `MOVE_UP/RIGHT/DOWN/LEFT` y
  `ACTION_UP/RIGHT/DOWN/LEFT` (enum `Buttons::Button`). Pines en
  `Config::Pin::*` y en `Buttons::PINS` (mismo orden).
- **Uso convencional:** `MOVE_*` navega/mueve; `ACTION_RIGHT` confirma/actúa;
  `ACTION_UP` vuelve al menú (común a todas las ventanas).
- **Configuración eléctrica:** `INPUT_PULLDOWN` (se leen en `HIGH` al pulsar).
- **Lectura:** `Buttons::read()` **una vez por frame** (en `loop()`), con
  antirrebote de `_buttonDelay` ms.
- **Eventos que expone:**
  - `pressed(i)` — flanco de subida en este frame.
  - `released(i)` — flanco de bajada.
  - `heldDown(i)` — botón estable presionado.
  - `anyHeld()` — algún botón estable presionado (respeta `ignoreUntilRelease`).
  - `anyPhysical()` — algún pin físicamente alto (crudo; para el reposo).
  - `ignoreUntilRelease()` — suprime todo hasta soltar los botones.
- **Cómo agregar un botón:**
  1. Añade el pin en `Config::Pin`.
  2. Añade el valor al enum `Buttons::Button` (antes de `MAX_BUTTONS`) y su pin
     en `Buttons::PINS`.
  3. `read()` itera `MAX_BUTTONS` automáticamente.
- **Cómo cambiar el comportamiento de un botón:** edita las condiciones de la
  ventana correspondiente (`Game`, `Menu`, `MenuCredits`, ...).

---

## 8. Sistema de sonido

- **Hardware:** zumbador PWM en `Config::Pin::BUZZER` (GPIO 14), gestionado por
  `Buzzer` (`ledcAttach` a 2000 Hz/10 bits; `ledcWriteTone`).
- **Secuencias:** en `src/devices/Sound.h` como `static constexpr Note SEQ_*[]` (pares
  `{freq, durMs}`) y la tabla `EFFECTS[]` que asocia cada `Sound::Sfx` con su
  secuencia y longitud. `freq == 0` = pausa.
- **Funcionamiento:** `Sound::play(effect, loop)` selecciona la secuencia;
  `Sound::update()` (cada frame) avanza una nota cuando el `Buzzer` deja de estar
  ocupado; con `loop` reinicia al terminar, si no se detiene.
- **Efectos (`Sound::Sfx`):** `SFX_NONE, SFX_CLICK, SFX_CONFIRM, SFX_BACK,
  SFX_EAT, SFX_START, SFX_TICK, SFX_TURN, SFX_PAUSE, SFX_RESUME, SFX_BOOT,
  SFX_PIRATES, SFX_DRAGON_BALL` (más `SFX_COUNT`).
- **Música de fondo:** `SFX_PIRATES` (roll de créditos normal) y
  `SFX_DRAGON_BALL` (créditos tras victoria), en bucle.
- **Cómo agregar un efecto:**
  1. Añade el valor al enum `Sound::Sfx` (antes de `SFX_COUNT`).
  2. Define su `static constexpr Note SEQ_XXX[]` en `src/devices/Sound.h`.
  3. Añade su fila en `EFFECTS[]` (mismo orden que el enum) con la longitud
     `sizeof(SEQ_XXX)/sizeof(Sound::Note)`.
  4. Llama `sound.play(Sound::SFX_XXX, false)` donde toque.
- **Cómo modificar un sonido:** cambia los pares `{freq, durMs}` de su `SEQ_*`.
- **Activar/desactivar:** `storage.setSoundEnabled(bool)` (lo hace `MenuSound`);
  en off, `Sound::update()` corta y `play()` ignora.

---

## 9. Sistema gráfico

- **Pantalla:** SSD1306 monocroma 128x64 por I2C (`Config::Screen::ADDRESS`,
  `Config::Pin::OLED_SDA/SCL`). `Display` reenvía a la librería; el
  `display.show()` final vuelca el buffer completo.
- **Regiones:** Header `0..15`, Body `16..63`, Foot `56..63`.
- **Coordenadas:** origen arriba-izquierda; x a la derecha, y hacia abajo. La
  celda `(col,row)` del tablero se dibuja en
  `(Playfield::originX() + col*cell, Playfield::originY() + row*cell)`, con
  `cell` px por celda.
- **Colores:** `true` = encendido (blanco), `false` = apagado.
- **Sprites:**
  - Serpiente: `Sprite::SPRITES[]`, 4x4 empaquetado en un `uint16_t`;
    `Sprite::pixel(part, x, y)`.
  - Comida normal: `Sprite::FOOD` (4x4, dentro de `SPRITES[]`).
  - Comida especial: `Special::SPECIAL_FOOD[]` (8x4), `Special::specialPixel(...)`.
  - Logo: `Logo::LOGO[]` (80x48, 1 bit/px, polaridad invertida: 1 = fondo).
- **Dibujado:** serpiente y comida especial escalan cada bit del sprite a un
  cuadrado `pixel`x`pixel` px (`pixel = cell/4`). La comida normal se dibuja con
  el sprite `Sprite::FOOD` (4x4, arte 3x3 centrado en la celda con `pixel/2`).
- **Escalas:** 4, 8 y 12 px por celda (`Config::Board::CELL_MIN/MAX/STEP`). La
  rejilla resultante es 31x11, 15x5 o 10x3 celdas, y siempre reserva
  `Config::Board::MARGIN` px por lado para el perímetro.
- **Animaciones:** parpadeo (`Blink`), vuelos de texto (`Scroller`, con cortina)
  y el conteo del juego (`Blink` + overlays).
- **Cómo agregar/modificar un sprite:**
  1. Añade/edita la tabla en `src/graphics/Sprite.h` (`SPRITES` + valor en `enum Part`, o
     `SPECIAL_FOOD`).
  2. Respeta el tamaño (4x4 la serpiente y la comida normal, 8x4 la especial) y lee con
     `pixel()`/`specialPixel()`.
  3. Dibújalo con `display.fillRect(...)` por bit (ver `Snake::drawSprite` y
     `Food::printSpecial`).

---

## 10. Temporizadores

- **`nowMs()`** — reloj monótono de 64 bits en ms. Único punto de tiempo;
  sustituye a `millis()` (que desborda a los ~49 días).
- **`Stopwatch`** — "¿pasaron X ms desde `start()`?". Para esperas puntuales:
  `start()` + `expired(ms)`; `blinkOn()` para parpadeos.
- **`Ticker`** — acumulador de "pasos" de `period` ms. Para ritmos que deben poder
  avanzar varios pasos si un frame se alarga (p. ej. la velocidad de la
  serpiente): `start()` y, cada frame, `consume()`.

| Necesidad | Herramienta |
|-----------|-------------|
| ¿Pasó un tiempo desde un evento? | `Stopwatch` |
| Temporizador de un solo disparo | `Stopwatch` |
| Ritmo/velocidad con pasos acumulables | `Ticker` |
| Parpadeo | `Blink` (usa `Stopwatch` por dentro) |

---

## 11. Flujo de una partida

```
Encendido
  ↓
Boot        (logo + "Press any button"; cualquier botón)
  ↓
Legend      (ayuda de botones; cualquier botón)
  ↓
Menu        (New / Continue / Difficulty / Sound / Credits)
  ├─ Difficulty → MenuDifficulty (nivel 1..10) → Menu
  ├─ Sound      → MenuSound (ON/OFF)           → Menu
  ├─ Credits    → MenuCredits (roll + tarjeta) → Menu
  ↓  New / Continue
Game
  ├─ START : conteo 3-2-1 (SFX_TICK) → ACTION_RIGHT o fin del conteo → PLAY
  ├─ PLAY  : mover → comer (SFX_EAT) → crecer; subir de nivel o morir
  ├─ PAUSE : ACTION_RIGHT pausa/reanuda (SFX_PAUSE / SFX_RESUME)
  └─ ACTION_UP vuelve al Menu sin terminar la partida
  ↓  colisión
Game Over   (die(): guarda récord si procede)
  ↓
MenuCredits (GAME OVER / YOU WIN, luego tarjeta)
  ↓
Menu        (Continue disponible si la partida sigue viva y con puntos)
```

Notas:

- `Continue` solo aparece cuando `Game::showContinue()` es `true`
  (`!isGameOver() && _score > 0`).
- Al llenar el tablero, `spawnFood` sube la dificultad (si no está en el máximo);
  en el máximo, `die()`.
- El récord se comprueba/actualiza solo al morir (`die()`); salir con `ACTION_UP`
  no lo actualiza.

---

## 12. Cómo modificar el juego

- **Velocidad de la serpiente:** `Game::speedFor()` (`src/windows/game/Game.cpp`).
- **Dificultad:** rango en `Config::Difficulty`; el nivel vivo en `Storage`.
- **Tamaño del tablero:** escala jugable en `Storage` (`scale()`), con los
  límites en `Config::Board::CELL_MIN/MAX/STEP`; la geometría la calcula
  `Playfield` (`src/graphics/Playfield.h/.cpp`). `MAX_LENGTH` es el tamaño
  máximo de la rejilla (31x11 = 341 segmentos en la escala fina) y los sprites
  asumen que `cell` es múltiplo de 4 (`pixel = cell/4`), así que el sprite 4x4
  llena la celda en cualquier escala.
- **Escala del tablero:** `src/windows/MenuScale.h/.cpp` (opción "Scale" del
  menú) + `src/graphics/Playfield.h/.cpp` (geometría) + `Config::Board::*`
  (límites). Cambiarla reinicia la partida en curso (§3.6).
- **Controles:** condiciones de cada ventana + enum `Buttons::Button`/`Config::Pin`.
- **Sprite:** `src/graphics/Sprite.h` (§9).
- **Agregar comida:** otra instancia `Food` en `Game` y llamarla en
  `step()`/`print()` (modelo: `_food` y `_specialFood`).
- **Agregar un sonido:** §8.
- **Modificar un menú:** `Menu::OPTION[]`/`Menu::Option` y el despacho de
  `Engine::update()`.
- **Agregar una pantalla nueva:** §13.
- **Modificar una animación:** `Scroller` (vuelos), `Blink` (parpadeos), `Game`/`Menu`.
- **Cambiar un texto:** `Menu::OPTION[]`, `MenuCredits::CREDITS[]`/`TITLE_*`,
  `Boot::MESSAGE[]`, `Legend::BTN_FUNC[]`, `Config::Version::*`.

---

## 13. Cómo agregar nuevas funcionalidades

Regla general: **cada ventana es un módulo**, y el `Engine` solo la orquesta.

1. **Nueva pantalla / ventana:**
   1. Crea `MiVentana.h/.cpp` con el patrón `begin/update/print/firstPrint/done`
      (copia el esqueleto de una simple, p. ej. `Boot`).
   2. Añádela como miembro en `Engine.h` y su `#include`.
   3. Añade el estado a `Engine::State`.
   4. Cablea: en `Engine::update()` llámala y transiciona con su `done()`; en
      `Engine::print()` dibújala; en `Engine::changeState()` llama a su `begin()`.
   5. Añade la entrada que la abre (p. ej. una opción en `Menu::Option` + su
      `case` en `Engine::update()`).
2. **Nuevo servicio (hardware/estado):**
   1. Clase nueva con su `.h/.cpp`.
   2. `extern` en `Globals.h` e instancia en el `.ino`.
   3. Llama su `begin()` en `setup()` y su `update()` en `loop()` si procede.
3. **Nueva constante compartida:** en `Config.h`; reexpónla con alias en el `.h`
   de la clase que la use (no uses `Config::` directo en los `.cpp`).
4. **Nuevo sonido:** §8.
5. **Nueva regla/dificultad:** normalmente en `Game` (o `Storage` si es estado
   compartido).

---

## 14. Dependencias

- **Librerías (no vendorizadas; se instalan desde el gestor de Arduino):**
  - **Adafruit GFX Library** (BSD-3-Clause): primitivas de dibujo y fuente.
  - **Adafruit SSD1306** (BSD-3-Clause): driver del panel OLED.
  - **Adafruit BusIO** (BSD-3-Clause): requerida por GFX/SSD1306.
- **Framework:** Arduino-ESP32 core de Espressif, versión **3.3.12** (integrado
  en el IDE). Incluye ESP-IDF (`esp_timer`, `esp_sleep`, `driver/gpio`,
  `esp_random`).
- **Hardware:**
  - MCU: ESP32-S3 (placa "ESP32-S3 Dev Module").
  - Pantalla: OLED SSD1306 128x64, I2C (SDA=8, SCL=9, dir `0x3C`).
  - 8 botones `INPUT_PULLDOWN` (§7).
  - Zumbador PWM en GPIO 14.
- **Dependencias externas en ejecución:** ninguna (sin red).
- **Atribuciones de terceros:** `THIRD_PARTY_NOTICES.md`.

---

## 15. Convenciones del proyecto

- **Nombres:** variables y parámetros completos (`centerX`, no `cx`). Miembros con
  prefijo `_` (`_state`, `_done`). Constantes en MAYÚSCULAS.
- **Organización de archivos:** una clase por `.h/.cpp`; el nombre del archivo
  coincide con el de la clase. Servicios en la raíz del sketch.
- **Uso de `const`:** getters y funciones de consulta son `const`. El core se
  compila con `-Wall -Wreorder` (vía `platform.local.txt`, fuera del repo) para
  que salten los desajustes de orden de inicialización.
- **Uso de `enum`:** `enum class` cuando el tipo importa (`Game::State`,
  `Scroller::EndMode`, `Food::Type`); `enum` simple para índices/valores
  (`Buttons::Button`, `Menu::Option`, `Sprite::Part`, `Sound::Sfx`).
- **Organización de las clases (secciones rotuladas):** `// Constantes de Config`,
  `// Constantes`, `// Variables comunes de las ventanas` (las compartidas por las
  ventanas: `_done`, `_clear`, `_blink`, `_selected`, `_lastSelected`,
  `_lastScroll`, `_scroller`, `_repeat`, `_isScrolling`, `_card`, `_pause`),
  `// Variables locales` (propias, ordenadas por uso). **El orden de declaración
  del `.h` manda**: la lista de inicialización del constructor debe respetarlo.
- **Organización de los métodos:** en el `.cpp`, `// Funciones comunes de las
  ventanas` (ciclo de vida) y `// Funciones locales` (resto).
- **Comentarios:** separadores de bloque de 3 líneas (`// ===`, rótulo, `// ===`),
  5 líneas vacías entre bloques y cierre `// Fin — <rótulo>` al final de cada
  bloque y del archivo.
- **Constantes:** se declaran en `Config.h` y cada clase las reexpone como alias
  en su `.h`; los `.cpp` no usan `Config::` directamente.
- **Alcance:** al trabajar en una clase, modificar solo esa clase salvo indicación
  contraria. Nuevas funcionalidades: §13.
- **Idioma:** identificadores en inglés; comentarios y documentación en español.

---

## 16. Problemas conocidos

Encontrados durante la revisión del código. **No** se corrigen aquí porque hacerlo
podría cambiar el comportamiento actual.

- **Sin persistencia:** `Storage` (`_bestScore`, `_soundEnabled`, `_difficulty`,
  `_scale`) vive solo en RAM; al reiniciar vuelve a los valores por defecto
  (récord 0, sonido ON, dificultad `DEFAULT_LEVEL`, escala `CELL_DEFAULT`). No
  hay guardado en flash.
- **Perímetro sin dibujar:** `Config::Board::MARGIN` reserva 1 px alrededor de la
  rejilla (la serpiente se envuelve, no choca con las paredes), pero ese marco
  todavía no se dibuja: la reserva existe para poder pintarlo después sin
  tocar la geometría.
- **Dificultad no aplicada al juego:** la dificultad real la fija
  `storage.difficulty()` en `Game::begin()`. `MenuDifficulty` guarda su nivel en
  `_difficulty` local y **no** llama `storage.setDifficulty(...)`, así que el
  nivel elegido en ese menú no afecta a la partida (y no se recuerda al reentrar,
  `STORAGE`/`Sound` siguen con copias parciales).
- **Título de créditos según el origen:** `Engine::changeState(MENU_CREDITS)`
  calcula `fromGame = (_state == NEW || CONTINUE)`. Si se abre "Credits" desde el
  `Menu`, `fromGame` es `false` y el título es `CREDITS` aunque antes haya habido
  una muerte; "GAME OVER"/"YOU WIN" solo aparecen si se llega justo tras terminar
  una partida.
- **`Game::speedFor` usa `Config::Difficulty::*` directamente** (`src/windows/game/Game.cpp`)
  en lugar de alias locales en `src/windows/game/Game.h`; inconsistencia de estilo.
- **`Food::printTime`** llama a `clear()` como efecto lateral cuando `_time == 0`;
  funciona, pero mezcla dibujo y cambio de estado.
- **`Scroller::startSlide`** fija `_size` con `_lines[0].size` para toda la
  cortina; con líneas de distinto tamaño la cortina usa el de la primera.
- **`ButtonRepeat`** guarda `_tick`/`_delay` que no se usan (las constantes
  `DELAY`/`TICK` son `static constexpr`); el constructor ni los inicializa.
- **`Snake::length()`** es público pero no se usa fuera de la clase.
- **`Engine::inGame()`/`inCredits()`** son públicos pero solo los usa
  `sleepBlocked()` internamente (candidatos a privado).
- **Código comentado heredado:** hay bloques comentados en varios archivos
  (p. ej. `Menu.h` con la API anterior, ajustes de audio en `Sound`). No afectan
  al comportamiento, pero aumentan el ruido.

---

_Fin de la documentación técnica._
