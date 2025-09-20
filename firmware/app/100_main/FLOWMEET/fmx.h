/**
 * @file fmx.h
 * @brief FMX public API and system events.
 *
 * Exposes shared types, event identifiers, and ThreadX related hooks used by
 * the FLOWMEET application layer.
 * @details Resume riesgos operativos, trazabilidad REQ-FMX y acuerdos con ThreadX/HAL para el modulo.
 */
#ifndef FMX_H_
#define FMX_H_

#ifdef __cplusplus
extern "C" {
#endif

// --- Includes ---
#include "main.h"
#include "app_threadx.h"
#include "tx_port.h"
#include "tx_api.h"

// --- Public Constants ---
// Codigos de eventos evitan perder pulsos aun con fallas.
// Codificacion en ULONG sigue REQ-FMX-QUEUE-001 y la granularidad de ThreadX.
// Alineados con la granularidad de cola ThreadX para tiempos deterministas.
/**
 * Event bitfield type for the FMX event queue.
 * Alineado con el ancho de ULONG requerido por ThreadX.
 */
typedef ULONG fmx_events_t;

#define FMX_EVENT_EMPTY             ((fmx_events_t)0u)
#define FMX_EVENT_MENU_REFRESH      ((fmx_events_t)1u)
#define FMX_EVENT_KEY_DOWN          ((fmx_events_t)2u)
#define FMX_EVENT_KEY_UP            ((fmx_events_t)3u)
#define FMX_EVENT_KEY_ESC           ((fmx_events_t)4u)
#define FMX_EVENT_KEY_ENTER         ((fmx_events_t)5u)
#define FMX_EVENT_KEY_DOWN_LONG     ((fmx_events_t)6u)
#define FMX_EVENT_KEY_UP_LONG       ((fmx_events_t)7u)
#define FMX_EVENT_KEY_ESC_LONG      ((fmx_events_t)8u)
#define FMX_EVENT_KEY_ENTER_LONG    ((fmx_events_t)9u)
#define FMX_EVENT_KEY_EXT_1         ((fmx_events_t)10u)
#define FMX_EVENT_KEY_EXT_2         ((fmx_events_t)11u)
#define FMX_EVENT_TIME_OUT          ((fmx_events_t)12u)
#define FMX_EVENT_END               ((fmx_events_t)13u)

// Stack dimensionado para cubrir GUI y tareas de monitoreo.
#define FMX_STACK_SIZE              (1024u * 16u)
// Prioridad definida por REQ-FMX-THREAD-010.
#define FMX_THREAD_PRIORITY_10      (10u)
// Threshold mantiene preemption deterministica.
#define FMX_THRESHOLD_10            (10u)
// Sin time-slice para evitar jitter en menu.
#define FMX_SLICE_0                 (0u)

// --- Public Types ---
/**
 * Low power control states.
 * Indica cuando la UI puede apagar perifericos para ahorrar energia.
 */
typedef enum {
    FMX_LOW_POWER_DISABLE = 0u,
    FMX_LOW_POWER_ENABLE
} FMX_LowPowerControl_t;

/**
 * General status codes for FMX modules.
 * El prefijo consistente facilita la trazabilidad de fallas.
 */
typedef enum {
    FMX_STATUS_NULL = 0u,
    FMX_STATUS_OK,
    FMX_STATUS_ERROR,
    FMX_STATUS_BUSY,
    FMX_STATUS_TIMEOUT,
    FMX_STATUS_INVALIDA_PARAM,
    FMX_STATUS_OUT_OF_RANGE
} fmx_status_t;

/**
 * Flow rate state machine for caudal detection.
 * Describe transiciones utilizadas por hilos y callbacks.
 */
typedef enum {
    FMX_RATE_OFF = 0u,
    FMX_RATE_TO_ON,
    FMX_RATE_ON,
    FMX_RATE_TO_OFF
} fmx_rate_status_t;

// --- Extern Variables ---
// Flag cableado via PCB habilita el canal debug.
extern const uint32_t FMX_DEBUG_UART_1_ENABLE;
// Cola acepta comandos diferidos provenientes de ISR.
extern TX_QUEUE fmx_deferred_cmd_queue;

// --- Public API ---
/**
 * Recupera el estado actual del flujo.
 * @return Valor de @ref fmx_rate_status_t para UI y logging.
 * @details Permite validar coherencia de sensores en runtime.
 */
fmx_rate_status_t FMX_GetRateStatus(void);

/**
 * Inicializa el modulo FMX y sus recursos de ThreadX.
 * @param memory_ptr Pool de memoria provisto por el RTOS.
 * @return Codigo de estado devuelto por ThreadX.
 * @details Verifica cada llamado segun REQ-FMX-INIT-001.
 */
UINT FMX_Init(VOID *memory_ptr);

/**
 * Enciende la luz de fondo del LCD y reinicia su temporizador.
 * @details Usa timers ThreadX para controlar consumo.
 */
void FMX_LcdBackLightOn(void);

/**
 * Garantiza que exista un evento de refresco de menu en la cola.
 * @details Mantiene compliance con REQ-FMX-GUI-002.
 */
void FMX_RefreshEventTrue(void);

/**
 * Dispara la secuencia de conexion del esclavo Bluetooth.
 * @details Despierta el hilo BT via semaphore sin recurrir a polling.
 */
void FMX_Trigger_BluetoothSlave(void);

#ifdef __cplusplus
}
#endif

#endif // FMX_H_
/*** END OF FILE ***/









