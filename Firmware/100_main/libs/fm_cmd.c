/*
 * Descripcion
 *
 * Autor:
 * Fecha: 
 */

// Includes.

// Sección define sin dependencia.

// Sección enum y typedef sin dependencia.

// Sección define, enum y typedef con dependencia.

// Debug.

// Variables non-static, primero las tipo const.

// Variables statics, primero las tipo const.

// Variables extern, las que no estan en .h.

// Prototipos funciones privadas.

// Cuerpo funciones privadas.

// Public function bodies.

// Interrupts

// ================================
// File: fm_cmd.c
// ================================
#include "fm_cmd.h"
#include <string.h>
#include <stdio.h>

extern UART_HandleTypeDef huart3;

static TX_QUEUE *cmd_queue_ptr = NULL; ///< Cola usada para comandos diferidos
static int temperature = 25; ///< Valor simulado

static void handle_temp(const char *args)
{
    char response[32];
    snprintf(response, sizeof(response), "TEMP:%d\r\n", temperature);
    HAL_UART_Transmit_IT(&huart3, (uint8_t*)response, strlen(response));
}

static void handle_log(const char *args)
{
    fm_cmd_command_t cmd;
    strncpy(cmd.line, "FM+LOG_ALL", FM_CMD_BYTE_SIZE);
    tx_queue_send(cmd_queue_ptr, &cmd, TX_NO_WAIT);
}

static void handle_log_all(const char *args)
{
    const char *log[] = { "LOG1\r\n", "LOG2\r\n", "LOG3\r\n" };
    for (int i = 0; i < 3; ++i) {
        HAL_UART_Transmit_IT(&huart3, (uint8_t*)log[i], strlen(log[i]));
    }
}

static void handle_count(const char *args)
{
    int count = 42; ///< Simula un conteo o contador
    char buf[32];
    snprintf(buf, sizeof(buf), "COUNT:%d\r\n", count);
    HAL_UART_Transmit_IT(&huart3, (uint8_t*)buf, strlen(buf));
}

static const fm_cmd_entry_t fm_commands[] = {
    { "FM+VERSION", FM_CMD_TYPE_LITERAL,  .response.literal = "FMC-320U" },
    { "FM+TEMP?",   FM_CMD_TYPE_HANDLER,  .response.handler = handle_temp },
    { "FM+LOG",     FM_CMD_TYPE_HANDLER,  .response.handler = handle_log },
    { "FM+LOG_ALL", FM_CMD_TYPE_DEFERRED, .response.handler = handle_log_all },
    { "FM+COUNT",   FM_CMD_TYPE_HANDLER,  .response.handler = handle_count },
};

#define NUM_COMMANDS (sizeof(fm_commands) / sizeof(fm_commands[0]))

/**
 * @brief Inicializa el módulo de comandos.
 * @param queue Puntero a la cola usada por el hilo de comandos
 * @retval Ninguno
 */
void FM_CMD_InitRtos(TX_QUEUE *queue)
{
    cmd_queue_ptr = queue;
}

/**
 * @brief Procesa una línea de comando recibida y ejecuta la acción correspondiente.
 * @param line Línea completa del comando
 * @retval Ninguno
 */
void fm_cmd_process_line(const char *line)
{
    for (size_t i = 0; i < NUM_COMMANDS; ++i)
    {
        if (strncmp(line, fm_commands[i].command, strlen(fm_commands[i].command)) == 0)
        {
            if (fm_commands[i].type == FM_CMD_TYPE_LITERAL)
            {
                HAL_UART_Transmit_IT(&huart3,
                                     (uint8_t*)fm_commands[i].response.literal,
                                     strlen(fm_commands[i].response.literal));
            }
            else if (fm_commands[i].type == FM_CMD_TYPE_HANDLER ||
                     fm_commands[i].type == FM_CMD_TYPE_DEFERRED)
            {
                fm_commands[i].response.handler(line);
            }
            break;
        }
    }
}

/**
 * @brief Función de entrada del hilo que procesa los comandos FM+ recibidos por UART.
 *        Esta función corre como un hilo ThreadX independiente.
 * @param input Puntero al TX_QUEUE que contiene las líneas completas a procesar
 * @retval Ninguno
 */
void FM_CMD_ThreadEntry(ULONG input)
{
    TX_QUEUE *queue = (TX_QUEUE *)input;
    fm_cmd_command_t cmd;

    while (1)
    {
        if (tx_queue_receive(queue, &cmd, TX_WAIT_FOREVER) == TX_SUCCESS)
        {
            fm_cmd_process_line(cmd.line);
        }
    }
}

/*** end of file ***/

