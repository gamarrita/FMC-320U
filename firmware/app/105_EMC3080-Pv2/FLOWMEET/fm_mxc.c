/*
 * @brief   Este modulo usa el UART3 para enviar y recibir datos del MAXCHIP, las respuesta las envía
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
#include "fm_debug.h"
#include "usart.h"

// Typedef.

// Lista de comandos AT
typedef enum
{
  AT_PLUS, AT_UARTE,         // Consulta si UART tiene eco.
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
  const at_id_t id;
  const uint32_t delay;  // retarmp que necesita el comando antes de prcesar el siguiente.
  const uint8_t crs;   // retornos de carro
} at_command_t;

// Const data.

// Defines.
#define FALSE 0
#define TRUE  1
#define SEND_AT_WAIT    5000 // Tiempo de espera por respuesta al enviar comando AT
#define BUFFER_RX_UART  80 // Tamaño del buffer de recepción de datos, máximo.
#define WAIT_FOR_5MS    5
#define WAIT_FOR_1MS    1

// Debug.

// Project variables, non-static, at least used in other file.
uint8_t huart3_rx_buffer = 0;    // Ultimo par de caracteres que respondió el MXChip.

// External variables.
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart1;

/*
 * Lista de comandos AT del EMC-3080, esta lista solo implementa los que como respuesta solo se espera OK.
 */
const at_command_t at_list[] =
{
    {
        "+++",
        AT_PLUS,
        100,
        1 },
    {
        "AT+UARTE?\r",
        AT_UARTE,
        10,
        2 },
    {
        "AT+UARTE=ON\r",
        AT_UARTE_ON,
        10,
        2 },
    {
        "AT+UARTE=OFF\r",
        AT_UARTE_OFF,
        10,
        2 },
    {
        "AT+STANDBY\r",
        AT_STANDBY,
        10,
        2 },
    {
        "AT+BROLE=1\r",
        AT_BROLE_MASTER,
        10,
        2 },
    {
        "AT+BINQ=1,FM-BLE\r", // FM IMPRESORA debe ser el nombre de la impresora.
        AT_BINQ_NAME,
        10,
        3 },
    {
        "AT+BCONN=0\r",
        AT_BCONN_0,
        10,
        3 },
    {
        "AT+BLE?\r",
        AT_BLE,
        10,
        2 },
    {
        "AT+BLE=ON\r",
        AT_BLE_ON,
        10,
        2 },
    {
        "AT+BLE=OFF\r",
        AT_BLE_OFF,
        10,
        2 },

    {
        "AT+BEVENT=ON\r",
        AT_EVENT_ON,
        10,
        2 },
    {
        "AT+BEVENT=OFF\r",
        AT_EVENT_OFF,
        10,
        2 },
    {
        "AT+BSENDRAW\r",
        AT_BSENDRAW,
        1000,
        2 },
    {
        "AT+REBOOT\r",
        AT_REBOOT,
        10,
        2 },
    {
        "AT+BSINQ\r",
        AT_BSINQ,
        10,
        2 },
    {
        "AT+BSERVUUID=1800\r",
        AT_BSERVUUID_1800,
        10,
        2 },
    {
        "AT+BTXUUID=2A00\r",
        AT_BTXUUID_2A00,
        10,
        2 },
    {
        "AT+BRXUUID=2A00\r",
        AT_BRXUUID_2A00,
        10,
        2 },
    {
        "AT+FWVER?\r",
        AT_FWVER,
        10,
        2 }, };

// Global variables, statics.

// Private function prototypes.
fm_mxc_status_t MXC_ReciveStatus();
fm_mxc_status_t MXC_SendAT(at_id_t id, int retry);

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
void FM_MXC_Mode(fm_mxc_mode_t mode)
{
  switch (mode)
  {
  case FM_MXC_MODE_OFF:
    HAL_GPIO_WritePin(MXC_ENABLE_GPIO_Port, MXC_ENABLE_Pin, GPIO_PIN_RESET); // Linea enable de chip disable
    HAL_GPIO_WritePin(MXC_ON_GPIO_Port, MXC_ON_Pin, GPIO_PIN_SET); // Quito alimentación
    break;
  case FM_MXC_MODE_ON:
    HAL_GPIO_WritePin(MXC_ENABLE_GPIO_Port, MXC_ENABLE_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(MXC_ON_GPIO_Port, MXC_ON_Pin, GPIO_PIN_RESET);
    break;
  case FM_MXC_MODE_DISABLE:
    HAL_GPIO_WritePin(MXC_ENABLE_GPIO_Port, MXC_ENABLE_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MXC_ON_GPIO_Port, MXC_ON_Pin, GPIO_PIN_RESET);
  case FM_MXC_MODE_ENABLE:
    HAL_GPIO_WritePin(MXC_ENABLE_GPIO_Port, MXC_ENABLE_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(MXC_ON_GPIO_Port, MXC_ON_Pin, GPIO_PIN_RESET);
    break;
  default:
    HAL_GPIO_WritePin(MXC_ENABLE_GPIO_Port, MXC_ENABLE_Pin, GPIO_PIN_RESET); // Linea enable de chip disable
    HAL_GPIO_WritePin(MXC_ON_GPIO_Port, MXC_ON_Pin, GPIO_PIN_SET); // Quito alimentación
  }
}

/*
 * @brief   Se conecta con impresoras térmicas con el nombre "FM-BLE"
 * @Note
 * @param   Ninguno.
 * @retval  Ninguno.
 */
void FM_MXC_PowerOn()
{
  HAL_UART_MspInit(&huart3);
  //MX_USART3_UART_Init();

  // buscar como des-habilitar la interrupción si no estoy usando impresora
  HAL_UART_Receive_IT(&huart3, &huart3_rx_buffer, 1);

  FM_MXC_PowerOff();
  HAL_Delay(10);

  FM_MXC_Mode(FM_MXC_MODE_ON);
  HAL_Delay(2000);
}

/*
 * @brief
 * @Note
 * @param   Ninguno.
 * @retval  Ninguno.
 */
void FM_MXC_PowerOff()
{
  HAL_UART_MspDeInit(&huart3);
  FM_MXC_Mode(FM_MXC_MODE_OFF);
}

/*
 * @brief   Se conecta con impresoras térmicas con el nombre "FM-BLE"
 * @Note
 * @param   Ninguno.
 * @retval  Ninguno.
 */
void FM_MXC_Sleep()
{
  MXC_SendAT(AT_STANDBY, 1);
}

/*
 * @brief
 * @Note
 * @param   Ninguno.
 * @retval  Ninguno.
 */
void FM_MXC_Wakeup()
{
  FM_MXC_Mode(FM_MXC_MODE_DISABLE);
  HAL_Delay(10);
  FM_MXC_Mode(FM_MXC_MODE_ENABLE);
  HAL_Delay(2000);
}

/*
 * @brief   Se conecta con impresoras térmicas con el nombre "FM-BLE"
 * @Note    Esta función deja al modulo MXChip en modo transparente. Si se quieren enviar comandos, se debe
 *          volver al modo AT.
 * @param   Ninguno.
 * @retval  Ninguno.
 */
void FM_MXC_BTConnect()
{
  fm_mxc_status_t retval;

  retval = MXC_SendAT(AT_BINQ_NAME, 1);
  if (retval == FM_MXC_FAIL)
  {
    FM_DEBUG_LedError(1);
  }

  retval = MXC_SendAT(AT_BCONN_0, 1);
  if (retval == FM_MXC_FAIL)
  {
    FM_DEBUG_LedError(1);
  }
}

void FM_MXC_ATMode()
{
  MXC_SendAT(AT_PLUS, 1);
}

/*
 * @brief   Se envía un comando AT al MXChip.
 * @param   id, identificador del comando a enviar.
 *          retry, cantidad de intentos hasta recibir una respuesta de "OK".
 *          delay,  máximo tiempo de espera por intento.
 * @retval  MXC_OK si el MXC responde con OK dentro de los intentos.
 *          MXC_FAIL si no se recibe MCX_OK al agotar los intentos.
 */
fm_mxc_status_t MXC_SendAT(at_id_t id, int retry)
{
  uint32_t time_delay = SEND_AT_WAIT;
  uint8_t str_ends_counter;
  fm_mxc_status_t retval = FM_MXC_FAIL;
  uint32_t tickstart;

  str_ends_counter = at_list[id].crs;
  huart3_rx_buffer = 0;

  HAL_UART_Transmit_IT(&huart3, (uint8_t*) at_list[id].command, strlen(at_list[id].command));

  /*
   * Luego de transmitir espero respuesta, una respuesta se transmitió hasta el final si llegaron la
   * cantidad de 0x0d0a corresondientes al comando AT transmitido.
   */
  tickstart = HAL_GetTick();
  while (((HAL_GetTick() - tickstart) < time_delay) && str_ends_counter)
  {
    if (huart3_rx_buffer == 0x0a)
    {
      huart3_rx_buffer = 0;
      str_ends_counter--;
    }
  }

  HAL_Delay(at_list[id].delay);
  return retval;
}

/*
 * @brief
 * @param
 * @retval
 *
 */
void FM_MXC_Print(char *str)
{
  MXC_SendAT(AT_BSENDRAW, 1);

  /*
   *  Pese a que espero respuesta, segundo 0x0a, si no agrego retardo adicional, perecería que MXChip no esta
   *  preparado para transmitir.
   */
  HAL_UART_Transmit_IT(&huart3, (uint8_t*) str, strlen(str));

  HAL_Delay(250);

  MXC_SendAT(AT_PLUS, 1);
}

// Interrupts

/*
 * @brief
 * @param
 * @retval
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  static uint8_t c = 0;

  // Preparo para recibir el siguiente que el microcontrolador recibe del MCXChip.
  HAL_UART_Receive_IT(&huart3, &c, 1);
  huart3_rx_buffer = c;
}

/*** end of file ***/

