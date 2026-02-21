
/*
 * Descripcion
 *
 * Autor:
 * Fecha: 
 */

// Includes.
#include "fm_main.h"
#include "main.h"
#include "usart.h"


// Sección define sin dependencia.
#define USART3_RX_BUFFER_SIZE 128

// Sección enum sin dependencia.

// Sección typedef sin dependencia.

// Sección define, enum y typedef con dependencia.

// Const data.

// Debug.

// Variables non-static

// Variables statics.
char     usart3_rx_buffer[USART3_RX_BUFFER_SIZE] = {65,65,65,65,65};
uint16_t usart3_rx_buffer_idx;
char     usart3_rx_isr_char;

// Variables extern, las que no estan en .h.

// Prototipos funciones privadas.

// Cuerpo funciones privadas.

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

//    HAL_UARTEx_EnableStopMode(&huart3);               // Habilita modo STOP wake-up
    HAL_GPIO_WritePin(MXC_ENABLE_GPIO_Port, MXC_ENABLE_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(MXC_ON_GPIO_Port, MXC_ON_Pin, GPIO_PIN_RESET);

    HAL_UART_Receive(&huart3, (uint8_t *)usart3_rx_buffer, USART3_RX_BUFFER_SIZE, HAL_MAX_DELAY);


	for (;;)
	{
	    HAL_GPIO_WritePin(LED_ACTIVE_GPIO_Port, LED_ACTIVE_Pin, GPIO_PIN_SET);
	    HAL_Delay(5); // simulo tiempo activo
	    HAL_GPIO_WritePin(LED_ACTIVE_GPIO_Port, LED_ACTIVE_Pin, GPIO_PIN_RESET);

	    HAL_Delay(1000);
	}
}


// Interrupts
/*
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

    HAL_GPIO_TogglePin(LED_SIGNAL_GPIO_Port, LED_SIGNAL_Pin);

    if (huart->Instance == USART3)
    {
        // Volver a armar recepción
        if (HAL_UART_Receive_IT(&huart3, (uint8_t *)&usart3_rx_isr_char, 5) != HAL_OK)
        {
            HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, GPIO_PIN_SET);
            return;
        }
        else
        {
            usart3_rx_buffer[usart3_rx_buffer_idx] = usart3_rx_isr_char;
            usart3_rx_buffer_idx++;
        }
    }
    if(usart3_rx_buffer_idx == USART3_RX_BUFFER_SIZE)
    {
        usart3_rx_buffer_idx = 0;
    }
}
*/

/**
 * @brief  Wake Up Timer callback.
 * @param  hrtc RTC handle
 * @retval None
 */
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hrtc);
}

/*** end of file ***/




/*** end of file ***/


