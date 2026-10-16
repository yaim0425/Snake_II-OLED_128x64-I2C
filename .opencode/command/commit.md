---
description: Crea un commit siguiendo las reglas del repo (conventional commits en español). Úsalo cuando pidas commitear.
agent: build
---

Vas a **crear un commit** en este repositorio (**Snake_II-OLED_128x64-I2C**).
Sigue estas reglas al pie de la letra.

## Antes de commitear
1. Inspecciona el estado: `git status`, `git diff` y `git log --oneline -10`.
2. Lee **todo** el diff (no solo lo último) para entender qué cambia.
3. Decide qué archivos entran. Si hay cambios no relacionados con el tema del
   commit, avísame y **pregúntame** si incluirlos o separarlos; no los mezcles
   sin avisar.
4. Nunca incluyas secretos (claves, tokens, `.env`) ni artefactos de build.

## Mensaje
- Estilo del repo: **conventional commits en español** → `tipo(scope): resumen`.
  - `tipo`: `feat`, `fix`, `refactor`, `chore`, `docs`, `style`, ...
  - `scope`: clase(s)/archivo(s) afectados, separados por coma, p. ej.
    `MenuCredits,Scroller`.
  - `resumen`: en minúscula, descriptivo y conciso, sin punto final. El repo
    suele omitir tildes (p. ej. "lineas", "geometria").
- Una sola línea de asunto; añade cuerpo solo si aporta (qué y por qué).
- **No** añadas firmas ni "Generated with ...".

## Reglas de git
- Stagea solo los archivos previstos (`git add` específico; `git add -A` solo si
  todo pertenece al mismo tema).
- **No** cambies `git config`.
- **No** uses `--no-verify` ni `--force`, ni amends de un commit ya hecho
  (salvo que te lo pida explícitamente).
- Si un hook falla, arregla el problema y haz un **commit nuevo**; no amends.
- **No** hagas `push` ni crees PR salvo que te lo pida explícitamente.
- Al terminar, muestra el hash + asunto del commit y el `git status` (debe
  quedar limpio).

## Instrucción concreta
$ARGUMENTS
