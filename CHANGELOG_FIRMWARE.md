# Changelog

Se registra los cmbios para el firmware, y cambios  que  no 

## [UNRELEASED]

### Notas
- Se crea un nuevo repositorio, aqui se empieza  desde la version 01.01.09
- Se uso una concencion de nombres  para poder soportar multiples archivos  CHANGELOG_XYZ-md
- Se mejoro redaccion de archivos relacionados con git flow.

### Added
- Inicialización del proyecto.

### Fixed
- Corrige problema de escritura en el RTC, por no inicializar a cero estas estructuras RTC_TimeTypeDef time = {0}; y  RTC_DateTypeDef date = {0};

### Changed
- Cambio presentacion de version a 01.01.009-B0
- Carpeta tag en administracion.
- Integro en la forma en que se calcula en tiempo a estar en idel, se  controla desde void ThreadEntryMain(ULONG thread_input).
- Bug en fm_rtc, 

### Removed
 - Documendos nos correcpodes en carpeta administracion ta

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
0x2FB398FC - 

