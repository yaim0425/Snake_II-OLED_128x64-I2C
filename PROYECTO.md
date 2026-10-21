# Snake_II-OLED_128x64-I2C — ESP32-S3

Snake II (estilo Nokia) para ESP32-S3 con Arduino IDE. POO: una clase por archivo `.h`/`.cpp`.

---

## 0. Reglas de trabajo con la IA

### Principios

- Toda regla que el usuario dé debe quedar escrita en este documento, para que sobreviva a la siguiente sesión.
- La IA no debe hacer nada sin una orden explícita del usuario.
- La **frase de inicio de sesión** (ver al final de esta sección) es una orden explícita, pero **solo** para retomar el trabajo ya registrado como pendiente o en curso (PROYECTO.md + el `.json`). Iniciar fases nuevas o ampliar el alcance exige una orden aparte.
- No crear, modificar ni eliminar archivos por iniciativa propia.
- Excepción: la IA puede modificar el .gitignore en cualquier momento, sin esperar una orden explícita.
- Proponer pero no iniciar fases de desarrollo por su cuenta; solo actuar cuando el usuario lo indique.

### Flujo de commits

- La IA debe hacer un commit cuando el usuario lo ordene o como parte del flujo de trabajo ya autorizado.
- Agrupar los cambios por tema de modificación: todo lo relacionado con un mismo tema (código, este documento y, si corresponde, API) queda en un commit, y el mensaje del commit debe ser informativo. Los micro-commits por cambio puntual no se usan.
- La IA decide cuántos commits hacen falta según los temas, aunque el usuario diga "haz un commit" o "un commit de todos los cambios": si hay varios temas, se hacen varios commits (nunca se fuerza todo en uno).
- Commit y push van juntos. En cuanto se crea el commit se hace push inmediatamente, sin pedir confirmación adicional.
- La IA debe actualizar PROYECTO.md antes de hacer el commit: toda modificación de código debe quedar reflejada (secciones/API) y ese cambio a PROYECTO.md debe incluirse en el mismo commit.
- Al pedir un commit, la IA no modifica el código (.ino, .h, .cpp). No "arregla" el código de los archivos para dejarlo bien antes de commitear: si algo está mal, lo señala y espera orden. Sí puede, sin permiso: modificar comentarios en .ino/.h/.cpp/.md, mover código de posición, ajustar las líneas vacías y ajustar la sangría (reordenar), pero nunca cambiar el código.

### Procedimiento cuando el cambio necesita más de un commit

Si el cambio necesita más de un commit, seguir este procedimiento:

1. Crear o actualizar `COMMITS_PENDIENTES.json` (junto a PROYECTO.md, en la raíz del proyecto) con los commits pendientes (futuros): la lista de archivos de cada commit y su mensaje.
2. Ir haciendo los commits uno a uno, según la regla de agrupación por tema; por cada commit hecho se borra su entrada del `.json`.
3. Cuando el `.json` quede sin entradas, se elimina el archivo.
4. Actualizar PROYECTO.md (ese cambio entra en el mismo commit).
5. Dar recomendaciones finales, brevemente.

### Checkpoint entre sesiones

- El `.json` es un punto de control (checkpoint): si los commits se detienen por cualquier razón, la siguiente sesión se retoma leyendo PROYECTO.md + el `.json` (el trabajo aún listado) y se continúa donde se quedó. Por eso se mantiene al día tras cada commit y no se borra hasta haber completado todo.
- El `.json` admite dos tipos de entrada:
  - `pendientes`: commits ya decididos (trabajo terminado), listos para hacer; es la lista que consume el procedimiento de arriba.
  - `en_curso`: como máximo uno; describe el trabajo en progreso (WIP): archivos, qué falta y el siguiente paso.
- Ciclo del WIP: al terminar una sesión con trabajo a medias se registra o actualiza `en_curso`; al retomarlo se lee y se continúa; al terminarlo se convierte en entradas de `pendientes` y se borra `en_curso`. Cuando el `.json` no tenga ni `pendientes` ni `en_curso`, se elimina.

### Convenciones de código

- El nombre de las variables dentro de las funciones y los parámetros debe ser completo, en vez de `cx` usar `centerX`.
- Las constantes se declaran en `Config.h` y cada clase/API las reexpone con un alias en su `.h`; así el valor se corrige en un único lugar y no hay que recorrer todos los `.h`.
- Al trabajar en una clase/API, modificar solo la que se está trabajando, a menos que se indique lo contrario de forma directa.
- Los miembros de cada clase se agrupan en secciones rotuladas: `// Constantes de Config`, `// Constantes`, `// Variables comunes de las ventanas` y `// Variables locales` (y, en `.cpp`, `// Funciones comunes de las ventanas` / `// Funciones locales`).
  - **Variables comunes de las ventanas:** las que comparten nombre y función entre las clases ventana (`_done`, `_clear`, `_blink`, `_selected`, `_lastSelected`, `_lastScroll`, `_scroller`, `_repeat`, `_isScrolling`, `_card`, `_pause`). Van primero.
  - **Variables locales:** las propias de la clase; van después de las comunes y ordenadas por uso.
  - El orden de declaración del `.h` manda: la lista de inicialización del constructor en el `.cpp` debe respetar ese orden.
  - Los bloques se separan con un separador de 3 líneas (`// ===`, rótulo, `// ===`) y 5 líneas vacías entre el cierre (`// Fin — <rótulo>`) y la apertura del siguiente.

### Ciclo de sesión

- La IA no compila sin permiso explícito del usuario. El usuario compila por su cuenta (compilar la IA y después el usuario demora el flujo de trabajo).
- La IA mantiene la coherencia interna del código (firmas, `#include`, declaración/definición) y, si no pudo compilar, lo indica al terminar para que el usuario verifique en Arduino IDE. No se commitea código que no compila, salvo orden expresa (p. ej. un WIP).
- Después de compactar la sesión, la siguiente sesión debe volver a leer las reglas.

### Frase para iniciar una nueva sesión

Para no empezar desde cero, **copiar y pegar tal cual** el siguiente bloque en la
nueva sesión (no hay que escribir nada más):

```
Ayudeme con proyecto de recrear el Juego Snake II de los Nokias.
Trabaja en el proyecto Snake_II-OLED_128x64-I2C (ESP32-S3, Arduino IDE) ubicado en
C:\ESP32S3\Snake_II-OLED_128x64-I2C.
```

---

## 1. Estado actual del proyecto

En desarrollo. `.ino` = wiring: define los globales (`Display`, `Buttons`, `Sound`, `Storage`; en `Globals.h`) y crea el `Engine`, dueño de las ventanas (`Boot`, `Legend`, `Menu`, `MenuDifficulty`, `MenuSound`, `MenuCredits`, `Game`). Constantes compartidas en `Config.h`.

**Arquitectura:**
- **`Storage` (§22):** almacén global de estado (mejor puntaje, sonido, dificultad); datos puros sin `begin()`/`update()`; instancia global `storage`. **Integración parcial:** `Menu` lee `storage.bestScore()` (pie "Best"), `Game` lee el récord y la dificultad y escribe el récord (al batirlo y al subir de nivel). `MenuDifficulty` (su `_difficulty`) y `Sound` (su `_enabled`) siguen con copias propias.
- **`Engine`:** despachador; posee las ventanas (no globales, no anidadas); llama `begin()` de la entrante al cambiar de estado. `loop()`: única lectura de botones + reposo (§26).
- **Render sin `clear()` global:** cada ventana limpia solo en su primer frame. `MenuDifficulty`/`MenuSound` son capas sobre el `Menu` (sustituyen la banda de rombos); al salir hacen `changeState(MENU)` — hoy `begin()` completo porque `Menu::restoreDiamondBand()` sigue comentada (§11).
- **Flujo:** `Boot` → `Legend` → `Menu`. La `Legend` solo aparece al arrancar. Submenús activos: `MenuSound` (On/Off), `MenuDifficulty` (nivel 1..10) y `MenuCredits` (roll vertical).
- **Partida activa:** `Game` (con `Food` normal + especial) está **reactivado e integrado** (§11/§16): el `Menu` abre `NEW`/`CONTINUE` (con `begin(showContinue, selected)`), el `Engine` despacha la ventana del juego y, al terminar (`done()`), vuelve a `MENU_CREDITS` si hubo muerte o a `MENU` si se salió con pausa. `MenuSound`/`MenuDifficulty`/`MenuCredits` se abren al confirmar su opción con `ACTION_RIGHT`. **Pendientes:** la dificultad del `MenuDifficulty` no llega al juego (`MenuDifficulty` guarda `_difficulty` local, `storage.setDifficulty` no se llama, `Game::setDifficulty` es no-op); los créditos abiertos desde el menú tras una muerte muestran "GAME OVER" porque `Engine` pasa `_game.isGameOver()` como `fromGame`.

**Clases extraídas de `Game`:** `Food` (alimento normal/especial), `Snake` (lógica + dibujo propio `print`), `Sprite.h` (namespaces `Sprite`/`Special`/`Logo`), `Timer.h` (reloj 64 bits; migración desde `millis()`).

**Características:** dificultad vía `Storage` (`Game::begin` usa `speedFor(storage.difficulty())`); puntaje = nivel de dificultad (comida normal) o ×10 (comida especial), `uint16_t`; al llenar el tablero se sube de nivel (`spawnFood` escribe `storage.setDifficulty(storage.difficulty() + 1)`) o muere si ya está en el máximo; al morir, `die()` marca `_win = _score > storage.bestScore()`, guarda el récord y termina (`_done=true`), y el `Engine` entra en `MENU_CREDITS` (tarjeta final `THANKS`/puntaje).

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
| `Config.h` | Constantes compartidas (namespace `Config`, §19): pines, geometría/regiones, `Difficulty`, `Version`, `Credits`, `Legend`, `Diamond`, `Button`, `Scroller`, `MenuStrip`, `DefaultTimer`, `Power`, `Food`. Sin `#define` para valores. |
| `Globals.h` | `extern` globales: `display`, `buttons`, `sound`, `storage` (definidos en el `.ino`). No declara `Buzzer` ni ventanas. |
| `Buttons.h/.cpp` | Clase `Buttons`: debounce; `heldDown`/`pressed`/`released`/`anyHeld`/`anyPhysical`/`ignoreUntilRelease`; `MAX_BUTTONS` = contador del enum `Button`; `isSet` declarado en `.h` y definido en `.cpp`. Completa. |
| `Boot.h/.cpp` | Clase `Boot`: Header "Press any button / to start" parpadeando (`PERIOD=500`, `OFF=20%`), Body con logo `Logo::LOGO` (80×48) vía `drawBitmap(...,true,false)`. `begin()` suena `SFX_BOOT`; cualquier botón termina (sonido según botón). Animación de bandas anterior comentada. Completa. |
| `Legend.h/.cpp` | Clase `Legend`: pad MOVE (4 flechas) + 4 rombos ACTION que parpadean de a uno en ciclo lento (`NEXT=2500ms`, `PERIOD=100ms`/`OFF=50%`) y texto de la función del rombo activo en el pie (`Scroller` propio, entra deslizándose). Cualquier botón cierra con sonido (MOVE=`CLICK`, `ACTION_UP`=`BACK`, `ACTION_RIGHT`=`CONFIRM`). Completa. |
| `Scroller.h/.cpp` | Clase `Scroller` (§14): scroller de 1 bit **en 4 direcciones** (`Draw::Direction`); bloque de **varias líneas** con color de letra/fondo y `size` por línea; cada línea ocupa el **ancho completo** con el texto centrado. API mínima: `begin(dir, EndMode)` + `setRow(y)` (horizontal) o `setVertical(to, from)` (vertical); sin pausas internas (`STAY` reposa en el destino, `EXIT` desaparece). El dibujo vertical se recorta a la banda del Body (el Header queda intacto). `_strip` por valor (sin memoria dinámica). Usada por `Menu`, `Legend` y `MenuCredits`. (Reemplaza a las retiradas `ScrollerH`/`ScrollerV`.) |
| `Menu.h/.cpp` | Clase `Menu`: scroller de 1 bit, rombos/triángulos de posición, navegación con repetición (`ButtonRepeat`) y anti-entrada `_confirm`/`ignoreUntilRelease`. `begin(showContinue, selected)` (pinta solo con `showOptions()` para OPT_DIFFICULTY/OPT_SOUND), `navigate()` (salta `OPT_CONTINUE` oculto), `action()` (`ACTION_UP`→Continue/New, `ACTION_RIGHT`→`_confirm`+`SFX_CONFIRM`), `blinkOption()` (asigna `_done` al confirmar), `nextOption()`. API anterior (`setOptions`, `setContinueAvailable`, `setBestScore`, `setSelected`, `setTitle`, `setShowFooter`, `confirm()`, `restoreDiamondBand()`, `holdButtons()`) comentada en `Menu.h`. |
| `MenuDifficulty.h/.cpp` | Clase `MenuDifficulty`: selector nivel 1..10 `< N >` con repetición al mantener. Refactorizada al estilo `MenuSound` (`firstPrint()`/`clear()`/`nextOption()`/`blinkOption()`; `_selected`/`_lastSelected`/`_clear`; `Draw::triangle`; `Blink::changed`/`isVisible`) y activa en el `Engine` (`MENU_DIFFICULTY`); entrada desde `Menu` pendiente. Capa sobre el `Menu` (banda 45..53). `difficulty()` → `Game::setDifficulty`. |
| `MenuSound.h/.cpp` | Clase `MenuSound`: selector On/Off con una flecha sobre la misma banda. Activa (`MENU_SOUND`); entrada desde `Menu` pendiente. Centra a mano (`Config::Screen::WIDTH`, `strlen*6`). Aplica con `Sound::setEnabled`. |
| `MenuCredits.h/.cpp` | Clase `MenuCredits` (§10.3): roll vertical automático de todo `Config::Credits::CREDITS` (constante `COUNT` derivada; modelo línea a línea en refactor: `startStay()`/`startExit()`, dos vuelos STAY/EXIT del `Scroller`, pausa externa `PAUSE_MS`=4 s). **Sin tarjeta final** (`drawCard()` comentado; el último tramo se repite hasta que el usuario salga). Cualquier botón cierra (con su sonido) y `buttons.anyHeld()` corta la música `SFX_DRAGON_BALL`/`SFX_PIRATES` en bucle. Título del Header `CREDITS`/`YOU WIN`/`GAME OVER` según `fromGame`/`winner` (el `Engine` pasa `_game.isGameOver()`/`_game.win()`). El `.cpp` usa solo los renombres de clase, sin `Config` directo. Completa (en refactor). |
| `Game.h/.cpp` | Clase `Game`: ventana del juego (estados `START`/`PLAY`/`PAUSE`/`GAME_OVER`). Coordina dificultad/velocidad (`speedFor`, público), botones (MOVE→`Snake::turn`), `Snake::step`, doble comida (`_food` normal + `_specialFood`), puntaje, sonidos, overlays y volcado (`firstPrint`/`printScore`). `begin(newGame)`: `true` → reset + `START` (cuenta atrás 3-2-1), `false` → `PAUSE` con la partida guardada. `die()` marca `_win` (récord), guarda el récord y termina. `update()` usa `Ticker _timer` + `Blink _blink`. |
| `Food.h/.cpp` | Clase `Food`: estado (posición, tipo `Type{NORMAL,SPECIAL}`, tiempo, puntaje), `spawn(Type, const Game&)` en celdas libres (dos recorridos sin buffer, consulta `Game::occupied`), dibujo del rombo (`Draw::diamond`) y del sprite `Special::SPECIAL_FOOD` (`Display::drawBitmap`), temporizador (`lessTime()` deciende el contador; `printTime()` lo muestra en el Header y auto-limpia al 0). Renombra `CELL`/`FOOD`/`FOOD_WIDTH`/`FOOD_HEIGHT` desde `Special::`; el `.cpp` no referencia `Config` ni `Sprite` directamente. `#include "timer.h"` (minúsculas). Completa. |
| `Snake.h/.cpp` | Clase `Snake`: lógica (buffer circular, dirección + giro pendiente sin reversa, paso con wrap, colisión, comer/crecer, elección de sprites) + dibujo propio (`print(Food, Food)`, como `Food`; usa la `Display` global y borra la cola liberada repintando sus píxeles en negro). Incluye `Food.h`; `step(Food, Food)`/`headPart(Food, Food)`/`print(Food, Food)` **copian los `Food` por valor**. `Result::MOVED`/`ATE_FOOT` (typo)/`ATE_SPECIAL`/`DIED`. Sin `Sound`. Completa. |
| `Engine.h/.cpp` | Clase `Engine` (antes `App`): despachador; posee las ventanas `_boot`, `_menu`, `_menuDifficulty`, `_menuSound`, `_menuCredits`, `_game`, `_legend`; `changeState()` llama `begin()` de la entrante; `changeState(MENU)` calcula `showContinue`/`selected` y llama `_menu.begin(showContinue, selected)`; `changeState(MENU_CREDITS)` llama `_menuCredits.begin(_game.isGameOver(), _game.win())`; al terminar el juego (`NEW`/`CONTINUE` + `done()`), vuelve a `MENU` o `MENU_CREDITS` según `isGameOver()`. `inGame()`/`inCredits()`/`sleepBlocked()` (reposo bloqueado en partida y en el roll de créditos). Comentarios de `Engine.h` §36-40 y `Engine.cpp` 63-68 **desactualizados**. Completa. |
| `Snake_II-OLED_128x64-I2C.ino` | Wiring: define servicios globales y `storage`; crea `Engine engine;` y `SleepManager sleepManager(engine);`. `setup()`: `display.begin()`, `buttons.begin()`, `sound.begin()`, `engine.begin()`, `sleepManager.begin()`. `loop()`: `buttons.read()` (única lectura), `if (!sleepManager.update()) return;` (si `false` = durmiendo/aviso, corta el frame), `engine.update()`, `engine.print()`, `sound.update()`, `display.show()`. |
| `Buzzer.h/.cpp` | Clase `Buzzer`: hardware, 1 tono no bloqueante (LEDC). Propiedad de `Sound` por valor. Completa. |
| `Sound.h/.cpp` | Clase `Sound`: secuencias de efectos sobre su `Buzzer` interno; `play(effect, loop)` con bucle (música de fondo); canciones `SFX_PIRATES` y `SFX_DRAGON_BALL`; `setEnabled`. Completa. |
| `Storage.h/.cpp` | Clase `Storage`: mejor puntaje, sonido, dificultad (defaults de `Config`; clamp en `setDifficulty`). Datos puros, sin `begin`/`update`/hardware. Instancia global `storage` (§22). Completa; **pendiente de integrar**. |
| `Sprite.h` | Namespaces de datos (header-only): `Sprite` (sprites de la serpiente 4×4: `HEAD_*`, `BELLY_*`, `TAIL_*`), `Special` (comida especial 8×4: `SPECIAL_FOOD`, `specialPixel`) y `Logo` (logo de arranque `LOGO` 80×48). `Boot.h` alias `Logo::LOGO`; `Food.h` alias `Special::SPECIAL_FOOD*`. |
| `Timer.h/.cpp` | Reloj de 64 bits (`nowMs()`, `Stopwatch`, `Ticker`) sobre `esp_timer_get_time()`. Definiciones en el `.cpp`. |
| `ButtonRepeat.h/.cpp` | Helper repetición al mantener (§23): paso inmediato + uno cada `TICK` tras `DELAY`. Lo usan `Menu` y `MenuDifficulty` (`_repeat`). |
| `Blink.h/.cpp` | Helper parpadeo (§24): `Stopwatch` + `_last`. API `start()`, `isVisible()`, `changed()`. Lo usan `Boot`, `Legend`, `Menu`, `MenuDifficulty`, `MenuSound`. |
| `Draw.h/.cpp` | Namespace `Draw`: `triangle(dir,cx,cy,show)`, `diamond(cx,cy,show)`; sin estado; `dir` 1↑ 2→ 3↓ 4←. |
| `PROYECTO.md` | Este documento. |
| `LICENSE.md` | PolyForm Noncommercial 1.0.0 (source-available; prohíbe venta/uso comercial; conservar línea `Required Notice:`). |
| `THIRD_PARTY_NOTICES.md` | Atribuciones (Adafruit BSD-3, core Arduino-ESP32 LGPL-2.1, ESP-IDF Apache-2.0); el repo no distribuye código/artwork de terceros. |

Notas:
- Arduino solo compila el `.ino` del sketch; el respaldo quedó como `.txt`.
- Refactor de servicios globales: las clases ya no reciben `Display`/`Buttons`/`Sound`/`Buzzer` por constructor (los usan vía `Globals.h`, incluido solo en los `.cpp`). `Sound` es la **única** clase que usa `Buzzer`, y lo **posee por valor** (`Sound(uint8_t pin)`, inicializado en `begin()`); no hay `Buzzer buzzer;` global. Constructores con parámetros de configuración: `Menu()` sin args (el récord vive en `Storage`), `MenuCredits()`, `Food()` sin args (posiciones vía `spawn`), `Sound(pin)`; `Game()`/`Boot()`/`Legend()` sin parámetros.

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
- **Retornos `uint8_t`** para anchos/altos (clamp 255). `Display::centerX()` devuelve **`int16_t`** (centrado horizontal; antes `uint8_t`).
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
| `bool heldDown(index)` / `pressed(index)` / `released(index)` | Acceso único por botón: `heldDown` = mantenido (antes `hold`/`state()`), `pressed` = pulso del ciclo, `released` = liberación. Los 24 getters con nombre se eliminaron. |
| `bool anyHeld()` | Algún botón confirmado (sujeto a la supresión `ignoreUntilRelease()`). |
| `bool anyPhysical()` | Algún botón físicamente presionado (`_rawButtons`, sin debounce ni supresión). La usa el reposo (§26). |

### Diseño del debounce (estados agrupados en bytes)
- 8 botones en bytes (`uint8_t`, 1 bit por botón, HIGH = presionado): `_rawButtons` (físico), `_buttons` (confirmado), `_pressed`/`_released` (eventos de 1 ciclo). `_lastButtons` eliminado (el evento se detecta contra el estado confirmado previo).
- `bool isSet(uint8_t states, uint8_t boton)` → `states & (1 << boton)`: **declarado en `Buttons.h`** (`static bool`, **sin `inline`**) y **definido en `Buttons.cpp`** (quitar el `inline` resolvió el *undefined reference*).
- Cambio físico por bit (`raw ^ _rawButtons`); instante por botón; estado nuevo tras `_buttonDelay` ms estables. Almacenamiento: 4 bytes (antes 4 `bool[8]` + 1 byte).

### Anticonflicto MOVE (eliminado)
El antiguo bloque `MOVE_MASK` (con 2+ bits MOVE confirmados a la vez se anulaban todos) se **eliminó** de `read()`, junto con la constante `MOVE_MASK` de `Buttons.h`: hoy los MOVE simultáneos se tratan como cualquier otro botón (cada uno confirma su propio bit).

---

## 7. Clase `Menu` — API

`Menu.h/.cpp`.

### Constructor
```cpp
Menu();
```
Sin parámetros (el récord vive en `Storage`); el pie muestra `storage.bestScore()` (§22). `_scroller()` por defecto. Texto del pie de `Config::Version::VERSION` (§19).

### Métodos
| Método | Descripción |
|--------|-------------|
| `void begin(bool showContinue = false, int8_t selected = OPT_NEW)` | Fija lista visible (5 con Continue / 4 sin él) y selección; reinicia flags (`_lastScroll`, `_confirm`, `_done`, `_clear`); `_blink.start()`; `buttons.ignoreUntilRelease()` (anti-entrada); `_repeat` listo. Solo para `OPT_NEW`/`OPT_CONTINUE`/`OPT_CREDITS` pone `_clear` (repintado completo en el primer `print()`); para `OPT_DIFFICULTY`/`OPT_SOUND` llama `showOptions()` directo (sin `clear()`, la banda del menú se conserva). |
| `void update()` | `if (_done) return;` luego `navigate()`, `action()`; si el `Scroller` vuela (`_lastScroll`) lo avanza y **congela el resto del frame**; al terminar, `_blink.start()`. |
| `void print()` | `if (_done) return;` + `firstPrint()` (primer frame o `forceRedraw`: `clear()` + título, cuadro, texto, marcadores, pie), `nextOption()`, `_scroller.print()`, `blinkOption()`. |
| `void showOptions()` | Borra la banda entre cuadro y pie y dibuja marcadores de no seleccionadas + rombo de la seleccionada (omite `OPT_CONTINUE` si está oculta). |
| `int8_t selected()` | Opción seleccionada (enum `Option`). |
| `bool done()` | `true` cuando pide salir: lo pone `blinkOption()` al confirmar (`_confirm` → `_done = true`). |

**API en pausa (comentada en `Menu.h`):** `setOptions`, `setContinueAvailable`, `setBestScore`, `setTitle`, `setShowFooter`, `setSelected`, `confirm()`, `restoreDiamondBand()`, `forceRedraw()`.

### Internos (privados)
| Método | Descripción |
|--------|-------------|
| ~~`holdButtons()`~~ | **Eliminado**: la anti-entrada es `buttons.ignoreUntilRelease()` en `begin()`. |
| `void navigate()` | `MOVE_LEFT`/`MOVE_RIGHT` con `_repeat.step()` (paso inmediato + cada `TICK` tras `DELAY`); salta `OPT_CONTINUE` si `_showContinue` es `false`; `SFX_CLICK` al mover; límites `OPT_NEW`/`OPT_COUNT-1`. |
| `void action()` | `ACTION_UP` → salta a `OPT_CONTINUE` (si visible) o `OPT_NEW` (`SFX_BACK`); `ACTION_RIGHT` → `_confirm = true` (`SFX_CONFIRM`; el `_done` lo pone `blinkOption()`). |
| `void blinkOption()` | Si `_confirm`: `_done = true; _confirm = false;`. Si no: guards `if (!_scroller.done()) return;`, `else if (_lastSelected != _selected) return;`, `else if (!_blink.changed(PERIOD, OFF)) return;`. Repinta texto + rombo (`toggleText` + `toggleDiamond`) con `visible = _done \|\| _blink.isVisible(PERIOD, OFF)`. |
| `void nextOption()` | Si cambió la selección: restaura marcadores anteriores (`toggleDiamond(_lastSelected,false)` + `toggleTriangle(_lastSelected,true)`), coloca los nuevos, arranca el `Scroller` (`begin(DIR_LEFT`/`DIR_RIGHT, STAY)` + `addText(OPTION[_selected], TEXT_12x16, false, true)` + `setRow(BOX_TOP-1)` + `startSlide()`); marca `_lastScroll`. |
| `void toggleText(bool show)` | Pinta/borra el texto de la opción en el cuadro (`drawText`, `BOX_TOP`, `TEXT_12x16`). |
| `void toggleDiamond(int8_t, bool)` / `void toggleTriangle(int8_t, bool)` | Pinta/borra el marcador (centro `(i+1)·_space`, restando 1 si Continue oculta): `Draw::diamond` en `DIAMOND_Y` / `Draw::triangle` en `TRIANGLE_Y`; `toggleTriangle` sale si el índice es `OPT_COUNT`. |
| ~~`holdRepeat(uint8_t)`~~ | **Eliminado**: extraído a `ButtonRepeat` (§23, `_repeat`). |

### Las opciones "Difficulty" y "Sound" son ventanas
`Menu::confirm()` (el método que devolvía la opción confirmada) sigue comentado, pero las ventanas **ya se abren**: confirmar con `ACTION_RIGHT` pone `Menu::done()` y el `Engine` despacha por `selected()` `OPT_DIFFICULTY`→`MENU_DIFFICULTY`, `OPT_SOUND`→`MENU_SOUND`, `OPT_CREDITS`→`MENU_CREDITS` (§11). Al confirmar, el `Engine` abre `MenuDifficulty`/`MenuSound` (hermanas, §11) en vez de editar inline. Ambas entran sobre el `Menu` ya dibujado, sustituyen solo la banda de rombos (45..53); al salir, el `Engine` llama `_menu.begin(showContinue, selected)` (con `selected` = la opción que originó la ventana), que repinta solo la banda vía `showOptions()` (sin `clear()`). La edición inline anterior (`beginSoundEdit`/`beginDifficultyEdit`, `drawSoundSelector`/`drawDifficultySelector`) se eliminó.

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
- **Animación (`Scroller`, §14):** 1 instancia (12x16). Cada opción se compone a **ancho completo** (fondo blanco, texto negro centrado) con `addText`, `MOVE_RIGHT` → entra por la derecha, `MOVE_LEFT` → por la izquierda; el destino es la posición de reposo (`setRow(BOX_TOP-1)` + `STAY`, el Scroller calcula borde+telón → `x=0`), así que entra y **queda quieta** (`DIR_RIGHT`: `-WIDTH-telón..0`; `DIR_LEFT`: `WIDTH+telón..0`), 1 columna cada `ANIM_H` (4 ms). En `Menu` la transición está **activa** (`nextOption()` + `startSlide()`); mientras `Scroller::update()` devuelve `true`, `Menu::update()` queda congelado.
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
- Pie: banda blanca `FOOT_TOP-1..63` con `BTN_FUNC[_btn]` `TEXT_6x8` centrado en `FOOT_TOP=56`: `Btn1`="Btn1 / Back", `Btn2`="Btn2 / Select / Pause", `Btn3`="Btn3 / None", `Btn4`="Btn4 / None". Entra deslizándose (`Scroller` propio `_scroller`: `begin(DIR_LEFT, STAY)` + `addText(BTN_FUNC[_btn], TEXT_6x8, false, true)` + `setRow(FOOT_TOP-1)` + `startSlide()`, desde la derecha). Mientras vuela, el ciclo se congela. `PIE_TOP=57` anterior comentado.
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

Las secuencias `SEQ_*` son **miembros estáticos** de la clase, ahora **`static constexpr` e inline en `Sound.h`** (antes `static const` declarados en `Sound.h` y definidos en `Sound.cpp`), indexadas por una **única tabla `EFFECTS[]` de `{const Note* notes, uint16_t len}`** (`uint16_t`: canciones largas; antes `uint8_t`, tope 255 notas) — orden = enum `Sfx`, `SFX_NONE` incluido con secuencia vacía: reemplaza a las constantes `LEN_*` (largo por `sizeof`) y al `switch` de `play()`, que ahora lee `EFFECTS[effect]`. `EFFECTS[]` también es `static constexpr` en el header. Detalle de implementación: la API pública no expone `Note`, `Seq` ni tablas.

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
  SFX_NEW_BEST, SFX_BOOT, SFX_PIRATES, SFX_DRAGON_BALL,
  SFX_COUNT   // SFX_COUNT = tamaño de EFFECTS
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
| `SFX_PIRATES` | Canción "Pirates of the Caribbean" (He's a Pirate), completa | 205 notas `{freq, durMs}` en `Sound.cpp` (`SEQ_PIRATES`); con enlace final (G4-E4) que devuelve el bucle al motivo inicial (E4); sin uso todavía. Fuente: repo de xitanggg (MIT, ver `THIRD_PARTY_NOTICES.md`), salvo el enlace final (propio). |
| `SFX_DRAGON_BALL` | Tema de apertura de Dragon Ball, en bucle (música del roll de créditos desde el menú) | ~90 entradas `{freq, durMs}` en `Sound.cpp` (`SEQ_DRAGON_BALL`): melodía ligada, cada nota mantiene su duración completa (`2*dur`, sin el silencio intermedio del sketch original `tone(dur);delay(dur)`, que sonaba picado) + respiro final `{0, 1000}`, ~28 s. Transcripción del sketch de Francisco Roco (www2.udec.cl/~franciscoroco/dragonBall.txt), sin licencia explícita en la página (ver `THIRD_PARTY_NOTICES.md`). |

Paleta del `GameBuzzer` original (salvo las canciones `SFX_PIRATES` y `SFX_DRAGON_BALL`, cuyo origen se cita en sus filas y en `THIRD_PARTY_NOTICES.md`).

### Métodos
| Método | Descripción |
|--------|-------------|
| `void begin()` | `_buzzer.begin()` (adjunta LEDC) y luego `stop()`. Único lugar que inicializa el `Buzzer`. |
| `void setEnabled(bool)` / `enabled()` | Activa/desactiva (opción "Sound"); al desactivar corta el efecto en curso. |
| `void play(Sfx, bool loop = false)` | Arranca la secuencia (no bloqueante). `loop=true` la repite en bucle hasta `stop()`/otro `play()` (música de fondo de los créditos). |
| `void update()` | Avanza a la siguiente nota (llama `_buzzer.update()` antes). Una vez por `loop()`. |
| `void stop()` | Corta el efecto y silencia. |
| `bool playing()` | `true` mientras suena un efecto. |

En el `.ino`: `Sound sound(Config::Pin::BUZZER);`; `sound.begin()` en `setup()`, `sound.update()` en `loop()`.

---

## 10.3 Ventanas del menú: `MenuDifficulty` y `MenuSound`

Las opciones **"Difficulty"** y **"Sound"** son ventanas del `Engine` (como `MenuCredits`, §10.4; históricamente `SoundWindow` y luego edición *inline* en `Menu`, ambas eliminadas). `Menu` las devuelve en `confirm()` y el `Engine` abre la correspondiente (`OPT_DIFFICULTY` → `MENU_DIFFICULTY`, `OPT_SOUND` → `MENU_SOUND`). **Estado actual:** ambas **fuera del aislamiento** y ya se abren desde el `Menu`: confirmar la opción con `ACTION_RIGHT` pone `Menu::done()` y el `Engine` despacha por `selected()` `OPT_DIFFICULTY`/`OPT_SOUND`/`OPT_CREDITS`. `Menu::confirm()` (el método) sigue comentado.

Esqueleto común (`begin`/`update`/`print`/`done`), banda de dibujo **45..53** (la de los rombos del `Menu`) y regla: **no borran la pantalla**, solo sustituyen esa franja. Al salir, el `Engine` llama `changeState(MENU)`, que invoca `_menu.begin(showContinue, selected)` con `selected` = la opción que originó la ventana; como esa opción es `OPT_DIFFICULTY`/`OPT_SOUND`, `begin()` **no** hace `clear()` y llama `showOptions()` directo (repinta solo la banda). `Menu::restoreDiamondBand()` sigue comentada.

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
| `uint8_t difficulty()` | Nivel **confirmado**; lo lee el `Engine` en `changeState(NEW/CONTINUE)` → `Game::setDifficulty`. **Pendiente:** `Game::setDifficulty` es no-op y nadie llama `storage.setDifficulty(...)`, así que el nivel elegido aquí no afecta al juego (§16/§22). |
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

### `MenuCredits` — roll de créditos

`MenuCredits.h/.cpp` (antes: 3 entradas navegables con 2 `Scroller` sincronizadas; luego roll por bloques con fase CARD; **ahora en refactor**: modelo línea a línea). Ventana de pantalla completa: Header 0..15 con el título 12x16 centrado y Body 16..63 como banda del roll. Cada tramo de filas se desliza con el `Scroller` (§14) propio; el antiguo modelo de bloques (`computeBlocks`/`addBlockLines`/`Phase::ROLL|CARD`) está **comentado**.

**Contenido** = `Config::Credits::CREDITS` completo (§19), iterado con `_line` y la constante `COUNT` (`static constexpr`, derivada del array). **Bloque** = tramo de filas no vacías consecutivas (la fila vacía es separador y se salta con `_line += i + 1`). En cada bloque la primera fila es un **Título** (barra blanca: `addText(text, TEXT_6x8, i != 0, i == 0)`) y el resto **nombres** (texto normal centrado).

**Movimiento (estilo cine, bloque a bloque, en DOS VUELOS del `Scroller`):** `startStay()` arma el bloque actual con `_scroller.begin(Draw::DIR_UP, Scroller::EndMode::STAY)` + un `addText()` por fila y lo arranca con `startSlide()` desde `setVertical(HEIGHT - 1, _rest)`, donde `_rest = BODY_TOP + (BODY_H − _blockH)/2` (reposo centrado en el Body). **1er vuelo (STAY):** `update()` lo detecta cuando `_scroller.update()` devuelve `false` (`_isScrolling=false`, `_pause.start()`). **Pausa externa** de `PAUSE_MS` (4 s, `Config::Credits::PAUSE_MS`). **2º vuelo (EXIT):** `startExit()` cambia el modo con `setEndMode(EXIT)` y relanza `setVertical(_rest, BODY_TOP)` (sin recargar las líneas: continúa desde el reposo sin salto visual) hasta que el bloque quede por encima del Body (el Scroller recorta a `[BODY_TOP, HEIGHT)`). Al terminar, `startStay()` compone el **siguiente** bloque. **No hay tarjeta final:** `drawCard()` está enteramente comentado; cuando `_line >= COUNT`, `startStay()` no arma nada y el último bloque se repite en bucle (pausa + salida) hasta que el usuario salga.

**Salida:** cualquier botón termina (`MOVE_*`/`ACTION_DOWN`/`ACTION_LEFT` → `SFX_CLICK`, `ACTION_UP` → `SFX_BACK`, `ACTION_RIGHT` → `SFX_CONFIRM`); `buttons.anyHeld()` corta la música (`sound.stop()`). **Música:** `begin(fromGame, winner)` arranca `sound.play(fromGame && winner ? SFX_DRAGON_BALL : SFX_PIRATES, true)` en bucle (§10.2). Título del Header: `CREDITS` (menú), `YOU WIN` (muerte con récord) o `GAME OVER` (muerte sin récord), según `fromGame`/`winner`.

**Reposo (§26):** `Engine::inCredits()` devuelve `true` mientras la ventana está abierta (`MENU_CREDITS` y `!done()`), y `sleepBlocked()` lo bloquea; `rolling()` quedó comentado.

| Miembro | Descripción |
|---------|-------------|
| `MenuCredits()` | `_done=true`, `_stay=true`: inerte hasta `begin()`. |
| `void begin(bool fromGame=false, bool winner=false)` | `_clear=true`, `_done=false`, `_stay=true`, `_line=0`; `buttons.ignoreUntilRelease()`; `startStay()` (compone el primer tramo y arranca su entrada); `sound.stop()` + música en bucle (`SFX_DRAGON_BALL` solo si `fromGame && winner`, si no `SFX_PIRATES`). |
| `void update()` | Cualquier botón → salir (`_done=true` con su sonido); si no, gestiona el ciclo `_isScrolling`/`_stay`: vuelo de entrada → pausa `PAUSE_MS` → `startExit()` → vuelo de salida → siguiente tramo. |
| `void print()` | `firstPrint()` + `_scroller.print()` (el `drawRoll()`/`drawCard()` del modelo anterior quedó comentado). |
| `void firstPrint(bool forceRedraw=false)` | Primer frame (o forzado): `clear()` + título 12x16 centrado en el Header (`CREDITS`/`YOU WIN`/`GAME OVER` según `fromGame`/`winner`). |
| `bool done()` | Salida pedida. (`rolling()` comentado.) |
| `startStay()` / `startExit()` | Compone el tramo actual (filas hasta la primera vacía) y arranca su entrada STAY centrada / relanza el vuelo EXIT desde el reposo hasta `BODY_TOP`. |
| `drawRoll()` / `drawCard()` | Delegan en `_scroller.print()` / **comentado** (sin tarjeta final). |
| `_fromGame` / `_winner` | `true` = se llegó con partida terminada / con récord. El `Engine` pasa `_game.isGameOver()` y `_game.win()`. **Defecto conocido:** abrir los créditos desde el menú tras una muerte también lleva `fromGame=true` → título "GAME OVER" en vez de "Credits". |
| `_line` / `_blockH` / `_rest` | Índice de la siguiente fila del array / alto del tramo actual en px / fila de reposo (centro del Body). |
| `_isScrolling` / `_stay` / `_pause` / `_paused` / `_exiting` | Bandera "vuelo en curso" / bandera "toca pausar o el vuelo de salida" / `Stopwatch` de la pausa externa / residuales del modelo anterior (sin uso activo). |
| `_scroller` | `Scroller` del slide del tramo actual. |

Constantes de clase: de `Config::Screen` (`WIDTH`, `HEADER_TOP`, `BODY_TOP`, `BODY_H`, `CHAR_H`) más `PAUSE_MS` (=`Config::Credits::PAUSE_MS`, 4 s). **Renombres de `Config`** (el `.cpp` no referencia `Config` nunca): `CREDITS`/`COUNT`, `TITLE_CREDITS`/`TITLE_GAME_OVER`/`TITLE_WINNER`. Los alias `CARD_*` (`THANKS`/`NAME`/`VERSION`/`DATE`) están comentados (sin tarjeta). El centrado horizontal lo hace `display.centerX()` (dentro del `Scroller`, vía `compose()`). Nota: el comentario del `.h` (§60-65) tiene la música al revés respecto del código (`fromGame && winner` → `SFX_DRAGON_BALL`, no `SFX_PIRATES`).

## 11. Clase `Engine` — despachador de ventanas

`Engine.h/.cpp` (antes `App`). **No anida** las ventanas pero las **posee como miembros**: `Boot`, `Legend`, `Menu`, `MenuCredits`, `Game` son clases independientes (hermanas) declaradas como `_boot`, `_menu`, `_menuDifficulty`, `_menuSound`, `_menuCredits`, `_game`, `_legend`. No son globales ni se pasan por referencia. **Estado actual:** todas las ventanas están **activas**, `Game` incluido: `#include`/miembro `_game` y estados `NEW`/`CONTINUE` cableados en `update()`/`print()`/`changeState()`. La vuelta desde `Game` ya **sincroniza** el menú: `changeState(MENU)` calcula `showContinue = !_game.isGameOver() && _game.score() > 0` y `selected`, y llama `_menu.begin(showContinue, selected)`; al terminar el juego con muerte (`isGameOver()`), el `Engine` entra en `MENU_CREDITS` con `_menuCredits.begin(_game.isGameOver(), _game.win())`. `MenuCredits` está **activo**: estado `MENU_CREDITS` cableado (update/print/repaint/changeState) y `OPT_CREDITS` lo abre desde el despacho del `Menu`. `MENU_SOUND`/`MENU_DIFFICULTY` activos también; el despacho del `Menu` (`done()`+`selected()`) abre `NEW`/`CONTINUE`/`OPT_SOUND`/`OPT_DIFFICULTY`/`OPT_CREDITS`. **Comentarios desactualizados:** `Engine.h` (§36-40) y `Engine.cpp` (63-68) aún describen el placeholder.

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
5. **Ventanas "capa" no limpian la pantalla:** `MenuDifficulty`/`MenuSound` son capas sobre el `Menu` ya dibujado; su `begin()` no dibuja nada (su `print()` borra y repinta solo la banda de rombos) y su regreso es `changeState(MENU)`, cuya rama de `MENU` llama `_menu.begin(showContinue, selected)`; como `selected` es `OPT_DIFFICULTY`/`OPT_SOUND`, `Menu::begin()` **no** hace `clear()` y llama `showOptions()` directo (solo la banda). `Menu::restoreDiamondBand()` y la guarda `if (beginWindow)` siguen comentadas.

### Estados internos
```cpp
enum class State : uint8_t {
  BOOT = 0,
  LEGEND,
  MENU,
  MENU_SOUND,
  NEW,
  CONTINUE,
  MENU_DIFFICULTY,
  MENU_CREDITS
};
```

| Estado | Ventana | Notas |
|--------|---------|-------|
| `BOOT` | `Boot` | Arranque: logo + mensaje parpadeante. Al `done()` (cualquier botón) → `LEGEND`. |
| `LEGEND` | `Legend` | Pad MOVE (4 flechas) + 4 rombos ACTION que parpadean rápidos uno a la vez (ciclo lento) con la función del rombo activo en el pie (`Scroller` propio) que entra deslizándose y congela el ciclo mientras se mueve. Cualquier botón la cierra → menú (efecto según botón; `Engine` no añade `SFX_BACK`). Solo tras el arranque. |
| `MENU` | `Menu` | **Activo.** Navegación con MOVE (`SFX_CLICK`); `ACTION_RIGHT` confirma: `blinkOption()` pone `_done` (con `SFX_CONFIRM` al pulsar) y el despacho `done()`+`selected()` abre `OPT_SOUND`→`MENU_SOUND`, `OPT_DIFFICULTY`→`MENU_DIFFICULTY`, `OPT_CREDITS`→`MENU_CREDITS`, `OPT_NEW`→`NEW`, `OPT_CONTINUE`→`CONTINUE`. `ACTION_UP` vuelve la selección a `New`/`Continue` (`SFX_BACK`) sin salir. `Menu::confirm()` sigue comentado. |
| `NEW` | `Game` | **Activo.** `setDifficulty(_menuDifficulty.difficulty())` (no-op) + `begin(true)`, con cuenta atrás 3-2-1 (`START`, `SFX_TICK` por dígito, `Blink` con `PERIOD=1000`/`OFF=25`). Al `done()`: si `isGameOver()` → `MENU_CREDITS` (con `_game.win()`), si no → `MENU`. `changeState(MENU)` calcula `showContinue`/`selected` (arriba). |
| `CONTINUE` | `Game` | **Activo.** Reanuda en pausa (`begin(false)`, con `speedFor(storage.difficulty())`); se retoma con `ACTION_RIGHT`; `ACTION_UP` sale sin perder la partida (`done()` sin `GAME_OVER`). Al `done()`, igual que `NEW`. |
| `MENU_DIFFICULTY` | `MenuDifficulty` | **Activo.** Selector 1..10 **sobre el `Menu`**: borra/repinta la banda 45..53 con `< N >`. `MOVE_LEFT/RIGHT` editan con repetición (`SFX_CLICK` por paso), `ACTION_RIGHT` aplica, `ACTION_UP` cancela. Al `done()` → `changeState(MENU)` (repinta solo la banda vía `showOptions()`); el nivel queda en `_difficulty` de la ventana — **no llega al juego** (§16). |
| `MENU_SOUND` | `MenuSound` | **Activo.** Selector On/Off sobre el `Menu`, misma banda. `MOVE_LEFT` apaga / `MOVE_RIGHT` enciende (`SFX_CLICK`), `ACTION_RIGHT` aplica (`sound.setEnabled()`, `SFX_CONFIRM` solo si queda ON), `ACTION_UP` cancela. Al `done()` → `changeState(MENU)` (repinta solo la banda). El valor real vive en `Sound`. |
| `MENU_CREDITS` | `MenuCredits` | **Activo** (§10.3). Roll vertical automático de los créditos: tramo a tramo (entrada, reposo centrado, pausa 4 s, salida por el límite superior del Body). **Sin tarjeta final** (el último tramo se repite en bucle). Música `SFX_DRAGON_BALL`/`SFX_PIRATES` en bucle. Cualquier botón cierra → `MENU`. Reposo bloqueado mientras está abierta (`inCredits()` → `Engine::sleepBlocked()`). **Defecto:** tras una muerte, abrir Credits desde el menú también lleva `fromGame=true` → título "GAME OVER". |

### Métodos
| Método | Descripción |
|--------|-------------|
| `void begin()` | Primera transición: `changeState(State::BOOT)`. Se llama desde `setup()`. |
| `void update()` | Actualiza la ventana activa y gestiona transiciones. `BOOT → LEGEND → MENU` + despacho de `NEW`/`CONTINUE`/`MENU_SOUND`/`MENU_DIFFICULTY`/`MENU_CREDITS`: al `done()` del `Menu`, `selected()` abre `NEW`/`CONTINUE`/`OPT_DIFFICULTY`/`OPT_SOUND`/`OPT_CREDITS` (default → `LEGEND`); cada submenú vuelve al `MENU` con su `done()` (los selectores tocan su propio `SFX_BACK`/`SFX_CONFIRM`; `MenuCredits` toca su sonido al salir). Al `done()` de `NEW`/`CONTINUE`: `SFX_BACK` y `MENU` si no hay muerte o `MENU_CREDITS` si `isGameOver()` (el récord ya lo volcó `Game::die()`). |
| `void print()` | Despacha el dibujo a la ventana activa. **Ya no limpia la pantalla**: cada ventana limpia solo en su primer frame tras `begin()` y luego solo sus zonas dinámicas (§13). |
| `bool inGame()` | `true` si hay partida en curso y el juego corre (el reposo no aplica, §26): `(NEW \|\| CONTINUE)` y `_game.state() == Game::State::PLAY`. |
| `bool inCredits()` | `true` si los créditos están abiertos: `_state == MENU_CREDITS && !_menuCredits.done()`. |
| `bool sleepBlocked()` | `true` si el reposo no aplica ahora: `inGame()` o `inCredits()`. Lo consulta `SleepManager::updateAwake()` (§26), que mantiene `_idle` a cero mientras dure el bloqueo. |
| `void setBestScore(uint16_t)` | **Comentado**: ya no hace falta (el menú lee `storage.bestScore()` directamente). |
| `void changeState(State newState)` | Transición común: guarda el estado y llama `begin()` de la ventana destino. `MENU`: calcula `showContinue = !_game.isGameOver() && _game.score() > 0` y `selected` (Continue si se venía de `NEW`/`CONTINUE` con partida reanudable; si no, mantiene la opción salvo que Continue quede oculta → `OPT_NEW`) y llama `_menu.begin(showContinue, selected)`. `MENU_CREDITS`: `_menuCredits.begin(_game.isGameOver(), _game.win())`. `NEW`/`CONTINUE`: `_game.begin(true/false)` (antes `setDifficulty`, no-op). Sin parámetro `beginWindow`. No reproduce sonidos. |

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
4. `ACTION_RIGHT` activa la opción del menú — `Menu::confirm()` sigue comentado: `action()` pone `_confirm = true` (con `SFX_CONFIRM`) y `blinkOption()` lo convierte en `_done`; el `Engine` despacha con `done()` + `selected()`.
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
| `MenuCredits` | Clear + título 12x16 en el Header (`CREDITS`/`YOU WIN`/`GAME OVER`), solo en el primer frame (§10.3) | Slide del tramo actual: `_scroller.print()` (pinta los px encendidos del bloque; no borra, cada paso cubre el anterior y el Scroller recorta a la banda del Body). Sin tarjeta final (`drawCard()` comentado) |
| `Legend` | clear + banda de título "Move"/"Action", pad MOVE, los 4 rombos de ACTION y la banda del pie con `BTN_FUNC[0]` | Rombo activo (`blinkDiamond()`/`toggleDiamond` cuando `_blink.changed(PERIOD, OFF)`) + texto del pie (`Scroller` propio): al cambiar de rombo entra deslizándose (`begin(DIR_LEFT, STAY)` + `addText` + `setRow` + `startSlide()`); mientras se mueve, el ciclo se congela (`_scroller.update()` devuelve `true`) y al terminar se reanclan `_ticker` y `_blink.start()`. Al cambiar, se restaura completo el rombo que deja de ser activo. |
| `Game` | clear + puntaje 12x16 en el Header (izq.) + comida normal/especial y serpiente | `print()`: `_food.print()` y `_specialFood.print()` cada frame (dibujan/borran solo sus celdas); `printScore()` solo al comer (`_ateFood`/`_ateSpecialFood`); `_snake.print(_food, _specialFood)` solo al mover; overlay con `drawOverlay(texto, fullWidth, show)` (texto invertido sobre banda blanca de lado a lado, o cuadro centrado): cuenta atrás `START` ("3"→"2"→"1" con `Blink` `PERIOD=1000`/`OFF=25`: visible → `SFX_TICK` + número, oculto → repinta el tablero vía `_clear`+`firstPrint()`), `PAUSE` → "PAUSA" (fullWidth) cada frame. `GAME_OVER` no dibuja overlay (`die()` ya puso `_done=true` → el `Engine` salta a `MENU_CREDITS`); el festejo de récord "BUT"/"YOU ARE"/"THE BEST" quedó **comentado**. El antiguo `_dirtyBoard`/`_redrawHeader`/`COUNT_HIDE_MS`/`NEW_BEST_SIGN_MS` están comentados o eliminados. |

4. `MenuDifficulty`/`MenuSound` son **capas sobre el `Menu`**: no limpian la pantalla completa, solo la banda de los rombos (ver §7). Comparten estructura: `firstPrint()`/`clear()` borran la banda (45..53) en su primer frame y luego `nextOption()`/`blinkOption()` repintan el selector. El resto del `Menu` (título, cuadro, pie) queda intacto. El regreso es `changeState(State::MENU)` → `_menu.begin(showContinue, selected)`; como `selected` es `OPT_DIFFICULTY`/`OPT_SOUND`, `begin()` **no** hace `clear()` y llama `showOptions()` directo (solo la banda). Los flags `_redrawSound`/`_redrawDifficulty` ya no existen.

---

## 14. Clase `Scroller` — API (scroller de 1 bit en 4 direcciones)

Scroller de 1 bit **en 4 direcciones** (`Draw::Direction`): compone un **bloque de varias líneas** en un array de 1 bit/píxel (cada byte = **1 columna de 8 px**, bit 0 = fila 0, bit 7 = fila 7) y lo desliza en la dirección fijada, desde `_from` hasta `_to`. **Un bloque** por instancia. Cada línea lleva **color de letra y de fondo** y su `size` (1..3); **el fondo ocupa el ancho completo** de la pantalla y **el texto va centrado** en ella. **Sin pausas internas**: el destino y el comportamiento al llegar los fija el caller con la **API mínima** (`begin(dir, EndMode)` + `setRow(y)` horizontal o `setVertical(to, from)` vertical); el timing (pausas entre vuelos) es del caller. **Una** clase que **reemplaza a las retiradas `ScrollerH` y `ScrollerV`**; la usan `Menu`/`Legend` (horizontal) y `MenuCredits` (vertical, dos vuelos). Sin memoria dinámica (`_strip` por valor). Archivos `Scroller.h/.cpp`.

- **`_strip`:** mapa de 1 bit/píxel de **toda la pantalla** `[HEIGHT/8][WIDTH]` (8×128 B = 1024 B); cada byte = 1 columna de 8 px (bit 0 = fila 0, bit 7 = fila 7).
- **Constructor:** `Scroller()` — sin parámetros (usa la `display` global). Copia bloqueada por coherencia.
- **Constantes:** `WIDTH`/`HEIGHT`/`CHAR_W`/`CHAR_H` de `Config::Screen`; `MAX_LINES = Config::Scroller::MAX_LINES` (12), `ANIM_H = Config::Scroller::ANIMATION_H` (4 ms/px), `ANIM_V = Config::Scroller::ANIMATION_V` (12 ms/px).
- **API:**
  - `enum class EndMode : uint8_t { STAY, EXIT }` — **STAY**: el bloque reposa en `_to` (`done()` al llegar). **EXIT**: el bloque sale por `_to` (desaparece). EXIT solo aplica a vertical; en horizontal se fuerza STAY.
  - `void begin(int8_t dir, EndMode end = STAY)` — fija la orientación (`Draw::Direction`) y el modo, y reinicia todo el estado (`_done = true`, sin bloque). Horizontal = `DIR_LEFT`/`DIR_RIGHT`; vertical = `DIR_UP`/`DIR_DOWN`.
  - `void addText(const char* text, uint8_t size, bool tColor, bool bgColor)` — apila una **línea pendiente** (texto + color de letra/fondo + escala 1..3, recortada en silencio). Hasta `MAX_LINES` (las siguientes se ignoran). **No copia el texto** (guarda el puntero): la cadena debe seguir viva hasta `startSlide()`.
  - `void setRow(int16_t y)` — modo horizontal: fila `y` del bloque. Los límites los fija `startSlide()`: aparece **desde el borde con el telón fuera** (`DIR_LEFT`: `WIDTH+telón`; `DIR_RIGHT`: `-WIDTH-telón`) y **reposa en `x=0`** (texto completo en pantalla, telón oculto). El telón se cuenta una sola vez.
  - `void setVertical(int16_t toLimit, int16_t fromLimit)` — modo vertical: `fromLimit` = origen donde aparece; `toLimit` = destino (reposo en STAY, límite de salida en EXIT). Coordenadas de pantalla del borde superior del bloque; pueden quedar fuera (`int16_t`). Sin `setVertical`, el vuelo por defecto es la travesía completa (`HEIGHT → -alto`).
  - `void startSlide()` — mide el bloque (alto = Σ `size·CHAR_H + 2`; **ancho = `WIDTH`**, el fondo va a todo lo ancho), lo rasteriza a `_strip` (`compose()`), resuelve `_from`/`_to` (los de `setRow`/`setVertical` o los por defecto de la dirección), fija la posición (`_pos = _from`, `_step` = signo de `_to − _from`), arranca el `Ticker` **y libera** las líneas (`_lineCount = 0`).
  - `bool update()` — avanza `_step` px por cada paso del `Ticker` (4 ms/px en H, 12 ms/px en V) hasta `_to`. Sin pausas: al llegar se detiene. Devuelve `true` mientras anima y `false` si `_done`.
  - `bool print()` — **pinta el bloque** actual (solo los píxeles encendidos de `_strip`); no borra nada (cada paso cubre el anterior y solo queda al descubierto el aire de la última línea). Recorta fuera de pantalla y, en **vertical, a la banda del Body** (`[BODY_TOP, HEIGHT)`): el Header queda intacto y el bloque desaparece al cruzar el límite. Devuelve `false` si no cambió de posición o si `_done`; pone `_done` en el frame en que `_pos == _to`.
  - `bool done() const` — `true` al alcanzar `_to` (reposo en STAY, salida completa en EXIT).
- **Composición (`compose()`):** por cada línea apila `size·CHAR_H + 2` px de alto; **el fondo se pinta a todo lo ancho** (incluidos los 1 px de aire arriba y abajo) y **el texto se centra en la pantalla** (`display.centerX`). Las líneas se apilan de arriba a abajo desde la esquina superior izquierda del bloque.
- **Privados:** `curtain()`/`timeCurtain()` (adorno de entrada, **solo horizontal**, portado de `ScrollerH`; usa el `size` de la primera línea), `getStripPixel()`/`setStripPixel()`, `clearStrip()`, `compose()`, `blockX()`/`blockY()`, `ticker()`.
- **Uso "entrar y reposar" (STAY):** el bloque queda **quieto** en `_to` (`done()`); `Menu`/`Legend` lo usan en horizontal (`setRow(y)`; reposo en `x=0`) y `MenuCredits` en vertical (`setVertical(rest, BODY_TOP+BODY_H)`: entra por abajo y reposa centrado).
- **Uso "entrar, reposar y salir" (dos vuelos):** `MenuCredits` encadena un vuelo STAY (hasta `_rest`) con una pausa externa y un vuelo **EXIT** (`setVertical(BODY_TOP − blockH, _rest)`) que arranca en el reposo (origen = posición actual: sin salto) y termina cuando el bloque queda **completo fuera** de la banda del Body.
- **Usos:** `Menu` (1, `TEXT_12x16`, fila `BOX_TOP−1`; `nextOption()` fija `DIR_LEFT`/`DIR_RIGHT` según el sentido del cambio, `STAY`, y congela `Menu::update()` mientras `update()` devuelve `true`). `Legend` (1, `TEXT_6x8`, fila `FOOT_TOP−1`; entra por la derecha en cada cambio de rombo, `STAY`). `MenuCredits` (1, banda `BODY_TOP/BODY_H`, `TEXT_6x8`; dos vuelos por bloque: entrada `STAY` + salida `EXIT`). `Boot` **no** lo usa.

## 15. Namespaces de `Sprite.h` — API (sprites de la serpiente, comida especial y logo)

`Sprite.h`. Tabla estática con los sprites (estilo Nokia), el de la **comida especial** y el **logo** del arranque. **Namespaces de solo datos** (sin instancia ni `.cpp`), divididos en tres: `Sprite` (serpiente), `Special` (comida especial) y `Logo` (logo). Accesos: `Sprite::pixel(part, x, y)`, `Special::specialPixel(x, y)` y `Logo::LOGO` (este último se dibuja por bloques con `Display::drawBitmap()`; no tiene función de lectura).

**Empaquetado a 1 bit/píxel.** Cada sprite es un **literal binario** del tamaño exacto de sus px (4×4 = un `uint16_t`; comida 8×4 = un `uint8_t` por fila): en memoria solo queda la versión empaquetada, sin conversión ni tabla intermedia.

| Tabla | Antes | Ahora | Bytes |
|-------|-------|-------|-------|
| Serpiente (4×4) | `uint8_t SPRITES[COUNT][4][4]` | `uint16_t SPRITES[COUNT]` (16 px = 16 bits) | 432 → **54** |
| Comida especial (8×4) | `uint8_t SPECIAL_FOOD[4][8]` | `uint8_t SPECIAL_FOOD[4]` (1 byte/fila) | 32 → **4** |

Los literales llevan separadores `'` para leer las filas: en 4×4, cuatro grupos de 4 bits (una fila por grupo); en la comida, dos grupos de 4 por fila. El px `(0,0)` es el bit más significativo; el último, el bit 0. El acceso lo hacen las funciones `constexpr` `pixel()`/`specialPixel()` (el juego nunca indexa bits a mano).

### Namespace `Sprite` — serpiente

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

| Miembro | Contenido |
|---------|-----------|
| `SIZE` | Lado del sprite (4). |
| `BITS` | Píxeles por sprite de la serpiente (16) = bits de su `uint16_t`. |
| `SPRITES[COUNT]` | Sprites **empaquetados** indexados por `Part`: cada uno un literal de 16 bits en cuatro grupos de 4. Rango: 0..3 cola, 4..7 cuerpo, 8..11 curvas, 12..15 cabeza cerrada, 16..19 cabeza abierta, 20..25 panza, 26 `EMPTY`, 27 `COUNT`. |
| `pixel(Part, x, y)` | `constexpr`: píxel (`x`=columna, `y`=fila) desde el `uint16_t` empaquetado. |
| `EMPTY` | Sprite vacío (todo fondo). |
| `COUNT` | Cantidad de sprites de la tabla. |

La panza recta comparte sprite por par de direcciones: `BELLY_TO_RIGHT` = `BELLY_TO_UP` y `BELLY_TO_LEFT` = `BELLY_TO_DOWN`.

### Namespace `Special` — comida especial

| Miembro | Contenido |
|---------|-----------|
| `SPECIAL_FOOD_W`/`SPECIAL_FOOD_H` | Ancho (8) y alto (4) en px del sprite de la comida especial. |
| `COUNT` | Número de sprites de la tabla (2: el sprite y el vacío). Sustituye al antiguo `SPECIAL_FOOD_MAX`. |
| `SPECIAL_FOOD[COUNT]` | Sprite 1 bit de la **comida especial** (8×4) **empaquetado por fila** (literal de 8 px por fila, px `(0,0)` = bit 7) = 4 bytes, tabla aparte por no encajar en los 4×4. |
| `specialPixel(x, y)` | `constexpr`: píxel de la comida especial. El índice se **acota** (`if (sprite >= COUNT) sprite = COUNT;`) en vez de devolver `false` fuera de rango; como `COUNT` es el sprite vacío, el resultado es equivalente. |

Lo consume `Food` (alias `SPECIAL_FOOD*` en `Food.h`, §17).

### Namespace `Logo` — logo del arranque

| Miembro | Contenido |
|---------|-----------|
| `LOGO_W`/`LOGO_H` | Ancho (80) y alto (48) en px del logo del arranque. |
| `LOGO[LOGO_H * (LOGO_W / 8)]` | Logo 1 bit **empaquetado por fila** en formato de `drawBitmap()` (1bpp, MSB primero: px `(0,0)` = bit 7): 10 bytes/fila = **480 bytes** (vs. 3840 si 1 byte/px). **Polaridad inversa al resto** (**1 = fondo, 0 = glifo**), por eso `Boot` lo pinta con `Display::drawBitmap(..., true, false)` y **no tiene** función de lectura. |

`Sprite.h` (antes `SnakeSprites.h`) forma parte del respaldo adaptado al estilo del proyecto (pie `// Fin`, cabecera descriptiva). Lo consumen `Snake` (§18, `Sprite::pixel`), `Food` (§17, `Special::specialPixel`) y `Boot` (§8, alias `Logo::LOGO`). Nota: el comentario de `Sprite.h` §39 aún menciona `Sprite::specialPixel` (nombre viejo; hoy es `Special::specialPixel`).

---

## 16. Clase `Game` — API (ventana del juego)

`Game.h/.cpp`. Ventana del juego (estados `NEW`/`CONTINUE` del `Engine`; estados propios `START`/`PLAY`/`PAUSE`/`GAME_OVER`), reemplaza al placeholder `InfoWindow` (eliminado). **Coordina**: la lógica vive en `Snake` (§18). `Game` decide el ritmo (dificultad vía `Storage`), lee botones, traduce MOVE a `Snake::Dir`, llama `snake.step(_food, _specialFood)` y maneja el resultado (`MOVED`/`ATE_FOOT`/`ATE_SPECIAL`/`DIED`) con puntaje, sonidos y regeneración del alimento (doble comida: `_food` normal + `_specialFood` especial, ambas `Food`). El dibujo de la serpiente lo hace `Snake::print` (§18); `Game` pinta el Header (puntaje), las comidas y los overlays.

### Constructor
```cpp
Game();
```
Sin parámetros: consume los servicios globales. `_snake`, `_food`, `_specialFood` y el resto se construyen solos en su lista de inicialización. Arranca en estado `START` (sin partida en curso; `begin()` decide `PAUSE`/`START` al entrar).

### Constantes (alias de `Config` en `Game.h`)
| Constante | Origen | Significado |
|-----------|--------|-------------|
| `BODY_MIDDLE`, `CHAR_W`, `CHAR_H`, `WIDTH`, `HEIGHT` | `Config::Screen` | Geometría de overlays y Header. |
| `COUNT` | `Config::Board::COUNT` (3) | Dígitos del conteo regresivo (3-2-1). |
| `PERIOD`, `OFF` | `Config::Board` (1000 ms, 25 %) | Parpadeo del conteo (`Blink`). |
| `FOOD_COUNT` | `Config::Food::FOOD_COUNT` (10) | Comidas normales entre cada comida especial. |

Los tiempos se miden con `Timer.h` (§21): `Ticker _timer` (intervalo entre pasos, iniciado con `_moveDelay` ms) y `Blink _blink` (conteo regresivo); ya **no** hay `Stopwatch _countdown`/`_step`/`_cele` ni comparación de `nowMs()` contra instantes absolutos. Los flags activos son `_done` (salida), `_clear` (primer frame), `_blinkChanged`, `_moveSnake`, `_ateFood`, `_ateSpecialFood`; los antiguos `_redrawHeader`/`_dirtyBoard`/`_overlayHidden`/`_lastCount`/`_newBest`/`_celeSfx` y las constantes `SPRITE_SCALE`/`COUNTDOWN_MS`/`COUNT_HIDE_MS`/`NEW_BEST_SIGN_MS` están **comentados**. El miembro `_win` recuerda si la última muerte superó el récord (lo consume el `Engine` para `MENU_CREDITS`).

### Métodos
| Método | Descripción |
|--------|-------------|
| `void begin(bool newGame)` | Ambos casos: `_done=false`, `_clear=true`, `snake.clearPending()`, `_win=false`, `speedFor(storage.difficulty())`, `_state=PAUSE`. `newGame=true` (NEW): `snake.reset()`, `randomSeed(esp_random() ^ nowMs())`, genera la especial primero y la normal después (`_specialFood.spawn(SPECIAL)` + `_food.spawn(NORMAL)` + `_specialFood.clear()`, reservando la celda especial), `START`, `_countFood=0`, `_score=0`, `_blink.start()`, `_timer = Ticker(_moveDelay)`, `_countdown=COUNT`. `newGame=false` (CONTINUE): queda en `PAUSE` con la partida guardada. |
| `void setDifficulty(uint8_t level)` | **No-op** (cuerpo comentado). El `Engine` aún lo llama con `_menuDifficulty.difficulty()`, pero la dificultad real la fija `storage.difficulty()` en `begin()`; `MenuDifficulty` no escribe en `Storage`. **Pendiente conocida.** |
| `void update()` | `START`: `ACTION_RIGHT` → `SFX_START` + `startPlay()`; si no, `Blink` con `PERIOD`/`OFF`: al pasar a oculto, si `_countdown==1` → `SFX_START` + `startPlay()`, si no `_countdown--`. `PLAY`: gira con MOVE (inline, no `handleTurn()`, que está vacío; `SFX_TURN` si `turn()` aceptó), `ACTION_RIGHT` → `SFX_PAUSE` + `PAUSE`, `_moveSnake = _timer.consume() > 0` y `step()` si toca. `PAUSE`: `ACTION_RIGHT` → `SFX_RESUME` + `startPlay()`. `GAME_OVER`: `anyHeld()` → `_done=true` (raramente alcanzable: `die()` ya puso `_done`). Tras el `switch`: `ACTION_UP` sale en cualquier estado salvo `GAME_OVER` (la partida NO se pierde). |
| `void step()` | `result = _snake.step(_food, _specialFood)`; `DIED` → `die()`; si no, `_specialFood.lessTime()` (deciende el contador de la especial **cada paso**); `SFX_EAT` si comió; si `ATE_FOOT`: `_score += _food.score()`, `_food.clear()`, `spawnFood(_food)`, `_countFood++` y al llegar a `FOOD_COUNT` (10) `spawnFood(_specialFood)` + `_countFood=0`; si `ATE_SPECIAL`: `_score += _specialFood.score()`, `_specialFood.clear()`. |
| `void spawnFood(Food&)` | `food.spawn(food.type(), *this)`; si no hay celdas libres: `_snake.reset()` y, si `storage.difficulty() < MAX_LEVEL`, `storage.setDifficulty(storage.difficulty() + 1)` (el "subir de nivel" al llenar el tablero) + `_clear=true` y respawn; si ya está en el máximo, `die()`. |
| `void die()` | `GAME_OVER`; `_win = _score > storage.bestScore()`; si `_win`, `storage.setBestScore(_score)`; **`_done = true`** (el `Engine` entra en `MENU_CREDITS` en vez de mostrar un panel de GAME OVER). El festejo de récord quedó comentado. |
| `void print()` | `firstPrint()`; `_food.print()` y `_specialFood.print()` cada frame; `printScore()` al comer (`_ateFood`/`_ateSpecialFood`); `_snake.print(_food, _specialFood)` al mover (`_moveSnake`); overlay del estado (ver §13). |
| `void printScore()` | Puntaje `TEXT_12x16` en `(0,0)`. |
| `void drawOverlay(const char*, bool fullWidth, bool show)` | Banda blanca de lado a lado (`fullWidth`) o cuadro centrado, con texto invertido centrado en el rectángulo (misma geometría que antes: `y = BODY_MIDDLE - h/2`). |
| `void firstPrint(bool forceRedraw = false)` | `clear()` + `printScore()` + `_food.print()` + `_specialFood.print()` + `_snake.print(...)`; si `PAUSE`/`START` y `forceRedraw`, overlay "PAUSA". |
| `bool done()` | `true` al pedir volver al menú. |
| `bool isGameOver()` | `true` si el estado es `GAME_OVER`. Lo usa el `Engine` para decidir `MENU` vs `MENU_CREDITS` y el `showContinue` del menú. |
| `uint16_t score()` | Puntaje actual. **El récord vive en `Storage`** (§22). |
| `State state()` | Estado actual (`START`/`PLAY`/`PAUSE`/`GAME_OVER`); lo consulta `Engine::inGame()` (solo cuenta con reposo bloqueado en `PLAY`). |
| `bool win()` | `_win`: la última muerte superó el récord (para el título "YOU WIN" de los créditos). |
| `void speedFor(uint8_t level)` | **Público.** `_moveDelay = 1000 - (nivel-1)*101 - (nivel+1)/3` ms/paso (secuencia 1000, 898, 797, 696, 594, 493, 392, 290, 189, 88; ver fórmula en el código). |
| `bool occupied(int16_t x, int16_t y)` | ¿Celda ocupada por la serpiente **o por cualquiera de las dos comidas**? Lo consulta `Food::spawn`. |

### Reglas del juego (qué coordina Game)
- **Movimiento:** la cabeza avanza 1 celda por paso con **wrap en X e Y**. Velocidad (`_moveDelay` ms/paso) lineal con la dificultad con **pasos alternados de 101/102 ms** (102 en niveles 1, 4 y 7): `1000 - (nivel-1)*101 - (nivel+1)/3`. Los 9 saltos suman 912 ms, de **1000 ms (nivel 1) a 88 ms (nivel 10)** (secuencia 1000, 898, 797, 696, 594, 493, 392, 290, 189, 88; 101,33 ms/nivel no es entero → 6 saltos de 101 y 3 de 102). Fórmula en `Game::speedFor`.
- **Paso (`step`):** llama `snake.step(_food, _specialFood)` (`Snake::step` recibe los dos `Food`). `Result`: `MOVED` → solo `_moveSnake` marca que se debe repintar la serpiente; `ATE_FOOT` → `_score += _food.score()` (nivel actual), `SFX_EAT`, `_food.clear()`, `spawnFood(_food)`, `_countFood++` y si llega a `FOOD_COUNT` (10) `spawnFood(_specialFood)` + `_countFood=0`; `ATE_SPECIAL` → `_score += _specialFood.score()`, `_specialFood.clear()`, `SFX_EAT`; `DIED` → `die()`.
- **Comer/crecer, colisión, giro, sprites:** `Snake` (§18). Al morir, `die()` guarda el récord solo si `_score > storage.bestScore()`, marca `_win` y pone `_done=true` → el `Engine` pasa directamente a `MENU_CREDITS`.
- **Doble comida:** `_food` (normal) y `_specialFood` (especial). La especial se **genera** cada 10 comidas normales y se **borra** al ser comida o cuando `lessTime()` la hace expirar en `step()` (cada paso se decrementa el tiempo de la especial).
- **Tablero lleno:** en `spawnFood`, si no hay celdas libres, `_snake.reset()`, si `storage.difficulty() < MAX_LEVEL` → `storage.setDifficulty(storage.difficulty() + 1)`, `_clear=true`, vuelve a `spawnFood`; si ya está en el máximo → `die()`.
- **Comer/crecer, colisión, giro, sprites:** los resuelve `Snake` (§18); `Game` solo consume el resultado y repinta.
- **Alimento:** rombo simétrico centrado en la celda (`Draw::diamond`), como el del menú; lo dibuja `Food` (§17). La **semilla aleatoria** se fija en `Game::reset()` con `randomSeed(esp_random() ^ (uint32_t)nowMs())` (`esp_random()` = RNG de hardware, entropía real) y se consume en `Food::spawn`.
- **Colisión con el cuerpo:** la celda destino es ilegal si coincide con el cuerpo **salvo la cola cuando NO come** (la cola se libera ese paso; bloqueante solo al comer); la resuelve `Snake::step`. Si colisiona: `GAME_OVER` (`SFX_GAME_OVER`) e informa `SFX_BACK` al volver. **El récord (`storage.bestScore()`, §22) se verifica/actualiza solo aquí, en GAME OVER** (o al llenar el tablero, que también pasa por `die()`): partidas abandonadas con `ACTION_UP` no cuentan. Si supera el récord → **festejo de nuevo récord**: "GAME OVER" → "BUT" → "YOU ARE" → "THE BEST" en ciclo (`NEW_BEST_SIGN_MS` por letrero; `_newBest`, `_celeSfx` y el `Stopwatch _cele` que arranca en `die()`) hasta pulsar un botón; `SFX_NEW_BEST` suena solo la primera vez que aparece "THE BEST" (YOU ARE ya no la dispara).
- **Pausa y salida:** `ACTION_RIGHT` (Btn2) durante `PLAY` pausa ("PAUSA", `SFX_PAUSE`); en `PAUSE` retoma con el mismo botón o `ACTION_LEFT` (`SFX_RESUME`). `ACTION_UP` (Btn1) durante la partida vuelve al menú **sin perderla** (el tablero y el estado se conservan; `Continue` la reanuda en pausa, porque el estado no es `GAME_OVER`). Tras `GAME_OVER` el estado pasa a `GAME_OVER`, así que `Continue` arranca una nueva. Al salir, el `Engine` deja la selección en **`Continue`** si siguió en curso **y con puntos**, o en **`New`** si terminó en GAME OVER o sin puntos (`resumable = !game.isGameOver() && game.score() > 0`, aplicado con `menu.setSelected(...)`); además **"Continue" se oculta** cuando no hay partida o no tiene puntos (`menu.setContinueAvailable(resumable)`): al arrancar, tras `GAME OVER` o sin puntos quedan 4 opciones (New, Dificultad, Sound, Créditos). **Pendiente:** ese cableado (`menu.setSelected`/`menu.setContinueAvailable`) aún no está activo (§11): hoy el `Menu` se reabre con `showContinue = false`.
- **Header:** puntaje en `TEXT_12x16` (izq., no se mueve) y segundos restantes de la **comida especial** en `TEXT_12x16` (der., `_food.specialTime()`, por ahora `Config::Food::SPECIAL_TIME` = 60 solo layout). Se redibuja solo al cambiar. Ya no muestra el récord `HI` ("Best: N" queda solo en el menú).
- **Overlays:** conteo 3-2-1 (un dígito/segundo, `COUNTDOWN_MS/3` ms por dígito, centrado) y "PAUSA"/"GAME OVER" como **banda blanca de lado a lado**. `drawOverlay(title, fullWidth)` dibuja **cuadro centrado** (`fullWidth=false`, el conteo) o **banda borde a borde** (`fullWidth=true`, PAUSA/GAME OVER). La banda sobresale 2 px arriba y 0 abajo y está **centrada como rectángulo a lo alto del Body** (16..63): `y` se deriva del alto de la banda (`bandH = h + 2`) y el texto queda 2 px dentro; siempre `Display::fillRect` blanco + texto `TEXT_12x16` en negro sobre blanco (`drawText(..., SSD1306_BLACK, SSD1306_WHITE)`) centrado. **Festejo:** si al morir se supera el "Best", "GAME OVER" pasa al ciclo de cuatro letreros (`NEW_BEST_SIGN_MS` cada uno) hasta pulsar un botón; `SFX_NEW_BEST` solo la primera vez que aparece "THE BEST". **Parpadeo del conteo:** en los últimos `COUNT_HIDE_MS = 250` ms de cada dígito el número y su cuadro desaparecen; al ocultarlo se marca `_dirtyBoard` una vez (`_overlayHidden`, reiniciado en `reset()`). **Pitido por dígito:** `SFX_TICK` al aparecer cada dígito (`_lastCount`, reiniciado en `reset()`; `SFX_START` no suena en `reset()`, se reserva al arranque real). Al pasar `START → PLAY` suena `SFX_START` (GO!) justo tras el "1". Al volver a `PLAY` se marca `_dirtyBoard`.

### Validación en host (MinGW)
Antes de escribir el código se simuló en el PC la lógica núcleo (misma aritmética de ring buffer, colisiones y selección de sprites): comer/crecer (cola, score, longitud), longitud estable sin comida, colisión real (la cabeza no avanza), la cabeza **puede** ocupar la celda de la cola (anillo casi cerrado), wrap X/Y, y cargo de integridad de 400 pasos con giros. Ese núcleo vive hoy en `Snake` (§18), que por no depender de `Display`/`Buttons`/`Sound`/`Food` se compila y prueba en el PC sin stubs.

## 17. Clase `Food` — API (alimento del tablero)

`Food.h/.cpp`. Encapsula estado y lógica del alimento, extraídos de `Game` (antes `_food`/`_hasFood`/`_specialTime` y `spawnFood()`/`drawFood()`). Componente del juego (no ventana): sin `begin()`/`update()` propios; lo usa `Game` (miembro `_food`).

### Constructor
```cpp
Food(uint8_t cols, uint8_t rows, uint8_t top);
```
Recibe la geometría (dibuja con la `Display` global): rejilla de `cols`×`rows` celdas de `Config::Screen::CELL` px desde la fila `top` (el Body). `Game` la construye: `_food(Snake::COLS, Snake::ROWS, Config::Screen::BODY_TOP)`. El `Food` **renombra** `Config::Screen::CELL` como `CELL` (y el sprite como `FOOD`/`FOOD_WIDTH`/`FOOD_HEIGHT`) en su `.h`; **`Food.cpp` no referencia `Config` ni `Sprite`** (usa solo los renombres). El alto no puede llamarse `FOOD_H`: chocaría con la macro del include guard `#define FOOD_H`.

### Enum y constantes
```cpp
enum class Type : uint8_t { NORMAL = 0, SPECIAL };
static constexpr uint8_t SPECIAL_TIME = Config::Food::SPECIAL_TIME;   // segundos iniciales de la especial (layout)
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
| `uint8_t x()` / `y()` | Columna (0..`COLS`-1) / fila (0..`ROWS`-1). Internamente `_x`/`_y` son **`int16_t`** (antes `uint8_t`). |
| `uint8_t specialTime()` | Segundos restantes de la comida especial. |
| `void setSpecialTime(uint8_t s)` | Actualiza el temporizador de la especial. |
| `void draw() const` | Dibuja según tipo: `NORMAL` → rombo (`drawNormal` → `Draw::diamond`); `SPECIAL` → sprite (`drawSpecial` → `Display::drawBitmap`, 1 px por bit). Si no hay alimento, no dibuja. |

`Game` usa `_food`: lo genera en `reset()` y al comer (`_food.spawn(Food::Type::NORMAL, *this)`; si devuelve `false` se muere), consulta `_food.has()/x()/y()` para comida y boca abierta, dibuja `_food.draw()` al volcar el tablero y muestra `_food.specialTime()` en el Header. La construye en la lista de inicialización como arriba.

---

## 18. Clase `Snake` — API (lógica y dibujo de la serpiente)

`Snake.h/.cpp`. Encapsula toda la lógica de la serpiente, extraída de `Game`. Componente del juego (no ventana): **no depende de `Sound` ni `Food`**, pero sí de la `Display` global (`Globals.h`), porque **se dibuja a sí misma** (como `Food`) —usa `Config.h` (constantes del tablero), `Sprite.h` (enums de partes) y `Arduino.h`—.

### Qué hace y qué no
`Snake` mantiene el **buffer circular** de segmentos, la dirección commitida, el giro pendiente (sin buffer ni reversa directa), el avance con wrap, la detección de colisión, comer/crecer, la elección del sprite de cada parte y su **propio dibujo** (`draw`, que incluye el borrado de la cola liberada). `Game` **coordina**: le pasa el alimento en cada paso, aplica puntaje/sonidos con el resultado, regenera la comida y llama a `Snake::draw(...)` tras cada `step()` (o redibuja el tablero entero). `Game` mantiene la **dificultad (ritmo)** y la **velocidad** (aquí no intervienen): `Snake::step` se llama una vez por paso y `Game` controla la cadencia.

### Constructor
```cpp
Snake();
```
Deja la serpiente en su estado inicial tras construcción: `_dir = Dir::RIGHT`; hay que llamar `reset()` antes de usarla (o para iniciar una partida).

### Constantes
| Constante | Valor | Significado |
|-----------|-------|-------------|
| `COLS`, `ROWS` | 16, 6 | Tablero: rejilla 16×6 de celdas de 8 px en el Body (128×48). Provienen de `Config::Board::COLS`/`ROWS` (alias en `Snake.h`). `Game`/`Food` las usan vía `Snake::COLS`/`Snake::ROWS`. |
| `MAX_LENGTH` | 96 | Segmentos máximos (una celda por segmento; 16×6 celdas caben y nunca se repite celda viva). |
| `Dir::NONE`/`UP`/`RIGHT`/`DOWN`/`LEFT` | 0/1/2/3/4 | Direcciones. El orden (UP=1..LEFT=4) coincide con el de los sprites por dirección (`dir-1`). `NONE` = sin dirección (estado vacío). |
| `Result::MOVED`/`ATE_FOOT`/`ATE_SPECIAL`/`DIED` | — | Resultado de `step`: avanzó sin comer / comió comida normal / comió especial / colisión. Al comer, la cola NO avanza ese paso. |
| `CELL`/`SIZE`/`SCALE`/`BODY_TOP` | 8/4/2/16 | Dibujo: `CELL` = px por celda, `SIZE` = px del sprite 4×4, `SCALE` = px por píxel del sprite (`CELL/SIZE`), `BODY_TOP` = fila superior del tablero. Renombres de `Config::Board`/`Config::Screen`/`Sprite` (el `.cpp` no referencia `Config`/`Sprite` directamente para estas constantes). |
| `START_COL`/`START_ROW`/`START_LENGTH`/`START_DIR` | 1/2/4/2 | Estado inicial de `reset()`: celda de la cola, fila del cuerpo, número de segmentos y dirección inicial (`2` = `Dir::RIGHT`). Renombres de `Config::Snake` (alias en `Snake.h`). |

### Métodos
| Método | Descripción |
|--------|-------------|
| `void reset()` | Inicializa: celdas `(START_COL,START_ROW)..` (`(1,2)..(4,2)`), cabeza a la derecha, longitud `START_LENGTH` (4), giro pendiente `NONE`. |
| `void clearPending()` | Descarta el giro pendiente (`_nextDir = NONE`). Lo llama `Game::begin()` al entrar (START) para no reanudar con un giro acumulado del menú. |
| `bool turn(Dir direction)` | **Único giro pendiente, sin buffer ni reversa directa**. Se evalúa **siempre** desde la dirección actual de la cabeza (`_dir`, la COMMITIDA): seguir, girar a izquierda o derecha; la contraria (180°) se **ignora**. Devuelve `true` si el giro quedó pendiente (el último válido pisa al anterior) para que `Game` toque `SFX_TURN` solo al aceptar. Así no se genera un GAME OVER espurio por una reversa falsa del último MOVE. |
| `Result step(Food food, Food specialFood)` | **Avanza un paso** (aplica el giro pendiente si lo hay). Pasa los dos `Food` (por valor). Devuelve `Result` (`MOVED`/`ATE_FOOT`/`ATE_SPECIAL`/`DIED`); `Game` interpreta: al comer, crece (la cola NO avanza ese paso), `DIED` → no se mueve. |
| `bool occupied(int16_t cellX, int16_t cellY)` | `true` si la celda `(cellX,cellY)` está ocupada. Lo consulta `Food` (al colocar alimento) vía `Game::occupied`, que delega aquí. |
| `uint8_t length()` | Longitud actual. |
| `const Seg& segment(uint8_t segmentIndex)` | Segmento `segmentIndex` (0 = cola, `length()-1` = cabeza) para que `Game` lo dibuje. |
| `Sprite::Part headPart(Food food, Food specialFood)` | Sprite de la cabeza según la dirección commitida: `HEAD_<dir>_OPEN` **una casilla antes** de llegar a **cualquiera de las dos comidas** (la comida está en la próxima celda según `_dir`) y `HEAD_<dir>_CLOSE` al colisionar con ella. Lo usa `print`. |
| `void print(Food food, Food specialFood)` | **Dibuja la serpiente** con la `Display` global: borra la cola liberada en el último `step()` (si la hubo, `_tailDropped` + `_droppedSeg`, repintando sus píxeles en negro) y pinta todas las partes de cola a cabeza. La llama `Game` tras cada `step()` (o cuando corresponde). |

### Internos (segmentos y buffer)
```cpp
enum class Dir : uint8_t { NONE = 0, UP = 1, RIGHT, DOWN, LEFT };
enum class Result : uint8_t { MOVED, ATE_FOOT, ATE_SPECIAL, DIED };
  struct Seg {
    int16_t col, row;
    Dir dir;            // hacia el segmento siguiente (más cerca de la cabeza)
    Sprite::Part part;  // sprite persistente
  };
  Seg _body[MAX_LENGTH];
  uint8_t _headIndex, _tailIndex;   // índices circular (cabeza/cola)
  uint8_t _length;
  Dir _dir, _nextDir;         // commitida + giro pendiente
  bool _tailDropped;          // el último step() liberó una cola pendiente de borrar en draw()
  Seg _droppedSeg;            // esa cola (celda + sprite con el que estaba pintada)
  Sprite::Part bodyPartFor(Dir inDir, Dir outDir);
  Sprite::Part bellyPartFor(Dir inDir, Dir outDir);
  void drawSprite(Sprite::Part part, int16_t col, int16_t row, bool white);
```

- **Ring buffer:** cola en `_tailIx`, cabeza en `_headIx`. Cada paso **agrega una parte nueva** (la cabeza) y **elimina la última** (la cola) salvo al comer (la cola NO avanza). El nuevo segmento se escribe en `slot()` = `_headIx+1` módulo `MAX_LENGTH`; la cabeza nueva apunta a ese slot.
- **El cuerpo NO se mueve:** la casilla que la cabeza deja se convierte en cuerpo con su `part` persistente (`bodyPartFor`/`bellyPartFor`), según por qué lado entra y sale la tubería: recta `BODY_TO_<dir>` (`in == out`) o esquina `CORNER_<horizontal>_<vertical>`/`BELLY_RIGHT_UP`.. al girar (`in != out`). La panza recta comparte sprite por par: `BELLY_TO_RIGHT` = `BELLY_TO_UP` y `BELLY_TO_LEFT` = `BELLY_TO_DOWN`.
- **Comer (`bodyPartFor` modo `EAT`):** con `in == out` se toma la panza recta de la dirección `in`; con giro (comer y girar a la vez), la panza curva `BELLY_<horizontal>_<vertical>`.
- **Colisión (`step`):** la celda destino es ilegal si coincide con el cuerpo **salvo la cola cuando NO come**. El giro pendiente se evalúa con una **copia local** (`dir`) y `_dir` (la COMMITIDA) solo se actualiza si el destino es legal, así al morir la cabeza conserva la orientación real de su último movimiento (su sprite se dibuja según `_dir`).
- **Wrap:** `x = (x + 1) % COLS` etc.: sale por un borde, aparece por el opuesto.
- **Dibujo y borrado de cola:** cuando `step()` no come, recuerda la celda y el sprite de la cola que libera (`_droppedSeg` + `_tailDropped`). `print()` la borra **primero** repintando sus píxeles en negro y luego pinta todas las partes (cola → cabeza). `drawSprite` pinta cada píxel del sprite 4×4 como un bloque `SCALE`×`SCALE` (2×2): cubre la celda de `CELL` px. |

### Validación en host
`Snake` y `Food` usan la `Display` global para dibujarse, así que para compilar en el PC (MinGW) hacen falta stubs de `Display`/`Globals`; el núcleo de lógica sigue siendo válido. |

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

namespace Board {
  constexpr uint8_t COLS = Screen::WIDTH / Screen::CELL;   // columnas del tablero (16)
  constexpr uint8_t ROWS = Screen::BODY_H / Screen::CELL;  // filas del tablero (6)
  constexpr uint8_t MAX_LENGTH = COLS * ROWS;              // segmentos máximos
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

namespace Snake {           // estado inicial de la serpiente (nuevo)
  constexpr uint8_t START_COL = 1;      // columna de la cola al arrancar
  constexpr uint8_t START_ROW = 2;      // fila (serpiente horizontal en el Body)
  constexpr uint8_t START_LENGTH = 4;   // segmentos iniciales
  constexpr uint8_t START_DIR = 2;      // dirección inicial = Dir::RIGHT
}
}
```

Los namespaces internos (`Pin`, `Screen`, `Difficulty`, `Version`, `Credits`, `Legend`, `Diamond`, `Button`, `Scroller`, `MenuStrip`, `DefaultTimer`, `Food`, `Snake`) van en columna 0, sin indentar respecto de `namespace Config {`.

### Qué contiene y quién lo usa
| Namespace | Constantes | Las usan |
|-----------|------------|----------|
| `Config::Pin` | `BUTTONS`, `BUZZER`, `OLED_SDA`, `OLED_SCL` | el `.ino` (pasa `BUTTONS` a `Buttons` y `BUZZER` a `Sound`, que construye su `Buzzer`), `Buzzer` (pin por defecto), `Display` (pines I2C por defecto) |
| `Config::Screen` | `WIDTH`/`HEIGHT`/`CELL`/`ADDRESS` y regiones `HEADER_*`/`BODY_*`/`FOOT_*` | `Display` (defaults y `regionBounds`), `Boot` (bandas TITULO=Header/CUERPO=Body), `Game` (tablero en el Body, celda de sprites y `BODY_TOP`), `Food` (celda: centro del rombo y sprite especial), `MenuCredits` (rol del Body), `Legend` (línea/fila del pie), `Scroller` (`WIDTH`) |
| `Config::Board` | `COLS` (16), `ROWS` (6), `MAX_LENGTH` (96) | `Snake` (constantes del tablero; alias en `Snake.h`), `Game` y `Food` las usan vía `Snake::COLS`/`ROWS`. |
| `Config::Difficulty` | `MIN_LEVEL`/`MAX_LEVEL`/`DEFAULT_LEVEL` | `MenuDifficulty` y `Game` (`setDifficulty`/velocidad): antes duplicadas. El sufijo `_LEVEL`/`DEFAULT_LEVEL` evita la macro `DEFAULT` del core ESP32. |
| `Config::Version` | `VERSION`/`RELEASE_DATE` | `Menu` (`version`, texto del pie; antes `"v0.1"` en la firma). `RELEASE_DATE` lo usa la tarjeta final de `MenuCredits` (`CARD_DATE`). `NAME` ("Snake II") lo dibujan `Boot` (título) y `Menu` (Header). |
| `Config::Credits` | `CREDITS[]` (30 filas; `COUNT` **derivada** con `sizeof`, `uint8_t`), `THANKS`, `PAUSE_MS` (1.5 s), `PX_MS` (40) | `MenuCredits` (§10.4): el roll y la tarjeta final (vía los renombres de clase `CREDITS`/`COUNT`/`CARD_THANKS`); `PAUSE_MS` es la **pausa externa** con el bloque centrado (entre el vuelo STAY y el EXIT). `PX_MS` queda **sin uso** (la velocidad del roll la da `Scroller::ANIMATION_V`, §14). `COUNT` se deriva del array: la lista nunca se desalinea. |
| `Config::Button` | `DELAY` (400 ms)/`TICK` (100 ms) | `ButtonRepeat` (§23): mantener `DELAY` ms para repetir y luego un paso cada `TICK` ms; lo usan `Menu` y `MenuDifficulty` a través suyo. **Nuevo**. |
| `Config::Legend` | `NEXT` (2500 ms)/`PERIOD` (100 ms)/`OFF` (50 %); `HOLD` **comentado** | `Legend` (ciclo de rombos con `_ticker`, parpadeo con `_blink`/`Blink`): `HOLD` pasó de 900 ms a comentado; `NEXT` subió de 2200 a 2500 ms. |
| `Config::Diamond` | `SIZE` (3 px) | `Menu` (rombo marcador y triángulos: `Menu::SIZE`) y `Legend` (rombos de ACTION: `Legend::SIZE`). |
| `Config::Food` | `SPECIAL_TIME` (60 s) | `Food` (segundos iniciales de la comida especial; renombrado `SPECIAL_TIME` en `Food.h`, §17; el Header de `Game` lo muestra). |
| `Config::Snake` | `START_COL`/`START_ROW`/`START_LENGTH`/`START_DIR` (1/2/4/2) | `Snake` (estado inicial de `reset()`; renombrados en `Snake.h`, §18). `START_DIR` = `2` = `Dir::RIGHT` (orden de `Snake::Dir`). **Nuevo**. |
| `Config::Scroller` | `ANIMATION_H` (4 ms/px)/`ANIMATION_V` (12 ms/px)/`MAX_LINES` (12) | `Scroller` (§14: `ANIM_H`/`ANIM_V` y el array fijo de líneas). Reemplaza a los antiguos `Config::ScrollerH` (`MAX_H`/`ANIMATION`) y `Config::ScrollerV` (`MAX_H`/`ANIMATION`), **retirados**. |
| `Config::MenuStrip` | `BODY_H` (39)/`BODY_MIDDLE` (34)/`BOX_HEIGHT` (16)/`BOX_TOP` (= `BODY_MIDDLE - BOX_HEIGHT/2` = 26)/`VALUE_TOP` (44)/`VALUE_HEIGHT` (11)/`DIAMOND_Y` (= `VALUE_TOP + VALUE_HEIGHT/2` = 49)/`TRIANGLE_Y` (= `Screen::FOOT_TOP - 3` = 53) | `Menu` (cuadro y marcadores, §7) y `MenuSound` (banda del selector ON/OFF). Geometría **centrada en la mitad del Body** (`B`/`C` auxiliares). |
| `Config::DefaultTimer` | `PERIOD` (500 ms)/`OFF` (20 %); `HOLD` **comentado** | Parpadeo "de cualquier ventana" sin cronómetro propio: hoy `Boot` (mensaje) y `Menu` (rombo de la opción). `HOLD` comentado (sin fase fija inicial). |
| `Config::Power` | `IDLE_TIMEOUT_MS` (180000 ms; 3 min) | Reposo (§26): tiempo sin actividad fuera de partida antes del light sleep. Lo usa `SleepManager` (cronómetro `_idle`). |

Las regiones `HEADER_TOP/H` y `BODY_TOP/H` reemplazan las constantes repetidas `BODY_TOP`/`BODY_H`/`TITLE_TOP`/`TITLE_H` de `Menu`, `Game`, `Boot` y `MenuCredits`; los `static constexpr` de esas clases se eliminaron. `Boot` tenía además constantes de la animación de bandas (`BAR_W`, `BAR_SPACING`, `ANIM_TICK`, `TOTAL_MS`), hoy **comentadas**.

La banda del pie tiene su geometría en `Config::Screen`: `FOOT_LINE` (`HEIGHT - (FOOT_H + 1 + 1)` = 54) está **comentado**; `FOOT_TOP` = 56 con `FOOT_H` = 8, derivados de `HEIGHT` como `HEADER_*`/`BODY_*` (antes `FOOT_TOP` = `HEIGHT - 9`, `FOOT_H` = 7). Hoy los usan **`Legend`** y **`Menu`** (pie: banda blanca `55..63` con "Best" y la versión).

`CELL` es la **única** medida de px por celda (reemplaza al `static constexpr uint8_t CELL = 8` de `Food` y a los `8` literales de `Game::drawSprite`); `Food` lo reexpone como `CELL` en su `.h` (§17). Lo **no** compartido (factor de escala del sprite 4×4 a la celda) queda como constante propia con nombre: `Game::SPRITE_SCALE = 2` (`Sprite::SIZE * SPRITE_SCALE == Config::Screen::CELL`).

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

`Sound` no tiene reloj propio (delega en `Buzzer`). `Snake` no usa el reloj (y, al dibujar con la `Display` global, ya no es "lógica pura": ver §18).

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

### Estado actual (integración parcial)
La clase y su instancia global ya las consumen **`Menu`** (lee `storage.bestScore()` para el pie "Best", §7) y **`Game`** (lee el récord y lo **escribe** al batirlo en GAME OVER: `storage.bestScore()`/`setBestScore()`, §16). Sigue **pendiente** migrar a `storage` las copias de `MenuDifficulty` (su `_difficulty`, §10.3) y `Sound` (su `_enabled`, §10.2).

---

## 23. `ButtonRepeat.h` — repetición al mantener un botón

`ButtonRepeat.h/.cpp`. Helper **por composición** (no hereda ni es heredado): encapsula "paso inmediato al pulsar y, si se mantiene, un paso cada `tick` ms tras `delay` ms". Sustituye los dos `Stopwatch` (`_repeat`/`_repeatTick`) que `Menu` y `MenuDifficulty` llevaban sueltos, que además habían **divergido** (`Menu` repetía con `buttons.heldDown()`; `MenuDifficulty` con el viejo `buttons.state()`). Aquí se unifica en `heldDown()`.

### API
```cpp
class ButtonRepeat {
public:
  explicit ButtonRepeat(uint32_t delay = Config::Button::DELAY,
                        uint32_t tick  = Config::Button::TICK);

  bool step(uint8_t button);   // ¿toca aplicar un paso en este frame?
};
```
- `step(button)`: `true` si el botón pasó a presión (`pressed`, reinicia ambos cronómetros) o si sigue mantenido (`heldDown`) y ya vencieron `delay` y `tick` (reinicia solo el de cadencia). `false` en cualquier otro caso.

### Decisiones
- **Sin `reset()`**: ningún call site lo necesita hoy (YAGNI); si aparece, se agrega.
- **Tiempos por defecto de `Config::Button`** (`DELAY = 400`, `TICK = 100`): las ventanas no repiten constantes propias.
- Usa `buttons.heldDown()` (no `state()`), el método vigente.

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
- **`Food`**: rombo de la comida normal con `Draw::diamond(centerX, centerY)` en `Food::drawNormal` (§17).

### Decisiones
- **Sin estado → función libre** (no método de instancia): por eso namespace. El único estado que toca (`display`, `SIZE`) es global/compartido.
- Nombre a nivel de **intención** (`triangle`/`diamond`), no de primitiva GFX (`fillTriangle`), para que las ventanas no repitan coordenadas relativas.

## 26. Reposo (light sleep)

`SleepManager.h/.cpp` (clase `SleepManager`), con `Config::Power::IDLE_TIMEOUT_MS` (3 min) y los métodos `Display::power(on)`, `Buttons::anyPhysical()`/`ignoreUntilRelease()` y `Engine::isInGame()`/`sleepBlocked()`/`repaint()`. Antes era **función libre** del wiring (`enterSleep()`, `idleTimer`, `diagSeen`); se extrajo a su propia clase por ser un subsistema cerrado (política de inactividad + wake + plataforma + diagnóstico). No es ventana: usa los servicios globales (`display`, `buttons`, `sound`) y recibe `Engine` por referencia. **No bloqueante**: `update()` hace un paso por frame y devuelve `bool`; el wiring llama `sleepManager.begin()` (setup) y `if (!sleepManager.update()) return;` (loop, tras `buttons.read()`).

### Qué hace
- **Fases:** `update()` despacha por un `enum class State` interno: `AWAKE` (cuenta inactividad), `SLEEPING` (una rebanada de light sleep por frame) y `DIAG` (aviso en pantalla por `DIAG_MS`). Devuelve `true` = el frame puede seguir (`engine.*`, `sound.update()`, `display.show()`); `false` = el `loop()` corta con `return` y reinicia. Sin `while` ni `delay`.
- **Disparador (fase `AWAKE`):** en `SleepManager::updateAwake()` (cada frame desde `loop()`, tras `buttons.read()`), `_idle` (un `Stopwatch`) cuenta el tiempo sin botones. Cualquier `buttons.anyPhysical()` lo reinicia (`start()`). Se usa el estado **físico** (no `anyHeld()`, que refleja la supresión `ignoreUntilRelease()`): un pulso mantenido tras el wake sigue contando como actividad. El reposo **no aplica mientras haya una partida o corra el roll de créditos** (`Engine::sleepBlocked()`): en ese caso se hace `_idle.start()` y no se evalúa el vencimiento, así el conteo empieza al acabar el roll. Si **no hay ningún botón presionado** (`!buttons.anyPhysical()`, para no entrar con un pin en HIGH) y llegó a `IDLE_TIMEOUT_MS`, se entra en reposo: `enterSleep()` apaga el OLED (`display.power(false)`, `0xAE`; el framebuffer del panel se conserva) y corta el sonido (`sound.stop()`), pasa a la fase `SLEEPING` y `update()` devuelve `false`.
- **Reposo (fase `SLEEPING`):** `updateSleeping()` hace **una rebanada** de light sleep por frame —`esp_light_sleep_start()` y, al volver, `buttons.read()`— y devuelve `false` (el `loop()` reinicia) mientras siga durmiendo. No se sale hasta que hay un botón **realmente** presionado; si `esp_light_sleep_start()` vuelve "solo" (GPIO por nivel que ya no re-dispara, o host USB-CDC que impide persistir el light sleep), se vuelve a dormir en la rebanada siguiente y la pantalla queda apagada.
- **Diagnóstico del wake espurio:** cada rebanada mide cuánto durmió (`slept = nowMs() - t0`). Un light sleep que no **persistió** vuelve en ~0-5 ms sin botón (`ESP_ERR_SLEEP_REJECT`, o por debajo de `MIN_SLEEP_MS`); en placa sana duerme ~`WAKE_CHECK_MS`. Tras `SPURIOUS_LIMIT` espurios seguidos se pasa a la fase `DIAG`: `showDiag()` enciende el OLED y dibuja pantalla completa —"ERROR" (12x16) en el Header, "SLEEP FAIL" (12x16, invertido) en el cuadro del Menu, "The ESP32 is failing" (6x8, invertido) en el pie— y se marca `_diagSeen`. En `DIAG` el aviso se mantiene `DIAG_MS` **sin `delay()`** (un `Stopwatch _diag`; se siguen haciendo rebanadas de light sleep —panel encendido, la imagen permanece— para no girar en vacío), hasta que vence (apaga el OLED y vuelve a `SLEEPING`) o llega un botón (despierta). El contador de espurios se reinicia con cualquier rebanada "sana".
- **Despertar:** en `begin()` (setup), cada pin de `Config::Pin::BUTTONS` se habilita como fuente de wake con `gpio_wakeup_enable(pin, GPIO_INTR_HIGH_LEVEL)` (botones `INPUT_PULLDOWN`, pulsado = HIGH) y `esp_sleep_enable_gpio_wakeup()` (vía rápida) **más** `esp_sleep_enable_timer_wakeup(WAKE_CHECK_MS)` (garantía: despierta cada 200 ms para re-verificar por polling; con HW sano el botón despierta al instante por GPIO por nivel). Al volver con un botón real, `wake()`: `display.power(true)` (`0xAF`, misma imagen), `buttons.begin()` para **re-anclar el estado** (el botón que despertó no se lee como "press", no navega), `_idle.start()` (no volver a dormir al instante) y vuelve a `AWAKE`. Si hubo diagnóstico (`_diagSeen`), la imagen retenida quedó deformada: se llama `_engine.repaint()` antes de re-anclar los botones.

### Por qué light sleep (no deep sleep)
- **Conserva la RAM:** ventanas y `Storage` siguen vivas; `loop()` continúa donde quedó, sin reinicializar.
- **Cualquier GPIO despierta:** el wake por matriz GPIO admite todos los pines, incluido `GPIO47` (ACTION_LEFT). El deep sleep solo despierta por pines RTC (`EXT0`/`EXT1`) y perdería la RAM.

### API nueva que aporta
- `SleepManager(Engine&)` — el `.ino` la construye con `Engine` (`SleepManager sleepManager(engine);`) y solo llama `begin()`/`update()`.
- `SleepManager::begin()` — fuentes de wake del light sleep (GPIO por nivel + timer de verificación), antes en `setup()`.
- `SleepManager::update()` — un paso **no bloqueante** del reposo; despacha por fase (`AWAKE`→`updateAwake()`, `SLEEPING`→`updateSleeping()`, `DIAG`→`updateDiag()`) y devuelve `bool` (`true` = seguir el frame; `false` = el `loop()` corta con `return`). Quita del `.ino` los globales `idleTimer`/`diagSeen` y las funciones `enterSleep()`/`screenDiag()`. Helpers privados `enterSleep()`, `wake()`, `showDiag()`; miembros `_state` (enum `State`), `_spurious`, `_idle`, `_diag`.
- `Buttons::anyPhysical()` — algún botón físicamente presionado (actividad del contador; ajeno a la supresión `ignoreUntilRelease()`).
- `Buttons::ignoreUntilRelease()` — supresión de entrada hasta soltar todos los botones (el que despertó no cuenta como pulsación sostenida).
- `Engine::isInGame()` — verdadero si hay partida en curso (el reposo no aplica): `_state == State::NEW || _state == State::CONTINUE`.
- `Engine::sleepBlocked()` — verdadero si el reposo no aplica en este momento: `isInGame()` o (`_state == State::MENU_CREDITS` y `_menuCredits.rolling()`). `SleepManager::updateAwake()` lo consulta y mantiene `_idle` a cero mientras dure: el conteo de inactividad empieza al acabar el roll (fase `CARD` o salida con `ACTION_UP`).
- `Config::Power::IDLE_TIMEOUT_MS` (3 min), `Config::Power::WAKE_CHECK_MS` (200 ms; período del wake de verificación), `Config::Power::MIN_SLEEP_MS` (10 ms; por debajo, un retorno sin botón cuenta como espurio), `Config::Power::SPURIOUS_LIMIT` (3 espurios seguidos → diagnóstico), `Config::Power::DIAG_MS` (2 s; duración del aviso).
- `Engine::repaint()` — **repintado forzado** tras un diagnóstico: `display.clear()` puntual + `firstPrint(true)` de la ventana (el `forceRedraw` salta el guard `_clear`). En `MENU_SOUND` redibuja la base del Menu (`_menu.firstPrint(true)`) y encima el selector (`_menuSound.firstPrint(true)`), porque el `loop()` solo llamará al `print()` del parche.
- `Boot::firstPrint(true)` / `Legend::firstPrint(true)` / `Menu::firstPrint(true)` / `MenuCredits::firstPrint(true)` / `Game::firstPrint(true)` — piden el full frame (el `forceRedraw` salta `_clear`). Antes eran `forceRedraw()` (ponía `_clear = true`; el del `Menu` reseteaba `_done`): **comentados**.

### Notas
- `esp_timer` (base de `nowMs()`/`Stopwatch`) sigue avanzando durante el light sleep: el RTC no se detiene, el contador no se corrompe.
- El OLED conserva su RAM con `SSD1306_DISPLAYOFF` (0xAE): al volver con `SSD1306_DISPLAYON` (0xAF) se restaura la misma imagen. La versión instalada de Adafruit_SSD1306 no tiene `displayOn()`/`displayOff()`, así que `Display::power()` usa `ssd1306_command()`.
