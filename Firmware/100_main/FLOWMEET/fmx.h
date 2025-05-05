/*
 * Autor: Daniel H Sagarra
 *
 * Notas:
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
  FMX_LOW_POWER_DISABLE = 0U, FMX_LOW_POWER_ENABLE
} FMX_LowPowerControl_t;

/*
 * Lista de eventos eventos, un bit por cada evento, máximo 32 eventos.
 * Lugo se necesitara una mascara con todos los bits de los eventos
 */
typedef enum
{
  FMX_EVENT_EMPTY = 0, // Sin evento a procesar o tx_queue_receive(() devuelve TX_QUEUE_EMPTY
  FMX_EVENT_REFRESH,
  FMX_EVENT_KEY_DOWN,
  FMX_EVENT_KEY_UP,
  FMX_EVENT_KEY_ESC,
  FMX_EVENT_KEY_ENTER,
  FMX_EVENT_KEY_DOWN_LONG,
  FMX_EVENT_KEY_UP_LONG,
  FMX_EVENT_KEY_ESC_LONG,
  FMX_EVENT_KEY_ENTER_LONG,
  FMX_EVENT_KEY_EXT_1,
  FMX_EVENT_KEY_EXT_2,
} fmx_events_t;

typedef enum
{
  FMX_STATUS_OK = 0x00, FMX_STATUS_ERROR = 0x01, FMX_STATUS_BUSY = 0x02, FMX_STATUS_TIMEOUT = 0x03
} fmx_status_t;

// Macros, defines, microcontroller pins.

// Varibles extern.
extern const uint32_t FMX_DEBUG_UART_1_ENABLE;

// Defines.

// Mascara de eventos. Si no esta bien armada se bloquea el ThreadX.
#define FMX_EVENT_GROUP 0x01FF

// Function prototypes
UINT FMX_Init(VOID *memory_ptr);
void FMX_LcdBackLightOn();
void FMX_RefreshEventTrue();


#endif /* MODULE_H */

/*** end of file ***/
