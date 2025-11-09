/* @file fm_debug.h
 *
 * Author: DHS
 * Version: 1
 *
 * @brief
 * - Este modulo envía mensajes con propositos de debug, deben ser mensajes cortos.
 * - Se limito el tiempo de transmision a 3ms
 * - Es posible enviar 40 caracteres a 115200 en condiciones favorables
 * - Mensajes mas largos corren riesgos de ser recortados, se aborta luego de 3ms de transmision.
 * - Las funciones que terminan con _itm enviuan mensajes a la consola
 * - Las funciones que terminan con _uart enviuan al uart.
 * - Por defecto esta seleciconado el UART 1, pero se puede cambiar a cualquiero otro.
 * - Este modulo se encarga de separar los mensajes, no es necesario enviar salto de linea para cualqueir comando que se use
 * -
 *
 *
 * - Para poder ver los mensajes se tienen que configurar:
 * En Cube MX:
 * - Sys->Debug->Trace Asynchronous Sw
 * - En Clock configuration puede elegir cualquier frecuencia para HCLK, pero
 * deberá recordarla.
 *
 * En Debug Configuration:
 * Debugger->Serial Wire Viewer SVW->Enable(marcar).
 * Clock Core (Mhz)- El valor que debía recordar, HCLK en Clock Configuration.
 *
 * Iniciada la sesión de debug: en la parte inferior deberia aparecer la
 * solapa ITM console, si no aparece se la busca y se la agrega, dos pasos
 * necesarias, habilitar el port_0 y click el boton rojo para iniciar "trace".
 * 
 *
 * Versión 1
 * Autor: Daniel H Sagarra
 * Fecha: 14/4/2024
 * Modificaciones: version inicial.
 *
 *  Version 2
 * Autor: Daniel H Sagarra
 * Fecha 10/11/2024
 * Modificaciones:
 * - Se agrega función FM_Debug_init().
 *
 *
 */

// Includes.
#include "fm_debug.h"

// Defines.
/*
 * Por defecto usar 1ms como espera en librerias HAL.
 * Como maximo usar 3ms.
 * Estas son transmisiones de debug si toma mas tiempo estamos en problemas.
 * Mensajes de largo de hasta 40 caracteres se deberian transmitir sin problemas
 * para retardos de 5 mili segundos
 *
 */
#define WAIT_FOR_UART_5MS  5
#define WAIT_FOR_UART_1MS  1

#define TRUE    1
#define FALSE   0

/*
 * Las siguientes macros definen: cual es el tamaño maximo de caracteres,
 * incluyendo el terminador nulo, corresponde para los diferentes tipos de
 * datos. Las siguientes macros como las rutinas de conversiones son de uso
 * tan frecuente que deberian estar es una librería especializada.
 *
 */
#define SIZE_OF_UINT8	4 	// "255"
#define SIZE_OF_INT8	5 	// "-128"
#define SIZE_OF_UINT16	6 	// "65534"
#define SIZE_OF_INT16	7 	// "-32768"
#define SIZE_OF_UINT32	11	// "4294967295"
#define SIZE_OF_INT32	12 	// "-2147483648"
#define SIZE_OF_FLOAT	10	// "-123456.7", cuidado con este ejemplo, un flotante como str pude ser mas grande

//Debug.

#define FM_DEBUG_UART_MSG 		// Mensajes de uso general.
#define FM_DEBUG_UART_ERROR		// Mensajes de errpres.

// Typedef.

// Const data.

// Project variables, non-static, at least used in other file.

// Extern variables.
extern UART_HandleTypeDef huart1;

// Global variables, statics.
int debug_uart_enable;
int debug_led_enable;
char str_buffer[SIZE_OF_INT32]; // Como tamaño elijo el mayor de la lista
const char FM_DEBUG_ASCII_CR[] = "\n"; // Valor 10 tabla ASCII

// Private function prototypes.

// Public function bodies.

/*
 * @brief   Lee jumper de configuración y habilita funciones de depuración.
 * @note    PD1 habilita LEDs de error, actividad y señal.
 *          PD2 habilita mensajes por el UART1.
 * @param   Ninguno.
 * @retval  Ninguno.
 */
void FM_DEBUG_Init()
{
  GPIO_InitTypeDef gpio_init =
  {
      0 };
  GPIO_PinState leds_enable_pin;
  GPIO_PinState uart_enable_pin;

  /*Configure GPIO pin : PtPin */
  gpio_init.Pin = DEBUG_LED_Pin;
  gpio_init.Mode = GPIO_MODE_INPUT;
  gpio_init.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(DEBUG_LED_GPIO_Port, &gpio_init);

  gpio_init.Pin = DEBUG_UART_Pin;
  HAL_GPIO_Init(DEBUG_UART_GPIO_Port, &gpio_init);

  HAL_Delay(10);

  leds_enable_pin = HAL_GPIO_ReadPin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin);
  uart_enable_pin = HAL_GPIO_ReadPin(DEBUG_UART_GPIO_Port, DEBUG_UART_Pin);

  if (leds_enable_pin == GPIO_PIN_RESET)
  {
    debug_led_enable = TRUE;
  }
  else
  {
    debug_led_enable = FALSE;
  }

  if (uart_enable_pin == GPIO_PIN_RESET)
  {
    debug_uart_enable = TRUE;
  }
  else
  {
    debug_uart_enable = FALSE;
  }

  gpio_init.Pin = DEBUG_LED_Pin;
  gpio_init.Mode = GPIO_MODE_ANALOG;
  HAL_GPIO_Init(DEBUG_LED_GPIO_Port, &gpio_init);

  gpio_init.Pin = DEBUG_UART_Pin;
  HAL_GPIO_Init(DEBUG_UART_GPIO_Port, &gpio_init);
}

/*
 * @brief   Esta función envía mensajes de debug, se puede ver al momento de
 *          debugger.
 * @note    Se tienen que hacer ajustes en tres etapas diferentes:
 *          Al configurar con CubeMX:
 *          - Sys->Debug->Trace Asynchronous Sw
 *          - En Clock configuration puede elegir cualquier frecuencia para HCLK, pero
 *          deberá recordarla.
 *          En Debug Configuration:
 *          Debugger->Serial Wire Viewer SVW->Enable(marcar).
 *          Clock Core (MHz)- El valor que debía recordar, HCLK en Clock Configuration.
 *          Iniciada la sesión de debug: en la parte inferior debería aparecer la
 *          solapa ITM console, si no aparece se la busca y se la agrega, dos pasos
 *          necesarios, habilitar el port_0 y click el boton rojo para iniciar "trace".
 * @param
 * @retval
 *
 */
void FM_DEBUG_ItmMsg(const char *msg, uint8_t len)
{
  while (*msg && len)
  {
    ITM_SendChar((uint32_t) *msg);
    len--;
    msg++;
  }
}

void FM_DEBUG_LedError(int led_status)
{
  if (debug_led_enable && led_status)
  {
    HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, GPIO_PIN_SET);
  }
  else
  {
    HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, GPIO_PIN_RESET);
  }
}

void FM_DEBUG_LedActive(int led_status)
{
  if (debug_led_enable && led_status)
  {
    HAL_GPIO_WritePin(LED_ACTIVE_GPIO_Port, LED_ACTIVE_Pin, GPIO_PIN_SET);
  }
  else
  {
    HAL_GPIO_WritePin(LED_ACTIVE_GPIO_Port, LED_ACTIVE_Pin, GPIO_PIN_RESET);
  }
}

void FM_DEBUG_LedSginal(int led_status)
{
  if (debug_led_enable && led_status)
  {
    HAL_GPIO_WritePin(LED_SIGNAL_GPIO_Port, LED_SIGNAL_Pin, GPIO_PIN_SET);
  }
  else
  {
    HAL_GPIO_WritePin(LED_SIGNAL_GPIO_Port, LED_SIGNAL_Pin, GPIO_PIN_RESET);
  }
}

/*
 * @brief   Envía mensajes de debug por uart, un handle al UART es declarado en otro modulo, esto modulo lo
 *          acceso como extern. Usar esta única función como comunicación al UART de debug.
 * @param   Mensaje a transmitir y su longitud.
 * @retval  Ninguno.
 */
void FM_DEBUG_UartMsg(const char *p_msg, uint8_t len)
{
  HAL_StatusTypeDef ret;

  if (!debug_uart_enable)
  {
    return;
  }

  /*
   * La función HAL_UART_Transmit retorna HAL_BUSY, si el UART esta en uso,
   * no se logro transmitir.
   */
  if (len < 1)
  {
    return; // el tamaño del string no puede ser menor a 2, un caracter mas el nulo.
  }

  ret = HAL_UART_Transmit(&huart1, (const uint8_t*) p_msg, len, WAIT_FOR_UART_5MS);
  if (ret == HAL_BUSY)
  {
    // implementar
  }
  else
  {
    HAL_UART_Transmit(&huart1, (const uint8_t*) FM_DEBUG_ASCII_CR, 1, WAIT_FOR_UART_5MS);
  }
}

/*
 * @brief   Función que envía un número de 8 bits por el puerto UART.
 * @param   Número a imprimir.
 * @retval  Ninguno.
 */
void FM_DEBUG_UartUint8(uint8_t num)
{

  if (!debug_uart_enable)
  {
    return;
  }

  snprintf(str_buffer, sizeof(str_buffer), "%u\n", num);
  FM_DEBUG_UartMsg(str_buffer, strlen(str_buffer));
}

/*
 * @brief   Función que envía un número de 16 bits por el puerto UART.
 * @param   Número a imprimir.
 * @retval  Ninguno.
 */
void FM_DEBUG_UartUint16(uint16_t num)
{

  if (!debug_uart_enable)
  {
    return;
  }

  snprintf(str_buffer, sizeof(str_buffer), "%u\n", num);
  FM_DEBUG_UartMsg(str_buffer, strlen(str_buffer));
}

/*
 * @brief   Función que envía un número de 32 bits por el puerto UART.
 * @param   Número a imprimir.
 * @retval  Ninguno.
 */
void FM_DEBUG_UartUint32(uint32_t num)
{

  if (!debug_uart_enable)
  {
    return;
  }
  snprintf(str_buffer, sizeof(str_buffer), "%lu\n", num);
  FM_DEBUG_UartMsg(str_buffer, strlen(str_buffer));
}

/*
 * @brief   Envía un entero 32 bits por el puerto UART.
 * @param   Número a imprimir.
 * @retval  Ninguno.
 */
void FM_DEBUG_UartInt32(int32_t num)
{

  if (!debug_uart_enable)
  {
    return;
  }

  snprintf(str_buffer, sizeof(str_buffer), "%ld\n", num);
  FM_DEBUG_UartMsg(str_buffer, strlen(str_buffer));
}

/*
 * @brief   Envía un número flotante por el puerto UART.
 * @param   Número a imprimir.
 * @retval  Ninguno.
 */
void FM_DEBUG_UartFloat(float num)
{

  if (!debug_uart_enable)
  {
    return;
  }

  snprintf((char*) str_buffer, SIZE_OF_FLOAT, "%0.2f\n", num);
  FM_DEBUG_UartMsg(str_buffer, SIZE_OF_FLOAT);
}

// Private function bodies.

/*** end of file ***/
