# CHANGELOG

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

Enun proyecto grande, desde el main se pueden crear multiples ramas, del tipo feature, estas ramas se van haciendo merge al main-
Con varios features terminados se crea una rama realse, para crear una nueva version, 


## Convencion nombres de commits:
- `feat`: Nueva funcionalidad
- `fix`: Corrección de errores
- `refactor`: Reestructuración del código
- `test`: Adición o modificación de pruebas
- `docs`: Cambios en la documentación
- `chore`: Tareas menores o mantenimiento


## Estilo de este proyecto para crear bnuevas versiones

