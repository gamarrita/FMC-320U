# FLOWMEET Embedded C Style v1.0 (Charter)

## Propósito
Unificar el estilo de firmware en FLOWMEET para el proyecto FMC-320U, de forma que tanto programadores humanos como herramientas de IA puedan aplicar reglas claras, consistentes y verificables.

## Jerarquía de Normas
1. Este documento: **FLOWMEET Embedded C Style v1.0 (Charter)**.
2. Guías internas de detalle:  
   - `STYLE_GUIDE_FIRMWARE_NAMING.md` (nombres).  
   - `STYLE_GUIDE_FIRMWARE_COMMENTS.md` (comentarios, encabezados, secciones).  
   - `STYLE_GUIDE_FIRMWARE_CONCURRENCY.md` (concurrencia e ISR) *(opcional)*.
3. Documentación oficial de STM32Cube/HAL/ThreadX (cuando el prototipo o callback lo requiera).
4. Referencias externas: Barr Group Embedded C, MISRA C:2012 (subset esencial), NASA/JPL C, Google C Style Guide (solo para formato cuando no exista regla interna).
5. `.clang-format` oficial del repositorio (enforcement automático).

## Principios Clave
- **Consistencia sobre preferencia.**
- **Respetar APIs de terceros**: no renombrar HAL/ThreadX.
- **C portátil y seguro**: tipos explícitos (`uint32_t`), evitar casts implícitos.
- **Legibilidad**: 80 columnas, ASCII, voz activa en comentarios.
- **Compatibilidad futura**: reservar espacio en estructuras y registros críticos.

## Estructura de Archivos
- Encabezado obligatorio con Doxygen (`@file`, `@brief`).  
- Secciones internas estándar:  
  `// --- Includes ---`, `// --- Defines ---`, `// --- Types ---`,  
  `// --- Globals ---`, `// --- Static Data ---`, `// --- Public API ---`,  
  `// --- Static Functions ---`, `// --- Interrupts ---`.  
- Cierre obligatorio:  
  ```
  /*** END OF FILE ***/
  ```

## Nombres
- Archivos: `fm_modulo.c`, `fmx_modulo.c`.  
- API pública: `FM_MOD_Action`, `FMX_Action`.  
- Internas estáticas: `CamelCase` sin prefijo.  
- Tipos: `fm_mod_state_t`, `fmx_context_t`.  
- Macros: mayúsculas con prefijo (`FM_LOG_BUFFER_SIZE`).  
- Variables:  
  - Exportadas: `FM_LOG_QueueDepth`.  
  - Estáticas: `static uint8_t pulse_flag;`.  
  - Automáticas/params: `raw_count`.  
- Booleanas: sufijo `_flag`, `_enable`, `_active`.

## Concurrencia y RTOS
- ISRs: no bloquear, no malloc, diferir trabajo a tareas.  
- Comunicación: siempre por `TX_QUEUE` o event flags.  
- Documentar prioridades y tamaños de stack en headers.

## Política de Errores y Logs
- Tipo único `fm_status_t`.  
- No errores silenciosos: log o assert configurable.  
- Para logging persistente: buffer en RAM, flush por bloque, reservar 16 bytes de compatibilidad futura.

## Enforcement
- `.clang-format` obligatorio.  
- `clang-tidy` + `cppcheck` + MISRA subset en CI.  
- Revisión de IA: usar `PROMPT_FIRMWARE_STYLE_EXPERTS.md` + guías de detalle.

## Excepciones
- Módulos heredados mantienen nombres públicos.  
- Excepciones deben documentarse en el código y en `CHANGELOG_FIRMWARE.md`.
