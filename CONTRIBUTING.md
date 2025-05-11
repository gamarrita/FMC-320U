# CONTRIBUTING

Este archivo documenta los cambios en el proyecto FMC-320U.


## Convenciones de nombres de ramas

Utilizá los siguientes prefijos para indicar el propósito de cada rama:

| Prefijo       | Propósito                                             | Ejemplo                        |
|---------------|-------------------------------------------------------|--------------------------------|
| `main`        | Código estable listo para producción                  | -                              |
| `develop`     | Integración de funcionalidades antes de la liberación | -                              |
| `feature/`    | Desarrollo de nuevas funcionalidades                  | `feature/eco-mode`             |
| `bugfix/`     | Corrección de errores menores                         | `bugfix/led-flicker`           |
| `hotfix/`     | Corrección urgente en producción                      | `hotfix/fix-reboot-issue`      |
| `refactor/`   | Reestructuración del código sin cambiar funcionalidades | `refactor/init-logic`        |
| `test/`       | Pruebas o experimentos                                | `test/lptim3-capture`          |
| `release/`    | Preparación de una versión para liberación            | `release/1.1.9-beta`           |

Nota 1: en la rama main solamente se deberian en equipos de desarrollos de varias personas, desde la 

Nota 1 (DHS): en equipos de desarrollos de varias personas, desde la 

## Convencion nombres de commits:
- `feat`: Nueva funcionalidad
- `fix`: Corrección de errores
- `refactor`: Reestructuración del código
- `test`: Adición o modificación de pruebas
- `docs`: Cambios en la documentación
- `chore`: Tareas menores o mantenimiento

## Ejemplo de fluho de trabajo

# Flujo de trabajo para versiones beta y finales (Git Flow adaptado)

Este documento describe el flujo completo para preparar una versión `beta` y una versión final (`stable`) siguiendo un modelo similar a Git Flow, usando las ramas `develop`, `release` y `main`.

---

## Se crea una desde devlop

La rama que puede ser feature/xyz, release/xyz, ... etc.
En un proyecto grande quizas varios desarrolladores esten desarrollando varios features al mismo tiempo, uno a uno hace merge a develop, luego se crea una rama release.
Para este proyecto se puede crear la rama release directamente.

## Fase de testeo (versión beta)

Estás en la rama: `release/01.01.009-beta` (en nuestro caso ya con varios commits)

1. Realizás pruebas físicas y ajustes en la rama `release/01.01.009-beta`.
2. Actualizás parcialmente el archivo `CHANGELOG.md`.
3. Hacés un commit con esos cambios.
4. Creás un tag **beta**:

   ```bash
   git tag -a v01.01.009-beta -m "Versión beta 01.01.009 para pruebas físicas"
   git push origin v01.01.009-beta
   ```

> Este tag se utiliza para compartir con testers o implementar en entornos de prueba (staging).

---

## ✅ Preparación de la versión final (estable)

5. Realizás los últimos ajustes tras el testeo beta.
6. Completás el `CHANGELOG.md` con la información definitiva.
7. Hacés un commit final, por ejemplo: `chore: versión estable 01.01.009`.
8. Creás el tag **estable**:

   ```bash
   git tag -a v01.01.009 -m "Versión estable 01.01.009"
   git push origin v01.01.009
   ```

> El tag puede hacerse en esta etapa **en la rama `release`** si estás seguro del commit final, o bien en `main` después del merge. Ambas opciones son válidas (ver sección más abajo).

---

## 🔀 Cierre de la release: orden correcto de merges

9. Mergeás la rama `release/01.01.009-beta` en `main`:

   ```bash
   git checkout main
   git merge release/01.01.009-beta
   ```

   > Este paso marca oficialmente la publicación de la versión final.

10. Mergeás la misma rama en `develop`:

   ```bash
   git checkout develop
   git merge release/01.01.009-beta
   ```

   > Así `develop` queda actualizado con los últimos fixes aplicados durante el ciclo de release.

11. Eliminás la rama `release` (opcional):

   ```bash
   git branch -d release/01.01.009-beta           # local
   git push origin --delete release/01.01.009-beta  # remoto
   ```

---

## 📌 ¿Dónde conviene crear el tag de versión final?

Ambas opciones son válidas, según el contexto de tu proyecto.

### Opción 1: **Tag en la rama `release` (antes del merge)**

```bash
git checkout release/01.01.009-beta
git tag -a v01.01.009 -m "Versión estable 01.01.009"
```

- Recomendado si seguís Git Flow clásico.
- El commit taggeado se verá reflejado también en `main` luego del merge (si es fast-forward).

### Opción 2: **Tag en la rama `main` (después del merge)**

```bash
git checkout main
git tag -a v01.01.009 -m "Versión estable 01.01.009"
```

- Útil si tu despliegue a producción ocurre **solo desde `main`**.
- Garantiza que el tag está exactamente en la rama de producción.

---

## 📘 Ejemplo de `CHANGELOG.md`

```markdown
# Changelog

Todas las versiones importantes del proyecto se documentan aquí.

## [Unreleased]

### Added
- (Pendiente de futuras funcionalidades)

---

## [01.01.009-beta] - 2025-05-05

### Added
- Validaciones visuales en equipo durante el testeo físico.
- Prueba de integración general del firmware.
- Mejoras menores en presentación de versión.

### Changed
- Estructura del changelog actualizada.

---

## [01.01.009] - 2025-05-10

### Added
- Correcciones tras testeo de campo.
- Validaciones finales integradas.
```

---

Este flujo es flexible y puede adaptarse a distintos modelos de desarrollo, pero asegura claridad, trazabilidad y control en tus entregas.


Tenés que crear un nuevo tag?
Sí, deberías.
Cada tag representa un snapshot específico, así que si cambia el código, el tag debe cambiar para no generar confusión.

🤔 ¿Cómo versionarlo? ¿Y qué pasa con 01.01.009?
Opción A: seguir usando el mismo número con sufijos beta
Si aún estás dentro del mismo ciclo de versión 01.01.009, podés usar:

v01.01.009-beta.1

v01.01.009-beta.2

v01.01.009-beta.3

...

v01.01.009 (versión final)

✅ Esto es lo recomendado. Así mantenés la coherencia: todos esos tags son parte del mismo ciclo de release 01.01.009.

Opción B: saltar directamente a 01.01.010-beta
Esto se haría solo si decidís abandonar completamente la versión 01.01.009.

🔴 Pero: si nunca existió v01.01.009 final, entonces quedaría como un ciclo incompleto. Técnicamente válido, pero poco prolijo.

✅ Recomendación profesional
Si el bug fue encontrado durante la fase beta y todavía no se hizo merge a main ni se publicó una final estable, lo mejor es:

bash
Copy
Edit
git tag -a v01.01.009-beta.2 -m "Segunda beta con corrección de bug"
Y cuando esté todo bien:

bash
Copy
Edit
git tag -a v01.01.009 -m "Versión estable 01.01.009"
