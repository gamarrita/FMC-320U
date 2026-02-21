/*
 * @brief
 *
 * Fecha: 1/1/1970 (DD/MM/AAAA)  
 * Version: 1
 * Modificaiones.
 */

// Includes.
#include "main.h"
#include "fm_mxc.h"
#include "fm_main.h"
#include "string.h"
#include "fm_debug.h"

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
  FM_DEBUG_Init();

  for (int n = 3; n; n--)
  {
    FM_MXC_PowerOn();
    FM_MXC_BTConnect();
    FM_MXC_Print("FLOMEET\r\n");
    FM_MXC_PowerOff();
  }


  HAL_SuspendTick();
  HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
  HAL_ResumeTick();
}

// Interrupts

/*** end of file ***/

