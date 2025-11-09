# STYLE_GUIDE_FIRMWARE_COMMENTS

## Jerarquía
1. `FLOWMEET_Embedded_C_Style_Charter.md`.
2. Este documento.
3. Documentación oficial STM32Cube/HAL/ThreadX.
4. Referencias externas.

## Objetivo
Unificar encabezados, secciones y comentarios.

## Preambulo obligatorio
```c
/**
 * @file nombre_archivo.c
 * @brief Resumen conciso en presente.
 *
 * Contexto: dependencias, restricciones, supuestos.
 */
```

## Secciones estándar
- `// --- Includes ---`
- `// --- Defines ---`
- `// --- Types ---`
- `// --- Globals ---`
- `// --- Static Data ---`
- `// --- Public API ---`
- `// --- Static Functions ---`
- `// --- Interrupts ---`

## Funciones
- Públicas: Doxygen con `@brief`, `@param`, `@return`.
- Internas: `//` breve antes de la definición.
- Inline comments: `//`, no `/* */`.

## Estilo
- Voz activa, español neutro sin tildes.
- Máx 80 columnas.
- ASCII plano.

## Cierre
Todos los archivos terminan con:
```
/*** END OF FILE ***/
```
