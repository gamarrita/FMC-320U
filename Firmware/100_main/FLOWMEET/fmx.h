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
 * No se puede usar typedef, como se usa con ThreadX, en eventos, y el evento es ULONG, falla al comparar.
 */
typedef ULONG fmx_events_t;
#define FMX_EVENT_EMPTY             ((fmx_events_t)0)
#define FMX_EVENT_REFRESH           ((fmx_events_t)1)
#define FMX_EVENT_KEY_DOWN          ((fmx_events_t)2)
#define FMX_EVENT_KEY_UP            ((fmx_events_t)3)
#define FMX_EVENT_KEY_ESC           ((fmx_events_t)4)
#define FMX_EVENT_KEY_ENTER         ((fmx_events_t)5)
#define FMX_EVENT_KEY_DOWN_LONG     ((fmx_events_t)6)
#define FMX_EVENT_KEY_UP_LONG       ((fmx_events_t)7)
#define FMX_EVENT_KEY_ESC_LONG      ((fmx_events_t)8)
#define FMX_EVENT_KEY_ENTER_LONG    ((fmx_events_t)9)
#define FMX_EVENT_KEY_EXT_1         ((fmx_events_t)10)
#define FMX_EVENT_KEY_EXT_2         ((fmx_events_t)11)
#define FMX_EVENT_END               ((fmx_events_t)12)

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
