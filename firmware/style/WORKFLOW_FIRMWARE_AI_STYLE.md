# WORKFLOW_FIRMWARE_AI_STYLE

Guía operativa para usar IA en normalización de estilo FM-320U.

## Archivos a entregar
- `PROMPT_FIRMWARE_STYLE_EXPERTS.md`.
- `FLOWMEET_Embedded_C_Style_Charter.md`.
- `STYLE_GUIDE_FIRMWARE_NAMING.md`.
- `STYLE_GUIDE_FIRMWARE_COMMENTS.md`.
- Archivos objetivo.
- Ejemplos: `fm_style_example.c`, `fm_style_example.h`.

## Procedimiento
1. Revisar `CHANGELOG_FIRMWARE.md` por cambios de estilo.
2. Preparar paquete con los docs arriba.
3. Ejecutar prompt.
4. Validar análisis/diffs (sin cambios funcionales).
5. Registrar en changelog.

## Buenas prácticas
- Si el archivo depende de HAL/ThreadX, indicarlo en el mensaje.
- Procesar archivos uno por uno.
- Usar las guías más recientes.

## Seguimiento
- Registrar excepciones en issues o guías.
- Mantener CI/CD sincronizado para chequeos automáticos.
