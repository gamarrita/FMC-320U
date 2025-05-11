# Instructivo para mantener el archivo CHANGELOG.md

## 📌 Propósito

Este archivo documenta los **cambios funcionales, correcciones y pruebas realizadas** en cada versión del proyecto FMC-320U, así como datos técnicos relevantes para trazabilidad y validación.

---

## 📁 Estructura del archivo

Cada sección comienza con una cabecera de versión:

```
## [vX.Y.Z] - AAAA-MM-DD
```

O, si la versión aún no fue liberada:

```
## [UNRELEASED]
```

Dentro de cada versión, usar las siguientes categorías:

| Sección                 | ¿Qué contiene?                                                      |
|-------------------------|---------------------------------------------------------------------|
| `### Added`             | Funcionalidades nuevas agregadas                                   |
| `### Fixed`             | Errores corregidos                                                 |
| `### Changed`           | Comportamientos modificados o refactors con impacto funcional      |
| `### Removed`           | Funciones o archivos eliminados                                    |
| `### Notas`             | Anotaciones contextuales sobre el estado o decisiones técnicas     |
| `### Datos de consumo`  | Resultados de pruebas de corriente, condiciones de medición        |
| `### Testeos completados` | Casos de prueba realizados manualmente con sus resultados        |

---

## ✍️ Cómo mantener actualizado el changelog

1. **Durante el desarrollo (rama `release/` o `feature/`)**
   - Ir agregando ítems bajo `## [UNRELEASED]` según el tipo de cambio.
   - Si es beta, se puede usar una sección: `## [vX.Y.Z-beta]`.

2. **Al momento de hacer el tag**
   - Reemplazar `## [UNRELEASED]` por la versión y la fecha:
     ```
     ## [v01.01.09] - 2025-05-05
     ```

3. **No incluir commits menores**
   - Solo cambios relevantes para el comportamiento, mantenimiento técnico o validación.

4. **Formato recomendado**
   - Frases breves, en infinitivo (ej: “Corregir bug de inicialización”).
   - Listas claras, no párrafos extensos.

---

## 🧠 Consejo

Usá el comando:

```
git log --pretty=format:"- %s"
```

para listar títulos de commits recientes y ayudarte a completar el changelog.
