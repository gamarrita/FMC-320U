# WORKFLOW_FIRMWARE_AI_STYLE

Guia operativa para preparar interacciones con IA enfocadas en la normalizacion de estilo del firmware FM-320U.

## Archivos que debes proporcionar a la IA
- `PROMPT_FIRMWARE_STYLE_EXPERTS.md`: prompt consolidado con las instrucciones paso a paso.
- `STYLE_GUIDE_FIRMWARE_COMMENTS.md`: reglas de comentarios, encabezados y cierre de archivos.
- `STYLE_GUIDE_FIRMWARE_NAMING.md`: convenciones de nombres para funciones, tipos y variables.
- Archivo(s) objetivo(s) a revisar o modificar.
- Archivos de ejemplo `fm_style_example.c` y `fm_style_example.h` cuando la IA necesite contexto adicional.

## Procedimiento recomendado
1. **Revisar cambios recientes**: confirma en `CHANGELOG_FIRMWARE.md` si hubo ajustes de estilo que la IA deba conocer.
2. **Preparar el paquete**: recopila los archivos anteriores y comprueba que esten actualizados en la rama de trabajo.
3. **Ejecutar el prompt**: entrega a la IA el contenido de `PROMPT_FIRMWARE_STYLE_EXPERTS.md` seguido de los documentos de estilo y el archivo objetivo.
4. **Validar la respuesta**: revisa el apartado `Analisis` y los diffs propuestos. Verifica que no se hayan introducido cambios funcionales.
5. **Registrar resultados**: si los cambios se aplican, actualiza `CHANGELOG_FIRMWARE.md` en la seccion **Docs** con un resumen.

## Buenas practicas
- Cuando el archivo objetivo usa convenciones heredadas (HAL, ThreadX, STM32Cube), anotalo en el mensaje a la IA para evitar renombrados innecesarios.
- Si hay multiples archivos relacionados, procesa uno por uno para mantener el contexto claro.
- Antes de ejecutar el prompt, asegurate de que las guias de estilo reflejen la decision mas reciente del equipo y respeta la jerarquia indicada en `STYLE_GUIDE_FIRMWARE_NAMING.md`.
- Usa comentarios adicionales en el mensaje inicial cuando el archivo tenga restricciones especiales (memoria compartida, nombres expuestos al linker, etc.).

## Seguimiento
- Las mejoras o excepciones detectadas durante el uso de la IA deben anotarse en una nueva revision de las guias o registrarse mediante issues en el repositorio.
- Mantener este workflow sincronizado con las herramientas CI/CD si en el futuro se automatiza la verificacion de estilo.

