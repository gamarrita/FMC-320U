
/*
 * @brief
 *
 * Fecha: 1/1/1970 (DD/MM/AAAA)  
 * Version: 1
 * Modificaiones.
 */

// Includes.
#include <fm_mxc.h>
#include "fm_main.h"
#include "main.h"
#include "string.h"



// Typedef.

// Const data.

// Defines.

// Debug.

// Project variables, non-static, at least used in other file.

// External variables.
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;

// Global variables, statics.

// Private function prototypes.

// Private function bodies.

// Public function bodies.

/*
 * @brief   Loop infinito flowmeet.
 * @Note    El programa princiPal se desarrolla en este modulo. No escribir código en archivos
 *          generados automáticamente por los IDEs.
 * @param   Ninguno.
 * @retval  Ninguno.
 *
 */
void FM_MAIN_Main()
{
  char msg_power_on[] = "FM_MAIN_Main\n";

  HAL_UART_Transmit(&huart1, (uint8_t *)msg_power_on, strlen(msg_power_on), 10);

  FM_MXC_InitPtr();

  for(int i = 0; i < 3 ;i++)
  {
    HAL_UART_Transmit_IT(&huart3, (uint8_t *)msg_power_on, strlen(msg_power_on));
    HAL_Delay(1000);
  }

  for(;;)
    {
      HAL_GPIO_TogglePin(LED_ACTIVE_GPIO_Port, LED_ACTIVE_Pin);
      HAL_Delay(1000);
    }
}

// Interrupts


/*** end of file ***/


