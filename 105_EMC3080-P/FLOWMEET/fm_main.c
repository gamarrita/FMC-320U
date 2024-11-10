
/*
 * @brief
 *
 * Fecha: 1/1/1970 (DD/MM/AAAA)  
 * Version: 1
 * Modificaiones.
 */

// Includes.
#include "fm_main.h"
#include "main.h"
#include "fm_emc3080.h"


// Typedef.

// Const data.

// Defines.

// Debug.

// Project variables, non-static, at least used in other file.

// External variables.

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

  FM_EMC3080_Power(0);

  FM_EMC3080_Plus();



	for (;;)
	{

	  HAL_Delay(1000);
	  FM_EMC3080_Standby();
	  HAL_GPIO_TogglePin(LED_2_ACTIVE_GPIO_Port, LED_2_ACTIVE_Pin);
   	}
}

// Interrupts

/*** end of file ***/


