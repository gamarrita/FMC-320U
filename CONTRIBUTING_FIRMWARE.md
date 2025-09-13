# CONTRIBUTING

Este archivo documenta los procedimiento para hacer commits en el proyecto FMC-320U.

## Identificacion del commit
- Para commits que tengan que ver con el firmware usar el siguiente formato para el titulo "[FIRMWARE] Prefijo - Descripcion"
- Ejemplo [FIRMWARE] Chore - Mejora en comentarios del codigo"


## Convenciones de nombres de ramas

Utilizá los siguientes prefijos para indicar el propósito de cada rama:

| Prefijo     | Propósito                                  | Ejemplo de nombre de rama     |
| ----------- | ------------------------------------------ | ----------------------------- |
| `feat/`     | Nueva funcionalidad                        | `feat/logger-flash`           |
| `fix/`      | Corrección de bug no crítico               | `fix/uart3-idle-dma`          |
| `hotfix/`   | Corrección urgente en producción           | `hotfix/boot-crash`           |
| `refactor/` | Reestructuración sin cambiar funcionalidad | `refactor/boot-init-order`    |
| `perf/`     | Optimización de performance / consumo      | `perf/stop2-batch-flush`      |
| `test/`     | Código de pruebas / experimentos           | `test/logger-powerloss`       |
| `docs/`     | Documentación, requerimientos, specs       | `docs/rfc-001-logging-policy` |
| `release/`  | Preparación de una versión                 | `release/1.1.9`               |
| `ci/`       | Cambios en CI/CD                           | `ci/werror-static-analysis`   |
| `build/`    | Configuración de build / toolchain         | `build/linker-noinit`         |



## Convencion nombres de commits:

[DOMINIO] type(scope): mensaje en imperativo

DOMINIO (corchetes): 
- [FIRMWARE]
- [HARDWARE]
- [DOCS]
- [CI]
- [BUILD]
- [TEST]
- [MECHANICAL]
- [SECURITY]
- [TOOLING]
- [RELEASE]

Type:
- `feat`: Nueva funcionalidad
- `fix`: Corrección de errores
- `refactor`: Reestructuración del código
- `test`: Adición o modificación de pruebas
- `docs`: Cambios en la documentación
- `chore`: Tareas menores o mantenimiento

Scope (un módulo/subsistema concreto):
- logger
- uart3
- boot
- pcb
- spec

## Etiquetas antes del commit:

| Ejemplo de commit                                                          | Cuándo usar                     |
| -------------------------------------------------------------------------- | ------------------------------- |
| `[FIRMWARE] feat(logger): add RAM buffer + block-flush policy`             | Nueva funcionalidad en firmware |
| `[FIRMWARE] fix(uart3): handle IDLE + DMA circular to prevent stalls`      | Corrección de bug               |
| `[FIRMWARE] perf(power): reduce STOP2 wakeups by batching flushes`         | Optimización de consumo         |
| `[FIRMWARE] refactor(boot): split init into early/late to simplify MXC`    | Refactor sin cambio funcional   |
| `[DOCS] docs(spec): RFC-001 logging policy (record layout + triggers)`     | Documentación o requerimientos  |
| `[HARDWARE] fix(bom): change flash to 100k cycles part per endurance calc` | Cambios en BoM/hardware         |
| `[CI] ci(build): enable -Werror and add static analysis`                   | Ajustes en CI/CD                |
| `[RELEASE] chore(version): bump to v1.1.9-beta.1`                          | Cambio de versión / release     |



## Situaciones comunes donde se necesita un nuevo commit, procedimientos:
- Situacion: se necesita crear una nueva version beta, produccion detecto errores en la beta que esta testeando.
- Agregá sección `[UNRELEASED]` en `CHANGELOG.md`
- Hacé un commit tipo `chore:` o `docs:`
- Ejemplo: `chore: inicia sección UNRELEASED tras erroes que euncuentra produccion en v01.01.009-B0`

- Situacion: cambios intermedios dentro de la rama release/firmware-x.y.z-beta
- Durante testeo y validaciones:
- Usá `test:` para registrar lo que validaste
- Usá `fix:` para errores encontrados durante pruebas

- Situacion: liberar beta para produccion:
- Hacer un commit con el nombre 'release: abc' antes de hacer el tag
- Ejemplo: `release beta: v01.01.009 beta 0 para que teste producción`

- Situacion: liberar una versión:
- Hacer un commit con el nombre 'release: abc' antes de hacer el tag
- Ejemplo: `release: cierra v01.01.009 para producción`


# Flujo de trabajo para versiones beta y finales (Git Flow adaptado)

Este documento describe el flujo completo para preparar una versión `beta` y una versión final (`stable`) siguiendo un modelo similar a Git Flow, usando las ramas `develop`, `release` y `main`.

## Se crea una desde devlop

La rama que puede ser feature/xyz, release/xyz, ... etc.
En un proyecto grande quizas varios desarrolladores esten desarrollando varios features al mismo tiempo, uno a uno hace merge a develop, luego se crea una rama release.
Para este proyecto se puede crear la rama release directamente.

## Fase de testeo (versión beta)

Estás en la rama: `release/01.01.009-B0` (en nuestro caso ya con varios commits)

1. Realizás pruebas físicas y ajustes en la rama `release/01.01.009-B0`, hasta el final, donde decides que lo pruebe producción.
2. Actualizás el archivo `CHANGELOG.md`, pasa de[UNVERSIONED] al usar el nombre de la rama y fecha: [01.01.009-B0] [YY-MM-DD].
3. Hacés un commit con esos cambios.
4. Creás un tag **v01.01.009-B0**
5. Si la rama `release/01.01.009-B0` no fue aprobada en producción, desde el tag se cambia se hacen [01.01.009-B0] a [UNVERSIONED] volvemos al punto 1.
6. release/01.01.009-BX → validado por producción
7. Crear nueva rama: release/01.01.009-BX → release/01.01.009 (BX representa las iteraciones con produccion, B0 si no encontro problemas en el primer beta)
8. Cambio menores
9. Commit con titulo:  prepara v01.01.009-beta
10. Mergeás a main
11. Creás el tag v01.01.009
12. Mergeás también a develop

develop
   ▲
   │
   └─ release/01.01.009-beta ──▶ release/01.01.009 ──▶ main (tag v01.01.009)

>Alternativa, en lugar de crear la rama release/01.01.009 desde la beta, se podria pasar primero por develop, luego crear la rama release, esto  es util en proyectos mas  grande.

14. Eliminás la rama `release` (opcional):
