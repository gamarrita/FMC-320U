
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
	HAL_Delay(100);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);

	for (;;)
	{
		HAL_SuspendTick();
		HAL_PWREx_EnterSTOP3Mode(PWR_STOPENTRY_WFI);
		HAL_Delay(100);
		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	}
}

// Interrupts

/*** end of file ***/


