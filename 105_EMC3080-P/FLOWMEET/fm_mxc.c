/*
 * @brief   Este modulo usa el UART3 para enviar y recibir datos del MAXCHIP, las respuesta las envia
 *          al UART para uso de debug.
 *
 * Fecha: 14/12/2024
 * Autor. DHS
 * Version: 1
 * Resumen: version inicial.
 *
 */

// Includes.
#include "fm_mxc.h"
#include "main.h"
#include "string.h"

// Typedef.

typedef struct
{
  const char *command;
  fm_cmx_at_id_t id;
  const char *response; // Define un tamaño para la respuesta esperada.
} at_command_t;

// Const data.

// Defines.
#define SEND_AT_ATTEMPTS_3  3
#define UART3_BUFFER_LENGTH 80    // Tamaño máximo de una respuesta valida del MXC.
#define WAIT_FOR_5MS  5
#define WAIT_FOR_1MS  1

// Debug.

// Project variables, non-static, at least used in other file.
char mxc_rx_buffer[UART3_BUFFER_LENGTH] =
{
    0 };  // buffer que guarda la respuesta que envio el mxc.
uint8_t mxc_rx_char; // caracter a caracter recibido, de los enviados por mxc
uint32_t mxc_rx_index;

// External variables.
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart1;

const at_command_t at_list[] =
{
    {
        "AT+UARTE?\r",
        FM_MXC_AT_UARTE_ID,
        "OK" },
    {
        "AT+UARTE=ON\r",
        FM_MXC_AT_UARTE_ON_ID,
        "OK" },
    {
        "AT+UARTE=OFF\r",
        FM_MXC_AT_UARTE_OFF_ID,
        "OK" },
    {
        "AT+STANDBY\r",
        FM_MXC_AT_STANDBY_ID,
        "OK"
    },
};

// Global variables, statics.

// Private function prototypes.

// Private function bodies.

// Public function bodies.

/*
 * @brief   Manejo el pin del MXChip enable, al momento esta función sirve para hacer reset al
 *          MXChip. Queda pendiente ver que consumo tiene el MXChip es diable. El uso principal es
 *          hacer reset para quitar al MXChip de standby, si en el futuro se apaga el MXChip por hardware
 *          esta función no tendrá mas uso que hacer reset del MXChip.
 * @param   mode, habilitas, des-habilita o hace reset del MXChip.
 * @retval  Ninguno.
 */
void FM_MXC_Enable(int mode)
{
  switch (mode)
  {
  case FM_MXC_DISABLE:
    HAL_GPIO_WritePin(MXC_ENABLE_GPIO_Port, MXC_ENABLE_Pin, GPIO_PIN_RESET);
    break;
  case FM_MXC_ENABLE:
    HAL_GPIO_WritePin(MXC_ENABLE_GPIO_Port, MXC_ENABLE_Pin, GPIO_PIN_SET);
    break;
  case FM_MXC_RESET:
    HAL_GPIO_WritePin(MXC_ENABLE_GPIO_Port, MXC_ENABLE_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(MXC_ENABLE_GPIO_Port, MXC_ENABLE_Pin, GPIO_PIN_SET);
    break;
  default:
    HAL_GPIO_WritePin(MXC_ENABLE_GPIO_Port, MXC_ENABLE_Pin, GPIO_PIN_RESET);
  }
}

/*
 * @brief   Inicializa el modulo MXChip para usarlo como impresora. Usar este comando antes enviar algo a
 *          imprimir.
 * @param   Ninguno.
 * @retval  Ninguno.
 */
void FM_MXC_InitPtr()
{
  fm_mxc_status_t retval;

  FM_MXC_Enable(FM_MXC_RESET);
  FM_MXC_Plus();

  // Enciendo la recepción de caracteres
  HAL_UART_Receive_IT(&huart3, &mxc_rx_char, 1);
  retval = FM_MXC_SendAT(FM_MXC_AT_UARTE_OFF_ID, SEND_AT_ATTEMPTS_3);

  // Si no se puede inicializar encender led de error.
  if(retval == FM_MXC_FAIL)
  {
    HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, GPIO_PIN_SET);
  }
  else
  {


  }

}


/*
 * @brief   Se necesita una linea adicional del micro-contraolador para poder apagar por hardware el MXChip.
 * @param
 * @retval
 */
void FM_MXC_PowerOn(int mode)
{

}

/*
 * @brief   Cambia entre los modos AT y puente.
 * @param
 * @retval
 */
void FM_MXC_Plus()
{
  const char msg[] = "+++";
  HAL_UART_Transmit_IT(&huart3, (const uint8_t*) msg, strlen(msg));

  /*
   * Cuidado, si no hay un HAL_Delay aqui solo se transmiten 2 caracteres, esto aunque coloque el delay justo
   * despues de la llamada a esta function, punto de retorno, no se porque.
   */
  HAL_Delay(100);
}

/*
 * @brief   Se envía un comando AT al MXChip.
 * @param   id, identificador del comando a enviar.
 *          retry, cantidad de intentos hasta recibir una respuesta de "OK"
 * @retval  MXC_OK si el MXC responde con OK dentro de los intentos.
 *          MXC_FAIL si no se recibe MCX_OK al agotar los intentos.
 */
fm_mxc_status_t FM_MXC_SendAT(fm_cmx_at_id_t id, int retry)
{
  fm_mxc_status_t retval = FM_MXC_FAIL;

  while (retry >= 0)
  {
    // Borro contenido del buffer
    mxc_rx_index = 0;
    mxc_rx_buffer[0] = 0;

    // Transmito comando AT
    HAL_UART_Transmit_IT(&huart3, (uint8_t*) at_list[id].command, strlen(at_list[id].command));

    // Espero respuesta del MXChip. El tiempo espera me parece excesivamente largo. En pruebas anteriores
    // funcionaba con 10ms de retardo, ahora no puedo bajar de xxx mili-segundos.
    HAL_Delay(100);

    // Replico la respuesta en el puerto uart donde monitoreo el debugger.
    HAL_UART_Transmit(&huart1, (uint8_t*) mxc_rx_buffer, strlen(mxc_rx_buffer), 10);

    // Verifico si el el comando AT respondió OK, si no respondió se volvería a intentar.
    retval = FMC_MXC_ReciveStatus();
    if (retval == FM_MXC_OK)
    {
      return FM_MXC_OK;
    }
    else
    {
      retry--;
      HAL_Delay(1000);
    }
  }
  return retval;
}

// Interrupts

/*
 * @brief   Recibo los caracteres de respuesta del MXChip de a 1. Los voy escribiendo en un buffer hasta
 *          que llega el final del la respuesta, en este punto retransmito la respuesta completa por el
 *          UART usado para debugger.
 * @param
 * @retval
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  mxc_rx_buffer[mxc_rx_index] = mxc_rx_char;
  HAL_UART_Receive_IT(&huart3, &mxc_rx_char, 1);
  mxc_rx_index++;
  mxc_rx_buffer[mxc_rx_index] = 0;
}

/*
 * @brief   Se envia un comando AT al MXChip.
 * @param
 * @retval
 */
fm_mxc_status_t FMC_MXC_ReciveStatus()
{
  fm_mxc_status_t retval = FM_MXC_FAIL;
  const uint32_t msg_ok = 0x0a0d4b4f; // equivalente a la terminación "OK\r\n"
  uint32_t *index;

  if(mxc_rx_index >= sizeof(uint32_t))
  {
    index = (uint32_t *)(mxc_rx_buffer + mxc_rx_index);
    index--;

    if(*index ==  msg_ok)
    {
      retval = FM_MXC_OK;
    }
  }
  return retval;
}

/*** end of file ***/

