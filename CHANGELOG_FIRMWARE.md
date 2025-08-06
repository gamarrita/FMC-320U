# Registro de cambios en el firmware del computador FM-320U

## [01.01.010-B0+2] Beta
(guardo cambios para poder hacer checkout de otras versiones, quiero probar impresion de versiones anteriores)

### Docs
 - Modifico Documentos de carpeta administracion.

### Chore

### Added
- Agrego pantalla de bluetooth para descarga de datos, aun no es funcional.

### Fixed
- Se cambian el tipo de timer del ThreadX usado para evitar rebotes de pulsadores externos.

### Refactory
- Pasor UART3 de trabajar con ISR a DMA
- Elimino el uso de multiples tiempos en sleep para mejorar el consumo. Luego se usara tecnica mas limpia.
- Se crea el modulo fm_usart.c, mejora la claridad de codigo en fm_mxc.

### Removed

### Datos de consumo

### Testeos completados
TESTEO 0x2FB396C8
TESTEO 0x2FB39787
TESTEO 0x2FB397FE
TESTEO 0x2FB397FE
TESTEO 0x2FB398FC

---------------------------------------------------------------------------------------------------

## [01.01.009] 2025-06-07

### Notas

### Added

### Fixed
- Punto decimal no se borraba.

### Changed

### Removed

### Datos de consumo

### Testeos completados

---------------------------------------------------------------------------------------------------

## [01.01.009-B3] 2025-05-27

### Notas

### Added

### Fixed
- No permitia ajustar el factor a su valor minimo.
-

### Changed

### Removed

### Datos de consumo

### Testeos completados

---------------------------------------------------------------------------------------------------

## [01.01.009-B2] 2025-05-27

### Notas

### Added
Carpeta con documentos de produccion

### Fixed
- Error de cambio de pantalla a ingresar al menu setup

### Changed
- Se programa un refresco a 200ms si un caracter programado a parpadear esta apagado.
- Se cambia orden de lectura del RTC, HAL_RTC_GetTime luego date, parece solucionar el promela de edicion de la fecha y hora del computador.
- Cambia la manera en que procesa el segmento '.' en los caracteres de 8 segmentos

### Removed
- Archivo de como usar CHANGELOG_RULES.md, informacion duplicada con esta.

### Datos de consumo

### Testeos completados

---------------------------------------------------------------------------------------------------

## [01.01.009-B0] 2025-05-11

### Notas
- Se crea un nuevo repositorio, aqui se empieza  desde la version 01.01.09
- Se usó una convención de nombres para permitir múltiples archivos CHANGELOG_XYZ.md
- Se mejoro redaccion de archivos relacionados con git flow.

### Added
- Inicialización del proyecto.

### Fixed
- Corrige problema de escritura en el RTC, por no inicializar a cero estas estructuras RTC_TimeTypeDef time = {0}; y  RTC_DateTypeDef date = {0};

### Changed
- Cambio presentacion de version a 01.01.009-B0
- Carpeta tag en administracion.
- Integro en la forma en que se calcula en tiempo a estar en idel, se  controla desde void ThreadEntryMain(ULONG thread_input).
- Modifica archivo CONTRIBUTING.md

### Removed
 - Documentos no correspondientes eliminados de la carpeta de administración.

### Datos de consumo
Consumo 76uA, condiciones:
- Promedio de en un minuto medido con el PPK2
- ST-LINK 3.0v
- Si conectar ST-LINK
- Led azul de actividad deshabilitado
- Conextado a generado de señales por medio de dos pickup acoplados en serie y frecuencia de 100Hz
Consumo 43uA, condiciones:
- Promedio de en un minuto medido con el PPK2
- ST-LINK 3.0v
- Si conectar ST-LINK
- Led azul de actividad deshabilitado
- Sin conectar pickup

### Testeos completados
0x2FB396C8 - OK
0x2FB39787 - OK
0x2FB397FE - OK
0x2FB398FC - OK
0x2FB40A04 - OK

---------------------------------------------------------------------------------------------------

## [UNVERSIONEDED]

### Chore

### Added

### Fixed

### Refoctory

### Removed

### Datos de consumo

### Testeos completados