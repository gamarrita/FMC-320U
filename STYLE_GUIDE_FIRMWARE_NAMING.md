# STYLE_GUIDE_FIRMWARE_NAMING

Jerarquia de normas (orden de prevalencia):
1. Este documento define las reglas internas obligatorias del proyecto.
2. `STYLE_GUIDE_FIRMWARE_COMMENTS.md` establece formato de archivo (encabezado, secciones, cierre).
3. Documentacion oficial de STM32Cube/HAL/ThreadX cuando un simbolo o callback exige un nombre especifico.
4. Google C Style Guide y otras referencias publicas como apoyo cuando no exista regla previa.

Instrucciones para uso con IA:
- Provee este archivo junto a `STYLE_GUIDE_FIRMWARE_COMMENTS.md`, `PROMPT_FIRMWARE_STYLE_EXPERTS.md` mas los archivos de ejemplo `fm_style_example.c` y `fm_style_example.h` descritos en `WORKFLOW_FIRMWARE_AI_STYLE.md`.
- La IA debe respetar esta jerarquia; si detecta conflicto con normas externas, justificar el criterio adoptado en la salida.
- Siempre confirmar que el archivo resultante mantiene el encabezado Doxygen de apertura y el cierre `/*** END OF FILE ***/`.


Referencia para nombres de archivos, funciones, variables y tipos dentro del firmware FM-320U.

## Objetivo
- Unificar el estilo de identificadores provenientes de STM32Cube, HAL, ThreadX y desarrollos propios.
- Facilitar el trabajo con herramientas automaticas que dependan de convenciones claras.
- Permitir migraciones graduales sin reescribir modulos heredados.

## Principios
- Mantener los prefijos historicos (`FM_`, `FMX_`, `HAL_`, `TX_`) cuando vinculan al modulo externo que provee la API.
- Para piezas nuevas del proyecto usar prefijos `FM_` (driver/servicio) o `FMX_` (aplicacion FLOWMEET) segun corresponda.
- Seguir la guia de nombres documentada en STM32Cube MCU Package (cuando aplique) y ajustar con recomendaciones de Google C Style Guide para claridad en estructuras y variables locales.


## Ejemplos de referencia
- Consulta `fm_style_example.h` y `fm_style_example.c` para ver la aplicacion completa de estas reglas en headers y fuentes.
- Ambos archivos incluyen prefijos, callbacks HAL, uso de ThreadX y patrones de variables globales que la IA debe imitar.

## Archivos
- Fuentes `.c`: `fm_modulo.c`, `fmx_modulo.c` segun dominio. Usa minusculas y guion bajo.
- Headers `.h`: mismo nombre que la fuente.
- Ficheros relacionados (ej. `fm_log_policy.md`) deben compartir el prefijo del modulo que documentan.

## Funciones
- API publica (expuesta en `.h`): `FM_MOD_Action`, `FMX_Action`. Usa mayusculas en el prefijo y CamelCase en la parte descriptiva.
- Funciones externas de terceros (HAL/ThreadX): conserva el nombre provisto (`HAL_GPIO_Init`, `tx_thread_create`). No rebautizar wrappers generados.
- Funciones estaticas internas: CamelCase iniciando en mayuscula (`PulseUpdate`, `TimerEntryBackLightOff`). Evita prefijos `FM_` para resaltar que no se exportan.
- Callbacks asociados a HAL/RTOS deben seguir el prototipo oficial (`void HAL_GPIO_EXTI_Falling_Callback(...)`). Documenta la relacion en comentarios.

## Tipos
- `typedef enum`: `fm_modulo_state_t` (minusculas con sufijo `_t`). Cada enumerador en mayusculas con prefijo de modulo (`FM_MOD_STATE_IDLE`).
- `typedef struct`: `fm_modulo_context_t`, `fmx_menu_screen_t`. Miembros en minusculas con guion bajo (`uint16_t pulse_count`).
- `typedef` para punteros o alias: evita mayusculas completas salvo que sea heredado.

## Constantes y macros
- `#define`: mayusculas con guion bajo y prefijo (`FM_CMD_BYTE_SIZE`). Para macros locales de archivo, antepone `_LOCAL` si solo aplica dentro (`FM_LOG_LOCAL_BUFFER_LEN`).
- Literales numericas con sufijo (`16u`, `4UL`) segun el tipo esperado.

## Variables
- Globales exportadas: `uint32_t FM_LOG_QueueDepth;` preferentemente expone getters en vez de variables.
- Globales internas (`static`): minusculas con guion bajo (`static uint8_t pulse_flag;`). Prefijo `s_` opcional solo cuando mejora claridad en archivos con alta densidad de globals.
- Variables automaticas: minusculas con guion bajo (`rate_tick_new`).
- Handles de HAL/RTOS: mantener convenciones de la libreria (`UART_HandleTypeDef huart1`, `TX_THREAD main_thread`).

## Parametros y miembros
- Parametros de funcion: minusculas con guion bajo (`const char *args`).
- Structs de configuracion: agrupa miembros por contexto y documenta con comentarios cortos si el nombre no es autoexplicativo.

## Estados y banderas
- Banderas booleanas: sufijo `_flag`, `_enable`, `_active`. Valores simbolicos `TRUE`/`FALSE` o `0u/1u` segun el archivo; no mezclar dentro del mismo modulo.
- Timers y contadores: sufijo `_timer`, `_count`, `_ticks`.

## Conversion y compatibilidad
- Al migrar modulos heredados, respeta el nombre publico e introduce wrappers con el estilo nuevo si es necesario.
- No renombres callbacks o simbolos requeridos por el linker (ej. vectores de interrupcion) salvo que el proveedor lo permita.

## Referencias
- STM32Cube Firmware coding rules (STM32 MCU Package, Application Note AN4998).
- ThreadX Naming Conventions (Azure RTOS ThreadX User Guide, capitulo "Coding Conventions").
- Google C++ Style Guide, secciones "Naming" y "Type names" (aplicadas selectivamente a C).

