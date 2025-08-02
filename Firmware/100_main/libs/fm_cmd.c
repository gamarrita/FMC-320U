/*
 * Descripcion
 *
 * Autor:
 * Fecha: 
 */

// Includes.
#include "fm_cmd.h"
#include <string.h>
#include <stdio.h>

// Sección define sin dependencia.
#define UART3_TX_BUFFER_SIZE 32

// Sección enum y typedef sin dependencia.

// Debug.

// Variables non-static, primero las tipo const.

// Variables statics, primero las tipo const.
static int temperature = 25; ///< Valor simulado
static const fm_cmd_entry_t fm_commands[] =
{
{ "FM+VERSION?", FM_CMD_TYPE_LITERAL, .response.literal = "FMC-320U" },
{ "FM+TEMP?", FM_CMD_TYPE_HANDLER, .response.handler = FM_CMD_HandleTemp },
{ "FM+LOG_ALL?", FM_CMD_TYPE_DEFERRED, .response.handler = FM_CMD_HandleLogAll },
{ "FM+COUNT?", FM_CMD_TYPE_HANDLER, .response.handler = FM_CMD_HandleCount }, };

static TX_THREAD cmd_thread; // Hilo dedicado al procesamiento de comandos
static TX_QUEUE cmd_queue; // Cola para comandos FM+ entrante procesados desde UART.
uint8_t cmd_queue_buffer[3 * FM_CMD_BYTE_SIZE];

// Sección define, enum y typedef con dependencia.
#define NUM_COMMANDS (sizeof(fm_commands) / sizeof(fm_commands[0]))

// Variables extern, las que no están en un #include "xyz.h".

// Prototipos funciones privadas.
void ProcessLine(const char *line);

// Cuerpo funciones privadas.

/**
 * @brief Procesa una línea de comando recibida y ejecuta la acción correspondiente.
 * @param line Línea completa del comando
 * @retval Ninguno
 */
void ProcessLine(const char *line)
{
    for (size_t i = 0; i < NUM_COMMANDS; ++i)
    {
        if (strncmp(line, fm_commands[i].command, strlen(fm_commands[i].command)) == 0)
        {
            if (fm_commands[i].type == FM_CMD_TYPE_LITERAL)
            {
                HAL_UART_Transmit_DMA(&huart3, (uint8_t*) fm_commands[i].response.literal,
                        strlen(fm_commands[i].response.literal));
            }
            else if (fm_commands[i].type == FM_CMD_TYPE_HANDLER
                    || fm_commands[i].type == FM_CMD_TYPE_DEFERRED)
            {
                fm_commands[i].response.handler(line);
            }
            break;
        }
    }
}


// Cuerpo función privadas.

void FM_CMD_HandleTemp(const char *args)
{
    static char response[UART3_TX_BUFFER_SIZE];
    snprintf(response, sizeof(response), "TEMP:%d\r\n", temperature);
    HAL_UART_Transmit_DMA(&huart3, (uint8_t*) response, strlen(response));
}

void FM_CMD_HandleLogAll(const char *args)
{
    const char *log[] =
    { "LOG1\r\n", "LOG2\r\n", "LOG3\r\n" };

    for (int i = 0; i < 3; ++i)
    {
        HAL_UART_Transmit_DMA(&huart3, (uint8_t*) log[i], strlen(log[i]));
    }
}

void FM_CMD_HandleCount(const char *args)
{
    int count = 42; ///< Simula un conteo o contador
    char buf[32];
    snprintf(buf, sizeof(buf), "COUNT:%d\r\n", count);
    HAL_UART_Transmit_IT(&huart3, (uint8_t*) buf, strlen(buf));
}

/**
 * @brief Inicializa el módulo de comandos.
 * @param queue Puntero a la cola usada por el hilo de comandos
 * @retval Ninguno
 */
void FM_CMD_RtosInit(VOID *memory_ptr)
{
    // >>>> ThreadX: variables para reservar memoria.
    UINT ret_status = TX_SUCCESS;
    CHAR *pointer;
    TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*) memory_ptr;
    // <<<< Fin.

    // >>>> ThreadX: Crear hilo.
    ret_status = tx_byte_allocate(byte_pool, (VOID**) &pointer, FMX_STACK_SIZE, TX_NO_WAIT);
    if (ret_status != TX_SUCCESS)
    {
        __disable_irq();
        FM_DEBUG_LedError(1);
        while (1);
    }

    ret_status = tx_thread_create(&cmd_thread, "CMD_THREAD", FM_CMD_ThreadEntry, (ULONG )&cmd_queue,
            pointer, FMX_STACK_SIZE, FMX_THREAD_PRIORITY_10, FMX_THRESHOLD_10, FMX_SLICE_0, TX_AUTO_START);

    if (ret_status != TX_SUCCESS)
    {
        __disable_irq();
        FM_DEBUG_LedError(1);
        while (1);
    }
    // <<<< Fin

    // >>>> ThreadX: Crear hilo.
    ret_status = tx_queue_create(&cmd_queue, "CMD_QUEUE", FM_CMD_ULONG_SIZE, cmd_queue_buffer,
            sizeof(cmd_queue_buffer));
    if (ret_status != TX_SUCCESS)
    {
        __disable_irq();
        FM_DEBUG_LedError(1);
        while (1);
    }
    // <<<< Fin
}


/**
 * @brief Función de entrada del hilo que procesa los comandos FM+XYZ recibidos por UART.
 *        Esta función corre como un hilo ThreadX independiente.
 * @param input Puntero al TX_QUEUE que contiene las líneas completas a procesar
 * @retval Ninguno
 */
void FM_CMD_ThreadEntry(ULONG input)
{
    TX_QUEUE *queue = (TX_QUEUE*) input;
    fm_cmd_command_t cmd;

    while (1)
    {
        if (tx_queue_receive(queue, &cmd, TX_WAIT_FOREVER) == TX_SUCCESS)
        {
            ProcessLine(cmd.line);
        }
    }
}

// Interrupts

/*** end of file ***/

