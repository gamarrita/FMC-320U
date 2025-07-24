# CONTRIBUTING

Este archivo documenta los procedimiento para hacer commits en el proyecto FMC-320U.

## Identificacion del commit
- Para commits que tengan que ver con el firmware usar el siguiente formato para el titulo "[FIRMWARE] Prefijo - Descripcion"
- Ejemplo [FIRMWARE] Chore - Mejora en comentarios del codigo"


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


## Convencion nombres de commits:
- `feat`: Nueva funcionalidad
- `fix`: Corrección de errores
- `refactor`: Reestructuración del código
- `test`: Adición o modificación de pruebas
- `docs`: Cambios en la documentación
- `chore`: Tareas menores o mantenimiento

## Etiquetas antes del commit:
- Usar [FIRMWARE]       como etiqueta en titulo de commit para firmware
- Usar [HARWARE]        como etiqueta en titulo de commit para harware
- Usar [ADMINISTRACION] como etiqueta en titulo de commit para administracion
- Usar [BIBLIOTECA]     ...
- Usar [GLOBAL]         como etiqueta de commit de cambio en multiples carpetas


## Situaciones comunes donde se necesita un nuevo commit, procedimientos:
- Situacion: se necesita crear una nueva version beta, produccion detecto errores en la beta que esta testeando.
- Agregá sección `[UNRELEASED]` en `CHANGELOG.md`
- Hacé un commit tipo `chore:` o `docs:`
- Ejemplo: `chore: inicia sección UNRELEASED tras erroes que euncuentra produccion en v01.01.009-B0`

- Situacion: cambios intermedios dentro de la rama release/firmware-x.y.z-beta
- Durante testeo y validaciones:
- Usá `test:` para registrar lo que validaste
- Usá `fix:` para errores encontrados durante pruebas

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
