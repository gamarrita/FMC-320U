/*
 * @brief   Este modulo usa el UART3 para enviar y recibir datos del MXCHIP.
 *          Las respuestas del MXCHIP se re-envian al UART con motivo de debug.
 *
 * Fecha: 14/12/2024
 * Autor. DHS
 * Version: 1
 * Resumen: version inicial.
 *
 */

// Includes.
#include "fm_mxc.h"
#include "string.h"
#include "fm_cmd.h"
#include "stm32u5xx_ll_usart.h"
#include "fm_debug.h"

// Sección define sin dependencia.
#define FALSE 0
#define TRUE  1
#define SEND_AT_WAIT    3000 // Tiempo de espera por respuesta al enviar comando AT
#define BUFFER_RX_UART  80 // Tamaño del buffer de recepción de datos, máximo.
#define WAIT_FOR_5MS    5
#define WAIT_FOR_1MS    1
#define MAX_FM_LINE_LEN 128
#define FM_LINE_READY (((ULONG) 1) << 0)

// Sección enum y typedef sin dependencia.

// Lista de comandos AT del modulo MXChip EMC-3080-P
typedef enum
{
    AT_PLUS, AT_UARTE,  // Consulta si UART tiene eco.
    AT_UARTE_ON, // Activa el eco, el eco actúa sobre cada caracter recibido, no espera el \r
    AT_UARTE_OFF,       // Desactiva el eco.
    AT_STANDBY, // Pasa a modo de bajo consumo. Para salir de este modo se hace hard reset
    AT_BROLE_MASTER,    // Configura a MXChip como maestro, para imprimir
    AT_BROLE_SLAVE,  // Configura a MXChip como esclavo, para descarga de log.
    AT_BINQ_NAME,       // Escanea buscando un nombre especifico de impresora.
    AT_BCONN_0,         // Se conecta al primer resultado del escaneado.
    AT_BLE,             // Consulta el estado del bluetooth,
    AT_BLE_ON,          // Activa el modulo bluetooth.
    AT_BLE_OFF,         // Desactiva el modulo bluetooth.
    AT_EVENT_ON, // Los eventos notifican por el puerto UART. El mas relevante es el de AT+BINQ=1,....
    AT_EVENT_OFF,       // No se notifican los evento.
    AT_BSENDRAW,        // Pasa la impresora a modo transparente.
    AT_REBOOT,
    AT_BSINQ,
    AT_BSERVUUID_1800,
    AT_BTXUUID_2A00,
    AT_BRXUUID_2A00,
    AT_FWVER,
} at_id_t;

/*
 *  Parámetros de commandos AT del MXChip. En el manual de modulo para el comando se tiene
 *  el nombre, y las respuesta. Esta estructura sirve para:
 *  En el envió del comando se selecciona un string con el nombre del comando mediante el ID
 *  En la respuesta se cuentan los retornos de carro, al tener la cantidad de retornos de carro
 *  esperados se considera la respuesta completa.
 *  Al enviar comandos sucesivos, uno tras otro sin esperar respuesta, el MXChip fallaba, se
 *  tiene un parametro llamado delay, que es la espera de cada comando, antes de permitir enviar
 *  otro.
 */
typedef struct
{
    const char *command;    // Nombre del comando AT que entiende el MXChip
    const at_id_t id;       // ID del comando
    const uint32_t delay;   // Tiempo que tarda en procesar el MXChip al comando
    const uint8_t crs;      // Cantidad de retornos de carro en el mensaje de respuesta.
} at_commad_t;

typedef enum
{
    FM_TYPE_LITERAL, FM_TYPE_HANDLER, FM_TYPE_DEFERRED
} fm_command_type_t;

typedef struct
{
    const char *command;
    fm_command_type_t type;
    union
    {
        const char *literal;
        void (*handler)(const char *args);
    };
} fm_command_t;

// Sección define, enum y typedef con dependencia.

// Debug.

// Variables non-static, primero las tipo const.

// Variables statics, primero las tipo const.
static TX_QUEUE *queue_ptr = NULL;          // Puntero a la cola de comandos
static char fm_line_buffer[FM_CMD_BYTE_SIZE];    // Buffer para ensamblar la línea completa
static uint8_t fm_line_index = 0;           // Índice de escritura en el buffer

// Project variables, non-static, at least used in other file.
uint8_t huart3_rx_buffer = 0; // Ultimo par de caracteres que respondió el MXChip.

// Variables extern, las que no estan en .h.

// Global variables, statics.

// Lista de comandos AT del EMC-3080
static const at_commad_t at_list[] =
{
{ "+++", AT_PLUS, 25, 1 },
{ "AT+UARTE?\r", AT_UARTE, 1, 2 },
{ "AT+UARTE=ON\r", AT_UARTE_ON, 1, 2 },
{ "AT+UARTE=OFF\r", AT_UARTE_OFF, 1, 2 },
{ "AT+STANDBY\r", AT_STANDBY, 1, 2 },
{ "AT+BROLE=1\r", AT_BROLE_MASTER, 1, 2 },
{ "AT+BROLE=0\r", AT_BROLE_SLAVE, 1, 2 },
{ "AT+BINQ=1,FM-BLE\r", AT_BINQ_NAME, 1, 3 },
{ "AT+BCONN=0\r", AT_BCONN_0, 1, 3 },
{ "AT+BLE?\r", AT_BLE, 1, 2 },
{ "AT+BLE=ON\r", AT_BLE_ON, 1, 2 },
{ "AT+BLE=OFF\r", AT_BLE_OFF, 1, 2 },
{ "AT+BEVENT=ON\r", AT_EVENT_ON, 1, 2 },
{ "AT+BEVENT=OFF\r", AT_EVENT_OFF, 1, 2 },
{ "AT+BSENDRAW\r", AT_BSENDRAW, 100, 2 },
{ "AT+REBOOT\r", AT_REBOOT, 1, 2 },
{ "AT+BSINQ\r", AT_BSINQ, 1, 2 },
{ "AT+BSERVUUID=1800\r", AT_BSERVUUID_1800, 1, 2 },
{ "AT+BTXUUID=2A00\r", AT_BTXUUID_2A00, 1, 2 },
{ "AT+BRXUUID=2A00\r", AT_BRXUUID_2A00, 1, 2 },
{ "AT+FWVER?\r", AT_FWVER, 1, 2 } };

//#define FM_MXC_LINE_READY (1U << 0)

// Private function prototypes.
fm_mxc_status_t SendAt(at_id_t id, int retry);
void FM_HandleLogDeferred(const char *args); // solo declarada, no se usa directamente

// Private function bodies.

// Public function bodies.

/**
 * @brief Inicializa la recepción UART con interrupciones.
 * @param queue Puntero a la cola donde se enviarán los comandos recibidos
 * @retval Ninguno
 */
void FM_MXC_InitRtos(TX_QUEUE *queue)
{
    queue_ptr = queue;
}

/*
 * @brief   manejo el pin del MXChip enable, y su alimentación.
 * @param   mode: ver switch(mode)
 * @retval  ninguno.
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

    __HAL_UART_CLEAR_OREFLAG(&huart3);
    __HAL_UART_CLEAR_FEFLAG(&huart3);
    __HAL_UART_CLEAR_NEFLAG(&huart3);
    __HAL_UART_CLEAR_PEFLAG(&huart3);

    HAL_UART_Receive_IT(&huart3, &huart3_rx_buffer, 1);

    FM_MXC_Mode(FM_MXC_MODE_OFF);
    tx_thread_sleep(10);

    FM_MXC_Mode(FM_MXC_MODE_ON);
    tx_thread_sleep(200); // Si no le doy tiempo al EMC-3080 luego de encender, no responde.
}

/*
 * @brief
 * @param   Ninguno.
 * @retval  Ninguno.
 */
void FM_MXC_PowerOff()
{
    tx_thread_sleep(100);
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
    SendAt(AT_STANDBY, 1);
}

/*
 * @brief
 * @param   Ninguno.
 * @retval  Ninguno.
 */
void FM_MXC_Wakeup()
{
    FM_MXC_Mode(FM_MXC_MODE_DISABLE);
    tx_thread_sleep(1);
    FM_MXC_Mode(FM_MXC_MODE_ENABLE);
    tx_thread_sleep(200);
}

/*
 * @brief   Se conecta con impresoras térmicas con el nombre "FM-BLE". Esta función deja al modulo MXChip en
 *          modo transparente. Si se quieren enviar comandos, se debe volver al modo AT.
 * @param   Ninguno.
 * @retval  Ninguno.
 */
fmx_status_t FM_MXC_ConnectMaster()
{
    fm_mxc_status_t retval;

    retval = SendAt(AT_BROLE_MASTER, 1);
    if (retval == FM_MXC_FAIL)
    {
        return FMX_STATUS_ERROR;
    }

    retval = SendAt(AT_BINQ_NAME, 1);
    if (retval == FM_MXC_FAIL)
    {
        return FMX_STATUS_ERROR;
    }

    retval = SendAt(AT_BCONN_0, 1);
    if (retval == FM_MXC_FAIL)
    {
        return FMX_STATUS_ERROR;
    }

    retval = SendAt(AT_BSENDRAW, 1);
    if (retval == FM_MXC_FAIL)
    {
        return FMX_STATUS_ERROR;
    }
    return FMX_STATUS_OK;
}

/*
 * @brief	Se enciende el modulo en modo maestro
 * @param   Ninguno.
 * @retval  Ninguno.
 */
fmx_status_t FM_MXC_ConnectSlave()
{
    fm_mxc_status_t retval;

    FM_MXC_PowerOn();

    tx_thread_sleep(25);

    retval = SendAt(AT_BROLE_SLAVE, 1);
    if (retval == FM_MXC_FAIL)
    {
        return FMX_STATUS_ERROR;
    }

    retval = SendAt(AT_BSENDRAW, 1);
    if (retval == FM_MXC_FAIL)
    {
        return FMX_STATUS_ERROR;
    }

    return FMX_STATUS_OK;
}

/*
 * @brief   Se envía un comando AT al MXChip.
 * @param   id, identificador del comando a enviar.
 *          retry, cantidad de intentos hasta recibir una respuesta de "OK".
 *          delay,  máximo tiempo de espera por intento.
 * @retval  MXC_OK si el MXC responde con OK dentro de los intentos.
 *          MXC_FAIL si no se recibe MCX_OK al agotar los intentos.
 */
fm_mxc_status_t SendAt(at_id_t id, int retry)
{
    uint32_t time_delay = SEND_AT_WAIT;
    uint8_t str_ends_counter;
    fm_mxc_status_t retval = FM_MXC_FAIL;
    uint32_t tickstart;

    str_ends_counter = at_list[id].crs;
    huart3_rx_buffer = 0;

    HAL_UART_Transmit_IT(&huart3, (uint8_t*) at_list[id].command, strlen(at_list[id].command));

    /*
     * Luego de transmitir espero respuesta, una respuesta se transmitió hasta el final si llegaron
     * la cantidad de 0x0a esperados al comando AT transmitido.
     */
    tickstart = HAL_GetTick();

    for (int this_loop = TRUE; this_loop;)
    {

        if ((HAL_GetTick() - tickstart) > time_delay)
        {
            this_loop = FALSE;
        }

        if (huart3_rx_buffer == 0x0a)
        {
            huart3_rx_buffer = 0;
            str_ends_counter--;
        }

        if (str_ends_counter == 0)
        {
            this_loop = FALSE;
            retval = FM_MXC_OK;
        }
    }

    /*
     * Al enviar dos comandos AT seguidos, el EMC-3080 falla frecuentemente al procesar el segundo, esto aunque
     * se espera la correcta respuesta del primero.Se deben hacer mas pruebas para determinar si esto es un
     * problema del EMC-3080, o algo que esta haciendo mal este firmware. Por el momento la solución que se
     * implemento es esperar un tiempo, determinado experimentalmente, luego del envío de cada comando. El
     * tiempo de espera es un parámetro en la definición de cada comando AT.
     */
    tx_thread_sleep(at_list[id].delay);

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
    HAL_UART_Transmit_IT(&huart3, (uint8_t*) str, strlen(str));
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
    static int led = 0;

    led ^= 1;

    FM_DEBUG_LedSginal(led);

    if (huart->Instance == USART3)
    {
        // Volver a armar recepción
        if (HAL_UART_Receive_IT(&huart3, &c, 1) != HAL_OK)
        {
            FM_DEBUG_LedError(1);
        }
        huart3_rx_buffer = c;
    }

    __HAL_UART_DISABLE_IT(&huart3, UART_IT_RXNE);
    __HAL_UART_CLEAR_FLAG(&huart3, UART_FLAG_RXNE);
    __HAL_UART_ENABLE_IT(&huart3, UART_IT_RXNE);

    /*
     static uint8_t c = 0;

     HAL_UART_Receive_IT(&huart3, &c, 1);   // Prepara la siguiente recepción
     huart3_rx_buffer = c;

     if (c == '\n' || c == '\r')
     {
     if (fm_line_index > 0 && fm_line_index < FM_CMD_BYTE_SIZE)
     {
     fm_line_buffer[fm_line_index] = '\0'; // Finaliza la línea

     fm_cmd_command_t cmd;
     strncpy(cmd.line, fm_line_buffer, FM_CMD_BYTE_SIZE);

     if (queue_ptr != NULL)
     {
     //           tx_queue_send(queue_ptr, &cmd, TX_NO_WAIT);
     }

     fm_line_index = 0; // reinicia el buffer
     }
     }
     else
     {
     if (fm_line_index < FM_CMD_BYTE_SIZE - 1)
     {
     fm_line_buffer[fm_line_index++] = (char) c;
     }
     else
     {
     fm_line_index = 0; // overflow, descarta línea
     }
     }*/

}

/*** end of file ***/

