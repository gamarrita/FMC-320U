/*
 *
 * @brief plantilla headers
 *
 * Versión 1
 * Autor: Daniel H Sagarra
 * Fecha: 08/09/2024
 * Modificaciones: version inicial.
 *
 */

#ifndef FM_RTC_H_
#define FM_RTC_H_

// includes

// Typedef y enum.

typedef enum
{
	FM_RTC_SET_YEAR_UNIT = 0,
	FM_RTC_SET_YEAR_TEN,
	FM_RTC_SET_MONTH_UNIT,
	FM_RTC_SET_MONTH_TEN,
	FM_RTC_SET_DAY_UNIT,
	FM_RTC_SET_DAY_TEN,
	FM_RTC_SET_HOUR_UNIT,
	FM_RTC_SET_HOUR_TEN,
	FM_RTC_SET_MINUTE_UNIT,
	FM_RTC_SET_MINUTE_TEN,
	FM_RTC_SET_SECOND_UNIT,
	FM_RTC_SET_SECOND_TEN,
	FM_RTC_SET_END
}fm_rtc_set_t;

// Macros, defines, microcontroller pins (dhs).

// Extrnals, declaradas en fm_module.c y usadas en otros modulos.

// Defines.

// Function prototypes
void FM_RTC_Gets(char *time, char *data);
void FM_RTC_Set(fm_rtc_set_t sel, uint8_t mode);
void FM_RTC_Init();

#endif /* MODULE_H */

/*** end of file ***/
