# STYLE_GUIDE_FIRMWARE_COMMENTS

Guia de estilo para comentarios y encabezados en los modulos de firmware del proyecto FM-320U.

## Objetivo
- Entregar una referencia unica para homogeneizar encabezados, secciones y cierres de archivos C y H.
- Mantener comentarios profesionales, legibles y alineados con las guias de firmware de tiempo real.
- Asegurar que cualquier herramienta (incluyendo IA) pueda seguir un mismo formato sin ambiguedades.

## Preambulo obligatorio
Cada archivo C o H debe iniciar con un bloque Doxygen que describa el archivo y su proposito general. Plantilla sugerida:

```c
/**
 * @file nombre_archivo.c
 * @brief Resumen conciso en presente.
 *
 * Contexto adicional: que hace el modulo, dependencias clave, restricciones o supuestos.
 */
```

Reglas:
- `@file` usa el nombre real del archivo.
- `@brief` se limita a una frase corta (maximo 80 caracteres).
- El bloque termina con una linea en blanco antes del primer `#include`.

## Secciones internas estandar
Utiliza encabezados con doble guion para marcar bloques de codigo. Las secciones mas comunes son:
- `// --- Includes ---`
- `// --- Defines ---` o `// --- Module Defines ---`
- `// --- Types ---`
- `// --- Globals ---`
- `// --- Static Data ---`
- `// --- Public API ---`
- `// --- Static Functions ---`
- `// --- Interrupts ---`

Mantente consistente con el mismo titulo cuando reaparezcan en archivos relacionados. Evita crear variaciones superficiales (por ejemplo `// --- Internal state ---` y `// --- Internal State ---`).

## Documentacion de funciones
- Funciones expuestas en headers o utilizadas fuera del modulo: documentalas con Doxygen.
  ```c
  /**
   * @brief Accion principal en presente.
   * @param param Descripcion con unidades si aplica.
   * @return Resultado o codigo de error esperado.
   */
  ```
- Funciones estaticas o acciones triviales: utiliza comentarios de una linea `//` previos a la definicion para explicar el por que.
- No dupliques informacion entre header y fuente; el header debe contener el bloque principal y el C puede incluir notas adicionales cuando sea necesario.

## Comentarios en linea y bloques
- Mantiene voz activa, idioma neutro sin acentos y limite de 80 columnas como se describe en esta guia. Las herramientas IA consultan `PROMPT_FIRMWARE_STYLE_EXPERTS.md` como resumen operativo.
- Prefiere `// Comentario` para una sola idea. Evita `/* */` salvo que necesites un bloque multilinea local.
- Los comentarios descriptivos de estructuras y enums se ubican inmediatamente arriba del elemento que documentan.

## Cierre del archivo
Todos los archivos deben finalizar con:
```
/*** END OF FILE ***/
```
colocado en la ultima linea, sin espacio extra despues. No agregues comentarios de cierre personalizados.

## Referencias
- Barr Group Embedded C Coding Standard: https://barrgroup.com/embedded-systems/books/embedded-c-coding-standard
- NASA Jet Propulsion Laboratory C Coding Standard: https://software.nasa.gov/docs/C_Coding_Standard_Rev1.0.pdf
- MISRA C:2012 Guidelines for the Use of the C Language in Critical Systems
- ThreadX User Guide para convenciones RTOS (consulta la documentacion incluida con el BSP)

