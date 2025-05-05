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
  FM_RTC_SET_YEAR,
  FM_RTC_SET_MONTH,
  FM_RTC_SET_DAY,
  FM_RTC_SET_HOUR,
  FM_RTC_SET_MINUTE,
  FM_RTC_SET_SECOND,
  FM_RTC_SET_END
} fm_rtc_set_t;

// Macros, defines, microcontroller pins (dhs).

// Extrnals, declaradas en fm_module.c y usadas en otros modulos.

// Defines.

// Function prototypes
void FM_RTC_GetPpt(char *time_str, char *date_str);
void FM_RTC_Gets(char *time, char *data);
uint32_t FM_RTC_GetUnixTime(void);
void FM_RTC_Set(fm_rtc_set_t sel, uint8_t mode);
void FM_RTC_Init();

#endif /* MODULE_H */

/*** end of file ***/
