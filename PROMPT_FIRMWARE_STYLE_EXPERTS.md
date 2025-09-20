# Prompt IA - Comentarios y Estilo Firmware FM-320U

Usa este prompt cada vez que necesites que la IA revise y mejore comentarios, encabezados y nombres en archivos del firmware, siguiendo las guias internas y entregando una narrativa clara para humanos.

## Entradas necesarias
- ARCHIVOS_OBJETIVO: uno o varios archivos C/H completos (por ejemplo `firmware/app/100_main/FLOWMEET/fmx.c`).
- DOCUMENTOS_REFERENCIA: `STYLE_GUIDE_FIRMWARE_COMMENTS.md`, `STYLE_GUIDE_FIRMWARE_NAMING.md`, `fm_style_example.c`, `fm_style_example.h`.
- CONTEXTO_ADICIONAL (opcional): requisitos (`REQ-*`), notas de hardware o fragmentos relacionados que ayuden a entender el flujo.

## Preparacion que debe realizar la IA
1. **Sintesis de reglas**: resumir la jerarquia y las normas aplicables (encabezado Doxygen, secciones `// ---`, cierre `/*** END OF FILE ***/`, prefijos de nombres, limite de 80 columnas, ASCII).
2. **Comprension del archivo**: identificar proposito del modulo, dependencias HAL/ThreadX y puntos criticos (colas, timers, ISR, estados globales).

## Analisis de comentarios y nombres
- **Riesgos y fallas**: describir supuestos de hardware, rebotes, condiciones de fallo y acciones de resguardo.
- **Trazabilidad**: enlazar con requisitos, unidades y motivaciones del valor (por ejemplo REQ-FMX-*).
- **Concurrencia/RTOS**: aclarar por que se usan mutex, colas, semaforos, prioridades o timers.
- **API publica**: asegurar que Doxygen explique `@brief`, `@param`, `@return` con claridad.
- **Nomenclatura**: validar prefijos `FM_`, `FMX_`, tipos `_t`, macros en mayusculas y evitar mezclas HAL/ThreadX.

## Reescritura y edicion
- Mantener el codigo funcional intacto; editar solo comentarios, secciones y espaciado asociado.
- Conservar el orden de includes y bloques existentes.
- Redactar comentarios unificados: sintetizar la mejor explicacion en una sola linea o parrafo, sin listar referentes individuales.
- Prefiere voz activa en presente, espanol neutro sin tildes para `//` y ingles neutro para Doxygen.
- Mantener longitud < 80 caracteres salvo excepciones justificadas (URLs, defines).
- Documentar timers, callbacks, colas y banderas explicando su proposito practico y el impacto en el sistema.

## Formato de salida requerido
1. `Analisis:` lista numerada de hallazgos, cada uno con la regla interna aplicada y la necesidad cubierta.
2. `Cambios propuestos:` resumen por categoria (p. ej. riesgos, trazabilidad, RTOS, nomenclatura).
3. `Archivos actualizados:` bloques completos etiquetados con ```c``` para cada archivo modificado.
4. `Notas pendientes:` (opcional) preguntas, supuestos o recomendaciones adicionales.

## Post-proceso sugerido
1. Revisar el diff para confirmar que solo cambian comentarios/estructura y que se respetan las guias.
2. Ejecutar compilacion rapida o pruebas si se sospecha impacto indirecto.
3. Registrar en `CHANGELOG_FIRMWARE.md` bajo **Docs** las mejoras de documentacion aplicadas.

