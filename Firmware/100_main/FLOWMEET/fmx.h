/*
 * Autor: Daniel H Sagarra
 *
 * Notas:
 *
 */

#ifndef FMX_H_
#define FMX_H_

// Comienzo sección includes.
#include "main.h"
#include "app_threadx.h"
#include "tx_port.h"

// Primera sección define.

// Sección typedef y enum.

typedef ULONG fmx_events_t;

typedef enum
{
    FMX_LOW_POWER_DISABLE = 0U, FMX_LOW_POWER_ENABLE
} FMX_LowPowerControl_t;

typedef enum
{
    FMX_STATUS_OK = 0x00,       //
    FMX_STATUS_ERROR,           //
    FMX_STATUS_BUSY,            //
    FMX_STATUS_TIMEOUT,         //
    FMX_STATUS_INVALIDA_PARAM,  //
    FMX_STATUS_OUT_OF_RANGE     //
} fmx_status_t;

typedef enum
{
    FMX_RATE_OFF = 0, FMX_RATE_TO_ON, FMX_RATE_ON, FMX_RATE_TO_OFF,
} fmx_rate_status_t;

// Varibles extern.
extern const uint32_t   FMX_DEBUG_UART_1_ENABLE;
extern TX_QUEUE         fmx_deferred_cmd_queue;

// Ultimas sección defines (solos los que necesitan de una definición declaración anterior).

/*
 * Lista de eventos eventos, un bit por cada evento, máximo 32 eventos. No se usa typedef porque se
 * necesita un cast al tipo de datos ULONG.
 */
#define FMX_EVENT_EMPTY             ((fmx_events_t)0)
#define FMX_EVENT_MENU_REFRESH      ((fmx_events_t)1)   // El propio menu pide refresco
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
#define FMX_EVENT_TIME_OUT          ((fmx_events_t)12) // Expiro el tiempo maximo que el RTOS puede estar en stop mode.
#define FMX_EVENT_END               ((fmx_events_t)13)

// Function prototypes
fmx_rate_status_t FMX_GetRateStatus();
UINT FMX_Init(VOID *memory_ptr);
void FMX_LcdBackLightOn();
void FMX_RefreshEventTrue();
void FMX_Trigger_BluetoothSlave();

#endif /* MODULE_H */

/*** end of file ***/
