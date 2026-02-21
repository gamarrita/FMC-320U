/*
 * @brief   Proyecto para configurar el comportamiento del computador según si se han colocado o no jumper
 *          de PD1 a PD4
 * @note    Al momento se implemento configuración de leds y UART, para que los leds EROOR, ACTIVE y
 *          SIGNAL se encienda se debe colocar PD1, para enviar mensajes de UART se debe colocar PD2
 *
 * Autor: DHS
 * Fecha: 9/11/2024
 * Version: 1
 * Modificaciones: Version inicial.
 *
 *
 */

// Includes.
#include "fm_debug.h"
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

  HAL_Delay(100);

  // Reset del PCF8553, apaga la pantalla si quedo encendida.
   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
   HAL_Delay(10);
   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);


  for(;;)
  {
    FM_DEBUG_Init();
    HAL_Delay(1);

    FM_DEBUG_LedActive(0);
    HAL_SuspendTick();
    HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
    HAL_ResumeTick();
    FM_DEBUG_LedActive(1);

    FM_DEBUG_UartInt32(12345);
  }
}

// Interrupts

/*** end of file ***/

