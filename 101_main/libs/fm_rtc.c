/*
 * Author: DHS
 *
 * @brief plantilla codigo.
 *
 * Versión 1
 * Autor: Daniel H Sagarra
 * Fecha: 08/09/2024
 * Modificaciones: version inicial
 * 
 */

// Includes.
#include "fm_debug.h"
#include "fm_rtc.h"

// Typedef.

// Const data.

// Defines.

// Debug.

// Project variables, non-static, at least used in other file.

// External variables.
extern RTC_HandleTypeDef hrtc;

// Global variables, statics.

// Private function prototypes.

// Private function bodies.

// Public function bodies.

/*
 * 	@brief	devuelve fecha y ahora en dos strings.
 * 	@param	time_str, puntero a string, aquí se escribe la hora.
 * 	@param 	date_str, puntero a string, aquí se escribe la fecha.
 */
void FM_RTC_Gets(char *time_str, char *date_str)
{
	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;

	HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

	sprintf(time_str, "%02d.%02d.%02d ", time.Hours, time.Minutes, time.Seconds);
	sprintf(date_str, "%02d.%02d.20%02d ", date.Date, date.Month, date.Year);
}

/*
 * @brief		modifica el reloj de tiempo real.
 * @note		El RTC codifica en BCD fecha y hora. Uso hexadecimal en asignaciones y operacinoes con los
 * 					registros del RTC para resaltar el hecho que son BCD.
 *
 * @param		sel, selecciona que es lo que se va a modificar de la fecha y hora actual.
 * 					mode, 0 decrementa, 1 incrementa.
 * @retval	ninguno.
 */
void FM_RTC_Set(fm_rtc_set_t sel, uint8_t mode)
{

	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;

	HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BCD);
	HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BCD);

	switch (sel)
	{
		case FM_RTC_SET_YEAR_UNIT:
			if (mode)
			{
				if ((date.Year & ((uint8_t) 0x0FU)) < ((uint8_t) 0x09U))
				{
					date.Year += ((uint8_t) 0x01U);
				}
			}
			else
			{
				if ((date.Year & ((uint8_t) 0x0FU)) > ((uint8_t) 0x00U))
				{
					date.Year -= ((uint8_t) 0x01U);
				}
			}
			break;
		case FM_RTC_SET_YEAR_TEN:
			if (mode)
			{
				if ((date.Year & ((uint8_t) 0xF0U)) < ((uint8_t) 0x90U))
				{
					date.Year += ((uint8_t) 0x10U);
				}
			}
			else
			{
				if ((date.Year & ((uint8_t) 0xF0U)) > ((uint8_t) 0x00U))
				{
					date.Year -= ((uint8_t) 0x10U);
				}
			}
			break;
		case FM_RTC_SET_MONTH_UNIT:
			if (mode)
			{
				if ((date.Month & ((uint8_t) 0x0FU)) < ((uint8_t) 0x09U))
				{
					date.Month += ((uint8_t) 0x01U);
				}
			}
			else
			{
				if ((date.Month & ((uint8_t) 0x0FU)) > ((uint8_t) 0x00U))
				{
					date.Month -= ((uint8_t) 0x01U);
				}
			}
			break;
		case FM_RTC_SET_MONTH_TEN:
			if (mode)
			{
				if ((date.Month & ((uint8_t) 0xF0U)) < ((uint8_t) 0x10U))
				{
					date.Month += ((uint8_t) 0x10U);
				}
			}
			else
			{
				if ((date.Month & ((uint8_t) 0xF0U)) > ((uint8_t) 0x00U))
				{
					date.Month -= ((uint8_t) 0x10U);
				}
			}
			break;
		case FM_RTC_SET_DAY_UNIT:
			if (mode)
			{
				if ((date.Date & ((uint8_t) 0x0FU)) < ((uint8_t) 0x09U))
				{
					date.Date += ((uint8_t) 0x01U);
				}
			}
			else
			{
				if ((date.Date & ((uint8_t) 0x0FU)) > ((uint8_t) 0x00U))
				{
					date.Date -= ((uint8_t) 0x01U);
				}
			}
			break;
		case FM_RTC_SET_DAY_TEN:
			if (mode)
				{
					if ((date.Date & ((uint8_t) 0xF0U)) < ((uint8_t) 0x30U))
					{
						date.Date += ((uint8_t) 0x10U);
					}
				}
				else
				{
					if ((date.Date & ((uint8_t) 0xF0U)) > ((uint8_t) 0x00U))
					{
						date.Date -= ((uint8_t) 0x10U);
					}
				}
			break;
		case FM_RTC_SET_HOUR_UNIT:
			break;
		case FM_RTC_SET_HOUR_TEN:
			break;
		case FM_RTC_SET_MINUTE_UNIT:
			break;
		case FM_RTC_SET_MINUTE_TEN:
			break;
		case FM_RTC_SET_SECOND_UNIT:
			break;
		case FM_RTC_SET_SECOND_TEN:
			break;
		default:
			FM_DEUBUG_LedError();
			break;
	}


	if (HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BCD) != HAL_OK)
	{
		FM_DEUBUG_LedError();
	}
	if (HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BCD) != HAL_OK)
	{
		FM_DEUBUG_LedError();
	}
}

/*
 *
 */
void FM_RTC_Init()
{

	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;

	time.Hours = ((uint8_t) 0x08U);
	time.Minutes = ((uint8_t) 0x37U);
	time.Seconds = ((uint8_t) 0x00U);
	time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	time.StoreOperation = RTC_STOREOPERATION_RESET;

	if (HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BCD) != HAL_OK)
	{
		FM_DEUBUG_LedError();
	}
	date.WeekDay = RTC_WEEKDAY_WEDNESDAY;
	date.Month = ((uint8_t) 0x08U);
	date.Date = ((uint8_t) 0x29U);
	date.Year = ((uint8_t) 0x07U);

	if (HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BCD) != HAL_OK)
	{
		FM_DEUBUG_LedError();
	}

}

// Interrupts

/*** end of file ***/

