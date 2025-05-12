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
4. Creás un tag **v01.01.009-beta**
5. Si la rama `release/x.y.z-beta` no fue aprobada en producción, se hacen modificaciones necesaria y se vuelve al punto 1.
6. release/01.01.009-beta → validado por producción
7. Crear nueva rama: release/01.01.009-beta → release/01.01.009
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
