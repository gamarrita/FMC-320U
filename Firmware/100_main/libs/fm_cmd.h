/*
 *
 * Instrucciones:
 * - #include "fm_main.h // copiar esta linea en main.c
 * - Nueva carpeta con el nombre FLOWMEET en el workspace
 * - Hacer que la carpeta FLOWWMEET ser "source floder"
 * - En las propiedades del proyecto la carpeta FLOWMEET de ser "include paths" en C/C++ Build->Settings MCU/MPU GVV Compiler
 * - Agregar la sentencia FM_MAIN_Main(); en el while infinico del main.c
 */

#ifndef FM_MAIN_H_
#define FM_MAIN_H_

// includes
#include "tx_api.h"
#include "main.h"
#include "usart.h"

// Sección define sin dependencia.
#define FM_CMD_ULONG_SIZE 16 // Tamaño del mensaje en ULONGs, es el maximo que maneja una cola.
#define FM_CMD_BYTE_SIZE  (sizeof(ULONG) * FM_CMD_ULONG_SIZE) // Tamaño del mensje en bytes

// Sección enum y typedef sin dependencia.

// Sección define, enum y typedef con dependencia.

typedef struct
{
    char line[FM_CMD_BYTE_SIZE];
} fm_cmd_command_t;

typedef enum
{
    FM_CMD_TYPE_LITERAL, //
    FM_CMD_TYPE_HANDLER, //
    FM_CMD_TYPE_DEFERRED //
} fm_cmd_type_t;

typedef struct
{
    const char *command;
    fm_cmd_type_t type;
    union
    {
        const char *literal;
        void (*handler)(const char *args);
    } response;
} fm_cmd_entry_t;

// Varibles extern

// Function prototypes
void FM_CMD_InitRtos(TX_QUEUE *queue);
void FM_CMD_ThreadEntry(ULONG input);

#endif  // FM_MAIN_H

/*** end of file ***/
