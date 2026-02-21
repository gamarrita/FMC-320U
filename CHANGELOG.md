# FW_VERSION = PRODUCT_VERSION

### Firmware change log for the FM-320U computer

## [feature log]

### Docs

### Chore

### Added

### Fixed
-   Se completan las funciones basicas para que el equipo guarde en flash los datos de logeos.
    Se decidio no continuar con el MXChip, lo que sigue es un re-factor a un nuevo modulo bluetooth


### Refactor

### Removed

### Power Consumption

### Completed Tests


## [01.01.011-B0]


### Docs
- Ajusto descripciones del arvhivo 100 - Casos de uso.docx

### Chore

### Added

### Fixed
- Rate no se calculaba correctamete, queda mas simple.
- Intercambio las funciones de boton arriba y abajo en el menu TTL-RATE, tiene mas logica.

### Refactor

### Removed

### Power Consumption

### Completed Tests
TESTEO 0x2FB396C8
TESTEO 0x2FB39787
TESTEO 0x2FB397FE
TESTEO 0x2FB398FC
TESTEO 0x2FB40A04


---------------------------------------------------------------------------------------------------

## [01.01.010-B0+2] Beta
(I keep these changes so I can check out other versions; I want to test printing previous versions.)

### Docs
- Updated the administration folder documents.
- Reorganized the style guides in `STYLE_GUIDE_FIRMWARE_COMMENTS.md`, `STYLE_GUIDE_FIRMWARE_NAMING.md`, documented the flow in `WORKFLOW_FIRMWARE_AI_STYLE.md`, prepared `PROMPT_FIRMWARE_STYLE_EXPERTS.md`, and added examples `fm_style_example.c/.h` for use with AI and humans.

### Chore

### Added
- Added a Bluetooth screen for data download; it is not functional yet.

### Fixed
- Changed the ThreadX timer type used to debounce external buttons.

### Refactor
- Moved UART3 from ISR-based handling to DMA.
- Removed the use of multiple sleep times to improve power consumption; a cleaner technique will be used later.
- Created the `fm_usart.c` module to improve code clarity in `fm_mxc`.

### Removed

### Power Consumption
60 uA consumption, conditions:
- Consumo medio de un minuto, alimentado por el PPK2, modo "source meter" a 3V
- ST-LINK desconectado conectado
- Activity blue LED disabled.
- Pickups conectado
    - Si inyecto 100hz + 10uA -> 70uA
    - Si desconecto el pickup -2uA -> 58uA

### Completed Tests
TEST 0x2FB396C8
TEST 0x2FB39787
TEST 0x2FB397FE
TEST 0x2FB397FE
TEST 0x2FB398FC

---------------------------------------------------------------------------------------------------

## [01.01.009] 2025-06-07

### Notes

### Added

### Fixed
- Decimal point was not being cleared.

### Changed

### Removed

### Power Consumption

### Completed Tests

---------------------------------------------------------------------------------------------------

## [01.01.009-B3] 2025-05-27

### Notes

### Added

### Fixed
- Did not allow the factor to be set to its minimum value.

### Changed

### Removed

### Power Consumption

### Completed Tests

---------------------------------------------------------------------------------------------------

## [01.01.009-B2] 2025-05-27

### Notes

### Added
- Production documents folder.

### Fixed
- Fixed a screen change error when entering the setup menu.

### Changed
- Scheduled a 200 ms refresh if a character configured to blink is currently off.
- Changed the RTC read order to call `HAL_RTC_GetTime` before the date; this appears to solve the problem when editing the computer date and time.
- Changed how the '.' segment is processed on the 8-segment characters.

### Removed
- Removed the "how to use CHANGELOG_RULES.md" file because the information was duplicated here.

### Power Consumption

### Completed Tests

---------------------------------------------------------------------------------------------------

## [01.01.009-B0] 2025-05-11

### Notes
- Created a new repository; the project starts here from version 01.01.009.
- A naming convention was used to allow multiple CHANGELOG_XYZ.md files.
- Improved the wording of files related to Git Flow.

### Added
- Project initialization.

### Fixed
- Fixed the RTC write issue caused by not zeroing these structures: `RTC_TimeTypeDef time = {0};` and `RTC_DateTypeDef date = {0};`.

### Changed
- Changed the version presentation to 01.01.009-B0.
- Added the tag folder in administration.
- Integrated how the idle time is computed; it is now controlled from `void ThreadEntryMain(ULONG thread_input)`.
- Modified `CONTRIBUTING.md`.

### Removed
- Removed unrelated documents from the administration folder.

### Power Consumption
76 uA consumption, conditions:
- One-minute average measured with the PPK2.
- ST-LINK 3.0 V.
- With ST-LINK connected.
- Activity blue LED disabled.
- Connected to a signal generator through two pickups in series at 100 Hz.
43 uA consumption, conditions:
- One-minute average measured with the PPK2.
- ST-LINK 3.0 V.
- With ST-LINK connected.
- Activity blue LED disabled.
- Without pickups connected.

### Completed Tests
0x2FB396C8 - OK
0x2FB39787 - OK
0x2FB397FE - OK
0x2FB398FC - OK
0x2FB40A04 - OK

---------------------------------------------------------------------------------------------------

## [UNVERSIONEDED]

### Chore
- Translated comments in `fmx.c` and `fmx.h` to English.

### Added

### Fixed

### Refactor

### Removed

### Power Consumption

### Completed Tests



