/*
 * @brief Programa para comprobar l consumo del computador de caudal.
 *
 *
 * Autor: DHS
 * Fecha: 4/10/2024
 * Version: 1
 * Modificaiones: Version inicial.
 * Consumo promedio en ventana de 1 minuto 14.00uA
 * Consumo maximo 14.07 uA
 * Consumo minimo 13.95 uA
 *
 *
 */

// Includes.
#include "fm_main.h"
#include "main.h"

// Typedef.
typedef enum
{
  MODE_RUN,
  MODE_SLEEP,
  MODE_STOP_1,
  MODE_STOP_2,
  MODE_STOP_3,
  MODE_STANDBY_RTC,
  MODE_STANDBY,
  MODE_SHUTDOWN
} power_mode_t;

// Const data.

// Defines.

// Debug.

// Project variables, non-static, at least used in other file.

// External variables.

// Global variables, statics.
int wakeup_flag;
power_mode_t power_mode;

// Private function prototypes.

// Private function bodies.

// Public function bodies.

/*
 * @brief   Loop infinito flowmeet.
 * @Note    El programa princial se derarrolla en este modulo. No escribir codigo en archivos
 *          generados automaticamente por los IDEs.
 * @param   Ninguno.
 * @retval  Ninguno.
 *
 */
void FM_MAIN_Main()
{
  // Se resetea el PCF8553
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
  HAL_Delay(10);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);

  power_mode_t power_mode = MODE_STANDBY_RTC;

  for (;;)
  {

    wakeup_flag = 1;

    // Shutdown mode reinicia el equipo, entonces el parpadeo antes de entrar al modo de cambiar el modo.
    HAL_GPIO_WritePin(LED_ACTIVE_GPIO_Port, LED_ACTIVE_Pin, GPIO_PIN_SET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(LED_ACTIVE_GPIO_Port, LED_ACTIVE_Pin, GPIO_PIN_RESET);

    switch (power_mode)
    {
    case MODE_RUN:
      while (wakeup_flag);
      break;
    case MODE_SLEEP:
      break;
    case MODE_STOP_1:
      HAL_SuspendTick();
      HAL_PWREx_EnterSTOP1Mode(PWR_STOPENTRY_WFI);
      HAL_ResumeTick();
      break;
    case MODE_STOP_2:
      /*
       * Consumo 17.38uA
       */
      HAL_SuspendTick();
      HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
      HAL_ResumeTick();
      break;
    case MODE_STOP_3:
      HAL_PWREx_EnterSTOP3Mode(PWR_STOPENTRY_WFI);
      break;
    case MODE_STANDBY_RTC:
      /*
       * Consumo 11.04uA
       * No se se esta despertando
       */
      HAL_PWR_EnterSTANDBYMode();
      break;
    case MODE_STANDBY:
      break;
    case MODE_SHUTDOWN:
      HAL_PWREx_EnterSHUTDOWNMode();
      break;
    default:
      break;
    }

  }
}

// Interrupts

/**
 * @brief  Wake Up Timer callback.
 * @param  hrtc RTC handle
 * @retval None
 */
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hrtc);
  wakeup_flag = 0;

}

/*** end of file ***/

