/* @file fm_debug.h
 *
 * Author: DHS
 * Version: beta 2
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
 * Date: 11/2/2024
 * Name: DHS
 * - Se crea este archivo a partir de uno anterior
 * - Se modifican comentarios
 * Resultado Version beta 1
 *
 * Date: 14/04/2024
 * Name: DHS
 * - Para que sea amigable del llamado desde ThreadX se convierten variables locales a globales.
 * Resultado beta 2
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
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

/*
 * La siguiente variable controla si se envían mensajes de propósitos generales por el UART.
 * Se usa una variable const pero el futuro el hardware tiene que dar soporte para controlar con un jumper.
 */
const uint32_t fm_debug_uart_msg = 0;


/*
 * La siguiente variable controla si se envían mensajes de error por el UART.
 * Se usa una variable const pero el futuro el hardware tiene que dar soporte para controlar con un jumper.
 */
const uint32_t fm_debug_uart_error = 1;

// Typedef.

// Const data.

// Project variables, non-static, at least used in other file.

// Extern variables.
extern UART_HandleTypeDef huart1;

// Global variables, statics.
char str_buffer[SIZE_OF_INT32]; // Como tamaño elijo el mayor de la lista

// Private function prototypes.

// Public function bodies.

/*
 * @brief
 * Esta función envía mensajes de debug, se puede ver al momento de
 * debugger. Se tienen que llevar ajuste en tres etapas diferentes:
 * Al configurar con CubeMX:
 * - Sys->Debug->Trace Asynchronous Sw
 * - En Clock configuration puede elegir cualquier frecuencia para HCLK, pero
 * deberá recordarla.
 *
 * En Debug Configuration:
 * Debugger->Serial Wire Viewer SVW->Enable(marcar).
 * Clock Core (MHz)- El valor que debía recordar, HCLK en Clock Configuration.
 *
 * Iniciada la sesión de debug: en la parte inferior debería aparecer la
 * solapa ITM console, si no aparece se la busca y se la agrega, dos pasos
 * necesarios, habilitar el port_0 y click el boton rojo para iniciar "trace".
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

/*
 * @brief Envía mensajes de debug por uart, un handle al UART es declarado en otro
 * modulo, esto modulo lo acceso como extern. Usar esta única función como comunicación
 * al UART de debug.
 *
 * @param Mensaje a transmitir y su longitud.
 *
 * @retval None
 *
 *
 */
void FM_DEBUG_UartMsg(const char *p_msg, uint8_t len)
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

	ret = HAL_UART_Transmit(&huart1, (const uint8_t*) p_msg, len, WAIT_FOR_UART_5MS);

	if (ret == HAL_BUSY)
	{
		// implementar
	}
}

/*
 * @brief Función que envía un número de 8 bits por el puerto UART.
 * @param Número a imprimir.
 * @retval None
 */
void FM_DEBUG_UartUint8(uint8_t num)
{
	snprintf(str_buffer, sizeof(str_buffer), "%u\n", num);
	FM_DEBUG_UartMsg(str_buffer, strlen(str_buffer));
}

/*
 * @brief Función que envía un número de 16 bits por el puerto UART.
 * @param Número a imprimir.
 * @retval None
 */
void FM_DEBUG_UartUint16(uint16_t num)
{

	snprintf(str_buffer, sizeof(str_buffer), "%u\n", num);
	FM_DEBUG_UartMsg(str_buffer, strlen(str_buffer));
}

/*
 * @brief Función que envía un número de 32 bits por el puerto UART.
 * @param Número a imprimir.
 * @retval None
 */
void FM_DEBUG_UartUint32(uint32_t num)
{

	snprintf(str_buffer, sizeof(str_buffer), "%lu\n", num);
	FM_DEBUG_UartMsg(str_buffer, strlen(str_buffer));
}

void FM_DEBUG_UartInt32(int32_t num)
{
	snprintf(str_buffer, sizeof(str_buffer), "%ld\n", num);
	FM_DEBUG_UartMsg(str_buffer, strlen(str_buffer));
}

/*
 * @brief Función que envía un número flotante por el puerto UART.
 * @param Número a imprimir.
 * @retval None
 */
void FM_DEBUG_UartFloat(float num)
{

	snprintf((char*) str_buffer, SIZE_OF_FLOAT, "%0.2f\n", num);
	FM_DEBUG_UartMsg(str_buffer, SIZE_OF_FLOAT);
}


void FM_DEUBUG_LedError()
{
	// Colocar un brekpoint en la siguiente instrucción.
	HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, GPIO_PIN_SET);
}


// Private function bodies.

/*** end of file ***/
