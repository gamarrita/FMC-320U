/*
 * Author: DHS
 *
 * @brief plantilla código.
 *
 * Versión 1
 * Autor: Daniel H Sagarra
 * Fecha: 10/11/2024
 * Modificaciones: version inicial
 * 
 */

// Includes.
#include "fm_debug.h"
#include "fm_rtc.h"
#include <time.h>

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
 * 	@brief	devuelve fecha y ahora en dos strings. Formato para LCD ddmmaa y hhmmss
 * 	@param	time_str, puntero a string, aquí se escribe la hora.
 * 	@param 	date_str, puntero a string, aquí se escribe la fecha.
 */
void FM_RTC_Gets(char *time_str, char *date_str)
{
  RTC_TimeTypeDef time = {0};
  RTC_DateTypeDef date = {0};

  HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
  HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);

  sprintf(time_str, "%02d.%02d.%02d ", time.Hours, time.Minutes, time.Seconds);
  sprintf(date_str, "%02d.%02d.20%02d ", date.Date, date.Month, date.Year);
}


/*
 * @brief   Retorna la cantidad de segundos transcurridos desde el 1 de enero de 1970
 * @note
 * @param   ninguno
 * @retval  ninguno
 */
uint32_t FM_RTC_GetUnixTime(void)
{
    RTC_TimeTypeDef time = {0};
    RTC_DateTypeDef date = {0};
    struct tm t;

    // Leer hora y fecha del RTC
    HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
    HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);

    // Llenar estructura tm (asumiendo año base 2000)
    t.tm_year = 2000 + date.Year - 1900; // tm_year: años desde 1900
    t.tm_mon  = date.Month - 1;          // tm_mon: 0 = enero
    t.tm_mday = date.Date;
    t.tm_hour = time.Hours;
    t.tm_min  = time.Minutes;
    t.tm_sec  = time.Seconds;
    t.tm_isdst = 0; // sin horario de verano

    // Convertir a timestamp UNIX (segundos desde 1/1/1970)
    time_t timestamp = mktime(&t);

    return (uint32_t)timestamp;
}

/*
 *  @brief  devuelve fecha y ahora en dos strings. Formato para impresora
 *  @param  time_str, puntero a string, aquí se escribe la hora.
 *  @param  date_str, puntero a string, aquí se escribe la fecha.
 */
void FM_RTC_GetPpt(char *time_str, char *date_str)
{
  RTC_TimeTypeDef time = {0};
  RTC_DateTypeDef date = {0};

  HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
  HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);

  sprintf(time_str, "%02d:%02d:%02d ", time.Hours, time.Minutes, time.Seconds);
  sprintf(date_str, "%02d/%02d/20%02d ", date.Date, date.Month, date.Year);
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

  //
  RTC_TimeTypeDef time = {0};
  RTC_DateTypeDef date = {0};

  HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

  switch (sel)
  {
  case FM_RTC_SET_YEAR:
    if (mode)
    {
      if (date.Year < 99)
      {
        date.Year++;
      }
    }
    else
    {
      if (date.Year > 1)
      {
        date.Year--;
      }
    }
    break;
  case FM_RTC_SET_MONTH:
    if (mode)
    {
      if (date.Month < 12)
      {
        date.Month++;
      }
    }
    else
    {
      if (date.Month > 1)
      {
        date.Month--;
      }
    }
    break;
  case FM_RTC_SET_DAY:
    if (mode)
    {
      if (date.Date < 31)
      {
        date.Date++;
      }
    }
    else
    {
      if (date.Date > 1)
      {
        date.Date--;
      }
    }
    break;
  case FM_RTC_SET_HOUR:
    if (mode)
    {
      if (time.Hours < 23)
      {
        time.Hours++;
      }
    }
    else
    {
      if (time.Hours > 0)
      {
        time.Hours--;
      }
    }
    break;
  case FM_RTC_SET_MINUTE:
    if (mode)
    {
      if (time.Minutes < 59)
      {
        time.Minutes++;
      }
    }
    else
    {
      if (time.Minutes > 0)
      {
        time.Minutes--;
      }
    }
    break;
  case FM_RTC_SET_SECOND:
    if (mode)
    {
      if (time.Seconds < 59)
      {
        time.Seconds++;
      }
    }
    else
    {
      if (time.Seconds > 0)
      {
        time.Seconds--;
      }
    }
    break;
  default:
    FM_DEBUG_LedError(1);
    break;
  }

  if (HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN) != HAL_OK)
  {
    FM_DEBUG_LedError(1);
  }

  if (HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN) != HAL_OK)
  {
    FM_DEBUG_LedError(1);
  }

}

/*
 *
 */
void FM_RTC_Init()
{

  RTC_TimeTypeDef time = {0};
  RTC_DateTypeDef date = {0};

  time.Hours = ((uint8_t) 0x08U);
  time.Minutes = ((uint8_t) 0x37U);
  time.Seconds = ((uint8_t) 0x00U);
  time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  time.StoreOperation = RTC_STOREOPERATION_RESET;

  if (HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BCD) != HAL_OK)
  {
    FM_DEBUG_LedError(1);
  }
  date.WeekDay = RTC_WEEKDAY_WEDNESDAY;
  date.Month = ((uint8_t) 0x08U);
  date.Date = ((uint8_t) 0x29U);
  date.Year = ((uint8_t) 0x07U);

  if (HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BCD) != HAL_OK)
  {
    FM_DEBUG_LedError(1);
  }

}

// Interrupts

/*** end of file ***/

