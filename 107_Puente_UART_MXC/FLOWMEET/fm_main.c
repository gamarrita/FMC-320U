/*
 * Desde un terminal se transmite al MXChip, lo que este devuelve va al terminal.
 *
 * Fecha: 25/12/2024 (DD/MM/AAAA)
 * Version: 1
 * Notas: Version inicial.
 *
 *
 */

// Includes.
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
uint8_t  mxc_rx_char;
uint8_t  uart_rx_char;


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
  char msg_power_on[] = "Puente UART MXChip\r";

  HAL_GPIO_WritePin(MXC_ENABLE_GPIO_Port, MXC_ENABLE_Pin, GPIO_PIN_SET);

  HAL_UART_Transmit(&huart1, (uint8_t*) msg_power_on, strlen(msg_power_on), 10);


  HAL_UART_Receive_IT(&huart1, &uart_rx_char, 1);
  HAL_UART_Receive_IT(&huart3, &mxc_rx_char, 1);

  for (;;)
  {
 /*  if(HAL_UART_Receive(&huart1, &uart_rx_char, 1, 1) = HAL_OK)
    {
      HAL_UART_Transmit(&huart1, (uint8_t*) msg_power_on, strlen(msg_power_on), 10);
    }

    if (my_index != mxc_rx_index)
    {
      HAL_UART_Transmit(&huart1, (uint8_t*) (mxc_rx_buffer + my_index), 1, 5);
      if (my_index < BUFFER_RX_UART)
      {
        my_index++;
      }
      else
      {
        my_index = 0;
      }
    }*/
  }
}

// Interrupts

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart == &huart1)
  {
    HAL_UART_Transmit_IT(&huart3, &uart_rx_char, 1);
    HAL_UART_Receive_IT(&huart1, &uart_rx_char, 1);
  }

  if(huart == &huart3)
  {
    HAL_UART_Transmit_IT(&huart1, &mxc_rx_char, 1);
    HAL_UART_Receive_IT(&huart3, &mxc_rx_char, 1);
  }
}

/*** end of file ***/

