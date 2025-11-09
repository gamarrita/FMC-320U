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
#define UART3_RX_BUFFER_SIZE 100

// Sección enum sin dependencia.

// Sección typedef sin dependencia.

// Sección define, enum y typedef con dependencia.

// Const data.

// Debug.

// Variables non-static

// Variables statics.
char uart3_rx_buffer[UART3_RX_BUFFER_SIZE] =
{ 65 };

uint16_t count = 0;

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
    char msg_1[] = "AT+FWVER?\n\r";

    HAL_GPIO_WritePin(MXC_ENABLE_GPIO_Port, MXC_ENABLE_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(MXC_ON_GPIO_Port, MXC_ON_Pin, GPIO_PIN_RESET);

    HAL_Delay(1000);

    HAL_UARTEx_ReceiveToIdle_DMA(&huart3, (uint8_t *)uart3_rx_buffer, UART3_RX_BUFFER_SIZE);

    for (;;)
    {
        HAL_GPIO_WritePin(LED_ACTIVE_GPIO_Port, LED_ACTIVE_Pin, GPIO_PIN_SET);
        HAL_Delay(5); // simulo tiempo activo
        HAL_GPIO_WritePin(LED_ACTIVE_GPIO_Port, LED_ACTIVE_Pin, GPIO_PIN_RESET);

        count = UART3_RX_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(huart3.hdmarx);

        if (count == UART3_RX_BUFFER_SIZE)
        {
            HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, GPIO_PIN_SET);  // DMA no recibe
            HAL_UART_Receive_DMA(&huart3, (uint8_t *) uart3_rx_buffer, UART3_RX_BUFFER_SIZE);
        }

        HAL_Delay(1000);
        HAL_UART_Transmit_DMA(&huart3, (uint8_t *)msg_1, sizeof(msg_1) -1);
    }
}




// Interrupts

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART3)
    {
        // Size bytes recibidos hasta IDLE o buffer completo
        // ProcessUARTData(uart3_rx_buffer, Size);
        // Reiniciar la recepción
        HAL_UARTEx_ReceiveToIdle_DMA(huart, (uint8_t *)uart3_rx_buffer, UART3_RX_BUFFER_SIZE);
    }
}
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART3)
    {
        HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, GPIO_PIN_SET);
        HAL_UART_AbortReceive(huart);
        HAL_UARTEx_ReceiveToIdle_DMA(huart, (uint8_t *)uart3_rx_buffer, UART3_RX_BUFFER_SIZE);
    }
}

/*** end of file ***/

