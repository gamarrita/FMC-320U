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

// Lista de comandos AT
typedef enum
{
  AT_UARTE,         // Consulta si UART tiene eco.
  AT_UARTE_ON,      // Activa el eco, el eco actúa sobre cada caracter recibido, no espera el \r
  AT_UARTE_OFF,     // Desactiva el eco.
  AT_STANDBY,       // Pasa a modo de bajo consumo. Para salir de este modo se hace hard reset
  AT_BROLE_MASTER,  // Configura al modulo como bluetooth maestro.
  AT_BINQ_NAME,     // Escanea buscando un nombre especifico de impresora.
  AT_BCONN_0,       // Se conecta al primer resultado del escaneado.
  AT_BLE,           // Consulta el estado del bluetooth,
  AT_BLE_ON,        // Activa el modulo bluetooth.
  AT_BLE_OFF,       // Desactiva el modulo bluetooth.
  AT_EVENT_ON,      // Los eventos notifican por el puerto UART. El mas relevante es el de AT+BINQ=1,....
  AT_EVENT_OFF,     // No se notifican los evento.
  AT_BSENDRAW,     // Pasa la impresora a modo transparente.
  AT_REBOOT,
  AT_BSINQ,
  AT_BSERVUUID_1800,
  AT_BTXUUID_2A00,
  AT_BRXUUID_2A00,
  AT_FWVER,
} at_id_t;

//
typedef struct
{
  const char *command;
  at_id_t id;
  const char *response; // Define un tamaño para la respuesta esperada.
} at_command_t;

// Const data.

// Defines.
#define SEND_ATTEMPTS_1     1  // Un solo intento al enviar comando AT.
#define SEND_ATTEMPTS_3     3  // Se envía hasta 3 veces el comando AT para recibir respuesta correcta.
#define SEND_ATTEMPTS_10    10 // Se envía hasta 10 veces el comando AT para recibir respuesta correcta.
#define BUFFER_RX_UART      80 // Tamaño del buffer de recepcio de datos, maximo.
#define WAIT_FOR_5MS  5
#define WAIT_FOR_1MS  1

// Debug.

// Project variables, non-static, at least used in other file.
uint8_t mxc_rx_buffer[BUFFER_RX_UART] =
{
    0 };  // buffer que guarda la respuesta que envio el mxc.
uint8_t mxc_rx_char = 0; // caracter a caracter recibido, de los enviados por mxc
uint32_t mxc_rx_index = 0;

// External variables.
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart1;

/*
 * Lista de comandos AT del EMC-3080, esta lista solo implementa los que como respuesta solo se espera OK.
 */
const at_command_t at_list[] =
{
    {
        "AT+UARTE?\r",
        AT_UARTE,
        "OK" },
    {
        "AT+UARTE=ON\r",
        AT_UARTE_ON,
        "OK" },
    {
        "AT+UARTE=OFF\r",
        AT_UARTE_OFF,
        "OK" },
    {
        "AT+STANDBY\r",
        AT_STANDBY,
        "OK" },
    {
        "AT+BROLE=1\r",
        AT_BROLE_MASTER,
        "OK" },
    {
        "AT+BINQ=1,FM-BLE\r", // FM IMPRESORA debe ser el nombre de la impresora.
        AT_BINQ_NAME,
        "OK", },
    {
        "AT+BCONN=0\r",
        AT_BCONN_0,
        "OK", },
    {
        "AT+BLE?\r",
        AT_BLE,
        "OK", },
    {
        "AT+BLE=ON\r",
        AT_BLE_ON,
        "OK", },
    {
        "AT+BLE=OFF\r",
        AT_BLE_OFF,
        "OK", },

    {
        "AT+BEVENT=ON\r",
        AT_EVENT_ON,
        "OK", },
    {
        "AT+BEVENT=OFF\r",
        AT_EVENT_OFF,
        "OK", },
    {
        "AT+BSENDRAW\r",
        AT_BSENDRAW,
        "OK", },
    {
        "AT+REBOOT\r",
        AT_REBOOT,
        "OK", },
    {
        "AT+BSINQ\r",
        AT_BSINQ,
        "OK", },
    {
        "AT+BSERVUUID=1800\r",
        AT_BSERVUUID_1800,
        "OK", },
    {
        "AT+BTXUUID=2A00\r",
        AT_BTXUUID_2A00,
        "OK", },
    {
        "AT+BRXUUID=2A00\r",
        AT_BRXUUID_2A00,
        "OK", },

    {
        "AT+FWVER?\r",
        AT_FWVER,
        "OK", },

};

// Global variables, statics.

// Private function prototypes.
fm_mxc_status_t MXC_ReciveStatus();
fm_mxc_status_t MXC_SendAT(at_id_t id, int retry, uint32_t time_delay);
void MXC_Delay(uint32_t time_delay);
void MXC_Plus();
void MXC_Bsendraw(char *str);

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

  // Enciendo la recepción de caracteres
  HAL_UART_Receive_IT(&huart3, &mxc_rx_char, 1);

  MXC_Plus();

  retval = MXC_SendAT(AT_UARTE_ON, 1, 500);
  if (retval == FM_MXC_FAIL)
  {
    HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, GPIO_PIN_SET);
  }

  retval = MXC_SendAT(AT_FWVER, 1, 500);
  if (retval == FM_MXC_FAIL)
  {
    HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, GPIO_PIN_SET);
  }

  retval = MXC_SendAT(AT_BSERVUUID_1800, 1, 500);
  if (retval == FM_MXC_FAIL)
  {
    HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, GPIO_PIN_SET);
  }


  retval = MXC_SendAT(AT_BTXUUID_2A00, 1, 500);
  if (retval == FM_MXC_FAIL)
  {
    HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, GPIO_PIN_SET);
  }

  retval = MXC_SendAT(AT_BRXUUID_2A00, 1, 500);
  if (retval == FM_MXC_FAIL)
  {
    HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, GPIO_PIN_SET);
  }


  retval = MXC_SendAT(AT_REBOOT, 1, 500);
  if (retval == FM_MXC_FAIL)
  {
    HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, GPIO_PIN_SET);
  }

  retval = MXC_SendAT(AT_EVENT_ON, 1, 500);
  if (retval == FM_MXC_FAIL)
  {
    HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, GPIO_PIN_SET);
  }

  retval = MXC_SendAT(AT_BROLE_MASTER, 1, 500);
  if (retval == FM_MXC_FAIL)
  {
    HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, GPIO_PIN_SET);
  }

  retval = MXC_SendAT(AT_BLE_ON, 1, 500);
  if (retval == FM_MXC_FAIL)
  {
    HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, GPIO_PIN_SET);
  }

  retval = MXC_SendAT(AT_EVENT_ON, 1, 500);
  if (retval == FM_MXC_FAIL)
  {
    HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, GPIO_PIN_SET);
  }

  retval = MXC_SendAT(AT_BSINQ, 1, 500);
  if (retval == FM_MXC_FAIL)
  {
    HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, GPIO_PIN_SET);
  }

  retval = MXC_SendAT(AT_BINQ_NAME, 1, 500);
  if (retval == FM_MXC_FAIL)
  {
    HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, GPIO_PIN_SET);
  }

  HAL_Delay(1000);

  retval = MXC_SendAT(AT_BCONN_0, 1, 500);
  if (retval == FM_MXC_FAIL)
  {
    HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, GPIO_PIN_SET);
  }

  MXC_Bsendraw("FLOWMEET SRL\n");

  /*

   retval = FM_MXC_SendAT(FM_MXC_AT_BTSENDRAW, SEND_AT_ATTEMPTS_3);
   if(retval == FM_MXC_FAIL)
   {
   HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, GPIO_PIN_SET);
   }


   retval = FM_MXC_SendAT(FM_MXC_AT_BSEND_FLOWMEET, 1);
   if(retval == FM_MXC_FAIL)
   {
   HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, GPIO_PIN_SET);
   }
   */
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
 * @param   Ninguno.
 * @retval  Ninguno.
 */
void MXC_Plus()
{
  const char msg[] = "+++";

  HAL_Delay(100);

  HAL_UART_Transmit_IT(&huart3, (const uint8_t*) msg, strlen(msg));
  HAL_Delay(100);

  /*
   * Cuidado, si no hay un HAL_Delay aqui solo se transmiten 2 caracteres, esto aunque coloque el delay justo
   * despues de la llamada a esta function, punto de retorno, no se porque.
   */
  MXC_Delay(1000);
}

/*
 * @brief   Se envía un comando AT al MXChip.
 * @param   id, identificador del comando a enviar.
 *          retry, cantidad de intentos hasta recibir una respuesta de "OK"
 * @retval  MXC_OK si el MXC responde con OK dentro de los intentos.
 *          MXC_FAIL si no se recibe MCX_OK al agotar los intentos.
 */
fm_mxc_status_t MXC_SendAT(at_id_t id, int retry, uint32_t time_delay)
{
  fm_mxc_status_t retval = FM_MXC_FAIL;

  HAL_UART_Transmit_IT(&huart3, (uint8_t*) at_list[id].command, strlen(at_list[id].command));

  MXC_Delay(time_delay);

  return retval;
}

/*
 * @brief
 * @param
 *
 * @retval
 *
 */
void MXC_Bsendraw(char *str)
{
  char str1[] = "AT+BSEND=1,0,18\r";
  char str2[] = "FLOWMEET\n";

  HAL_UART_Transmit_IT(&huart3, (uint8_t*) str1, strlen(str1));
  HAL_Delay(100);
  HAL_UART_Transmit_IT(&huart3, (uint8_t*) str2, strlen(str2));
  HAL_Delay(1000);

  /*
   MXC_SendAT(AT_BSENDRAW, 1, 500);

   HAL_Delay(1000);
   HAL_UART_Transmit_IT(&huart3, (uint8_t*) str, strlen(str));*/
}

/*
 * @brief   Se envia un comando AT al MXChip.
 * @param
 * @retval
 */
fm_mxc_status_t MXC_ReciveStatus()
{
  fm_mxc_status_t retval = FM_MXC_FAIL;
  const uint32_t msg_ok = 0x0a0d4b4f; // equivalente a la terminación "OK\r\n"
  uint32_t *index;

  if (mxc_rx_index >= sizeof(uint32_t))
  {
    index = (uint32_t*) (mxc_rx_buffer + mxc_rx_index);
    index--;

    if (*index == msg_ok)
    {
      retval = FM_MXC_OK;
    }
  }
  return retval;
}

/*
 * @brief
 * @param
 * @retval
 */
void MXC_Delay(uint32_t time_delay)
{
  static int my_index = 0;
  uint32_t tickstart = HAL_GetTick();

  while ((HAL_GetTick() - tickstart) < time_delay)
  {
    if (my_index != mxc_rx_index)
    {
      if (my_index < BUFFER_RX_UART)
      {
        my_index++;
      }
      else
      {
        my_index = 0;
      }
    }
  }
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

  // Preparo para recibir el siguiente.
  HAL_UART_Receive_IT(&huart3, &mxc_rx_char, 1);

  /*
   *  Se recibió un nuevo caracter, lo guardo en el buffer circular. Notar que el caracter recibido
   *  se escribe en la posicion den se tenia el caracter nulo, fin del buffer circular.
   */
  if (mxc_rx_char >= ' ')
  {
    mxc_rx_buffer[mxc_rx_index] = mxc_rx_char;

    /*
     * Busco la siguiente posición del buffer circular, sera la nueva posición del terminador nulo.
     */
    if (mxc_rx_index < BUFFER_RX_UART)
    {
      mxc_rx_index++;
    }
    else
    {
      mxc_rx_index = 0;
    }
  }
}

/*** end of file ***/

