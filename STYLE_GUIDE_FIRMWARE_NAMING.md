# STYLE_GUIDE_FIRMWARE_NAMING

## Jerarquía
1. `FLOWMEET_Embedded_C_Style_Charter.md`.
2. Este documento.
3. `STYLE_GUIDE_FIRMWARE_COMMENTS.md` (formato de archivos).
4. Documentación oficial STM32Cube/HAL/ThreadX.
5. Referencias externas (Barr, MISRA, Google C Style Guide).

## Objetivo
Unificar nombres en firmware FMC-320U.

## Principios
- Mantener prefijos históricos (`FM_`, `FMX_`, `HAL_`, `TX_`).
- Para nuevos módulos: `FM_` (driver) y `FMX_` (aplicación).
- Respetar nombres de HAL/ThreadX.

## Ejemplos
- Archivos: `fm_logger.c`, `fmx_menu.c`.
- Headers: mismo nombre que el `.c`.
- API pública: `FM_LOG_Init()`, `FMX_MenuStart()`.
- Internas estáticas: `UpdateCounter()`.
- Tipos: `fm_log_state_t`, `fmx_menu_context_t`.
- Constantes/macros: `FM_LOG_BUFFER_SIZE`.
- Variables:
  - Global exportada: `FM_LOG_QueueDepth`.
  - Estática: `static uint8_t pulse_flag;`.
  - Local: `sample_total`.
- Banderas: `shutdown_flag`, `enabled_flag`.

## Compatibilidad
- No renombrar callbacks HAL/RTOS (`HAL_GPIO_EXTI_Falling_Callback`).
- Para migración de módulos, usar wrappers.

## Referencias
- STM32Cube AN4998.
- ThreadX User Guide.
- Barr Group Embedded C.
- MISRA C:2012.
