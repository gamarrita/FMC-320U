
/*
 * @brief
 *
 * Fecha: 1/1/1970 (DD/MM/AAAA)  
 * Version: 1
 * Modificaiones.
 */

// Includes.
#include "fm_emc3080.h"
#include "main.h"


// Typedef.

// Const data.

// Defines.

#define WAIT_FOR_5MS  5
#define WAIT_FOR_1MS  1

// Debug.

// Project variables, non-static, at least used in other file.

// External variables.

extern UART_HandleTypeDef huart3;

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
void FM_EMC3080_Power(int mode)
{
  if(mode)
  {
    HAL_GPIO_WritePin(MXC_ENABLE_GPIO_Port, MXC_ENABLE_Pin, GPIO_PIN_RESET);
  }
  else
  {
    HAL_GPIO_WritePin(MXC_ENABLE_GPIO_Port, MXC_ENABLE_Pin, GPIO_PIN_SET);
  }
}

void FM_EMC3080_Msg(const char *p_msg, uint8_t len)
{
  HAL_StatusTypeDef ret = HAL_BUSY;

  /*
   * La función HAL_UART_Transmit retorna HAL_BUSY, si el UART esta en uso,
   * no se logro transmitir.
   *
   */

  if (len < 1)
  {
    return; // el tamaño del string no puede ser menor a 2, un caracter mas el nulo.
  }

  ret = HAL_UART_Transmit(&huart3, (const uint8_t*) p_msg, len, WAIT_FOR_5MS);

  if (ret == HAL_BUSY)
  {
    // implementar
  }
}

void FM_EMC3080_Plus()
{
  const char msg[] = "+++";
  HAL_UART_Transmit(&huart3, (const uint8_t*) msg, 3, WAIT_FOR_5MS);
  HAL_Delay(1000);
}

void FM_EMC3080_Standby()
{
  const char msg[] = "AT+STANDBY\n";
  HAL_UART_Transmit(&huart3, (const uint8_t*) msg, 3, WAIT_FOR_5MS);
  HAL_Delay(100);
}




// Interrupts

/*** end of file ***/


