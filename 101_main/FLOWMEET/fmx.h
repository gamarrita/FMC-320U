/*
 *
 * @brief Header de fmx.c
 *
 * @note
 *
 */

#ifndef FMX_H_
#define FMX_H_

// includes
#include "main.h"
#include "app_threadx.h"
#include "tx_port.h"

// Typedef y enum.
typedef enum
{
	FMX_LOW_POWER_DISABLE = 0U,
	FMX_LOW_POWER_ENABLE
} FMX_LowPowerControl_t;

/*
 * Lista de eventos eventos, un bit por cada evento, máximo 32 eventos.
 * Lugo se necesitara una mascara con todos los bits de los eventos
 *
 */
typedef enum
{
	FMX_EVENT_REFRESH = 0x0001,
	FMX_EVENT_KEY_DOWN = 0x0002,
	FMX_EVENT_KEY_UP = 0x0004,
	FMX_EVENT_KEY_ESC = 0x0008,
	FMX_EVENT_KEY_ENTER = 0x0010,
	FMX_EVENT_KEY_DOWN_LONG = 0x0020,
	FMX_EVENT_KEY_UP_LONG = 0x0040,
	FMX_EVENT_KEY_ESC_LONG = 0x0080,
	FMX_EVENT_KEY_ENTER_LONG = 0x0100,
} FMX_Events_t;

// Macros, defines, microcontroller pins.

// Varibles extern.
extern const uint32_t FMX_DEBUG_UART_1_ENABLE;

// Defines.

// Mascara de eventos. Si no esta bien armada se bloquea el ThreadX.
#define FMX_EVENT_GROUP 0x01FF

// Function prototypes
UINT FMX_Init(VOID *memory_ptr);

#endif /* MODULE_H */

/*** end of file ***/