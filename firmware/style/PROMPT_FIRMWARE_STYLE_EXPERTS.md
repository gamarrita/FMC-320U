# Prompt IA - Comentarios y Estilo Firmware FM-320U

## Entradas necesarias
- `FLOWMEET_Embedded_C_Style_Charter.md` (jerarquía y núcleo).
- `STYLE_GUIDE_FIRMWARE_NAMING.md`.
- `STYLE_GUIDE_FIRMWARE_COMMENTS.md`.
- Archivos de ejemplo `fm_style_example.c`, `fm_style_example.h`.
- ARCHIVOS_OBJETIVO a revisar.

## Preparación IA
1. **Síntesis de reglas**: aplicar charter primero, luego naming/comments.
2. **Comprensión**: identificar propósito, dependencias HAL/ThreadX.
3. **Comparar con ejemplos**.

## Análisis
- Riesgos y fallas.
- Concurrencia/RTOS.
- Trazabilidad con requisitos.
- API pública documentada.

## Reescritura
- Mantener funcionalidad.
- Respetar orden includes/bloques.
- Unificar estilo de comentarios.
- Longitud <80 cols.

## Checklist
- Encabezado Doxygen.
- Secciones `// ---`.
- Cierre `/*** END OF FILE ***/`.
- Prefijos correctos.
- Explicación de ISR/colas/mutex.

## Salida requerida
1. Análisis.
2. Cambios propuestos.
3. Archivos actualizados.
4. Notas pendientes.

## Post-proceso
- Revisar diff.
- Compilar rápido.
- Registrar en `CHANGELOG_FIRMWARE.md`.
