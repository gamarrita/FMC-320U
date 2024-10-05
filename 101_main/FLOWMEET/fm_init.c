/*
 * @brief	Conjunto de funciones del tipo ModuloXInit(), que deber ser llamdas antes de iniciar el RTOS.
 *
 * Versión 1
 * Autor: Daniel H Sagarra
 * Fecha: 08/09/2024
 * Modificaciones: version inicial.
 *
 */

// Includes.
#include "fm_backup.h"
#include "fm_debug.h"
#include "fm_flash.h"
#include "fm_fmc.h"
#include "fm_init.h"
#include "fm_logger.h"
#include "fm_rtc.h"
#include "main.h"


// Typedef.

// Const data.

// Defines.

// Debug.

// Project variables, non-static, at least used in other file.

// External variables.
extern LPTIM_HandleTypeDef hlptim1;

// Global variables, statics.

// Private function prototypes.

// Private function bodies.

// Public function bodies.

/*
 * @brief
 * @param
 * @retval
 *
 */
void FM_INIT_Init()
{
	flash_chip_info_t chip_info;
	uint16_t reset_counter;

	// Por  Por defecto el LPTIM se apaga en stop mode, lo habilito al bajo consumo.
	__HAL_RCC_LPTIM1_CLKAM_ENABLE();
	__HAL_RCC_LPTIM3_CLKAM_ENABLE();
	__HAL_RCC_LPTIM4_CLKAM_ENABLE();

	// El CubeMX configura el LPTIM pero es mi responsabilidad que arranque
	if (HAL_LPTIM_Counter_Start(&hlptim1) != HAL_OK)
	{
		__disable_irq();
		HAL_GPIO_WritePin(LED_1_ERROR_GPIO_Port, LED_1_ERROR_Pin, GPIO_PIN_SET);
		for (;;)
		{
		}
	}

	// Habilito la RAM BACKUP antes de usarla.
	FM_BACKUP_Init();

	reset_counter = FM_FLASH_NewReset(); // Registra un nuevo reset, power o o lo que sea que reinicie el microcontrolador

	chip_info = FM_FLASH_ChipInfoRead();

	if (reset_counter == 1)
	{
		// Solamente programo programo el reloj de tiempo real si es la primera vez que se enciende.
		FM_RTC_Init();

		// Para la primera vez que se enciente se tomaran los datos del sensor pasado como parámetro.
		FM_FMC_Init(FM_FACTORY_SENSOR_0);
	}
	else
	{
		// Aquí hay que implementar un chequeo de que contiene el reloj de tiempo real sea coherente.

		// Aquí hay que implementar un chequeo de que lo que contiene totalizer es coherente.
		FM_FMC_Init(FM_FACTORY_RAM_BACKUP); //FM_FACTORY_RAM_BACKUP
	}

	if (fm_debug_uart_msg)
	{
		FM_DEBUG_UartUint32(chip_info.reset_counter);
	}
}

// Interrupts

/*** end of file ***/
