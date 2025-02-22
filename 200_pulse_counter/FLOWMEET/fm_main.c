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
#include "fm_lcd.h"
#include "fm_debug.h"

// Typedef.
typedef enum
{
  MODE_RUN, MODE_SLEEP, MODE_STOP_1, MODE_STOP_2, MODE_STOP_3, MODE_STANDBY_RTC, MODE_STANDBY, MODE_SHUTDOWN
} power_mode_t;

// Const data.

// Defines.

// Debug.

// Project variables, non-static, at least used in other file.

// External variables.
extern LPTIM_HandleTypeDef hlptim4;

// Global variables, statics.
uint16_t lptim4_cnt_new;
uint16_t lptim4_cnt_old;
uint16_t lptim4_cnt_delta;
uint32_t counter;

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

  int refresh_lcd();

  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
  HAL_Delay(10);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);

  HAL_GPIO_WriteMultipleStatePin(LED_BACKLIGHT_GPIO_Port, LED_BACKLIGHT_Pin, GPIO_PIN_RESET);
  FM_LCD_Init(0xff);
  HAL_Delay(250);

  FM_LCD_LL_Clear();
  HAL_GPIO_WriteMultipleStatePin(LED_BACKLIGHT_GPIO_Port, LED_BACKLIGHT_Pin, GPIO_PIN_SET);

  if (HAL_LPTIM_Counter_Start(&hlptim4) != HAL_OK)
  {
    FM_DEBUG_LedError(1);
  }

  lptim4_cnt_new = LPTIM4->CNT;
  counter = 0;

  for (;;)
  {
    lptim4_cnt_old = lptim4_cnt_new;
    lptim4_cnt_new = LPTIM4->CNT;;
    lptim4_cnt_delta = lptim4_cnt_new - lptim4_cnt_old;
    counter += lptim4_cnt_delta;

    FM_LCD_PutUnsignedInt32(counter, FM_LCD_LL_ROW_1);
    FM_LCD_LL_Refresh();
    HAL_GPIO_WritePin(LED_ACTIVE_GPIO_Port, LED_ACTIVE_Pin, GPIO_PIN_SET);

    HAL_GPIO_WriteMultipleStatePin(LED_ACTIVE_GPIO_Port, LED_ACTIVE_Pin, GPIO_PIN_RESET);
    HAL_SuspendTick();
    HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
    HAL_ResumeTick();
  }
}

// Interrupts

/*** end of file ***/

