# Changelog

Se registra los cmbios para el firmware, y cambios  que  no 

## [UNVERSIONED]

### Notas

### Added

### Fixed

### Changed

### Removed

### Datos de consumo

### Testeos completados



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


# Tempate

## [UNVERSIONED]

### Notas

### Added

### Fixed

### Changed

### Removed

### Datos de consumo

### Testeos completados