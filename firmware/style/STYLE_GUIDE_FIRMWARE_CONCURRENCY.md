# STYLE_GUIDE_FIRMWARE_CONCURRENCY

## Propósito
Establecer reglas claras para ISR, hilos y comunicación en ThreadX.

## Reglas
- ISR: no malloc, no bloqueos, deferir trabajo a tareas.
- Sin acceso directo a periféricos compartidos: usar colas/event flags.
- Mutex: sólo en tareas, nunca en ISR.
- Prioridades: documentar en header del módulo.
- Timers RTOS: documentar tick, callback y efectos.
- Globals compartidas: siempre protegidas.
