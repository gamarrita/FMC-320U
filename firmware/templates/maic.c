/*
 * @file    fm_main.c
 * @brief   Implementación del punto de entrada de la aplicación.
 * @date    2025-09-14
 * @author  Flowmeet
 *
 * @details
 *   - Mantiene el estilo de secciones fijas.
 *   - Ejemplifica nombres: públicas FM_MAIN_*, privadas static snake_case.
 *   - Evita lógica pesada en ISR (solo flags).
 */

#include "fm_main.h"   // API pública de este módulo
// #include "fmx.h"    // RTOS API (si aplica)
// #include "fm_log.h" // Otros módulos de la app

/* =========================== Private Defines ============================== */
#define FM_MAIN_HEARTBEAT_SEC       (60u)
#define FM_MAIN_IDLE_SLICE_MS       (FM_MAIN_LOOP_PERIOD_MS)

/* =========================== Private Types ================================ */
typedef struct
{
    uint32_t last_heartbeat_unix;
    bool     app_ready;
} fm_main_state_t;

/* =========================== Private Data ================================= */
static fm_main_state_t g_main_state;

/* =========================== Private Prototypes =========================== */
static void app_init_services_(void);
static void app_service_heartbeat_(void);
static void app_idle_delay_ms_(uint32_t delay_ms);

/* =========================== Private Bodies =============================== */
static void app_init_services_(void)
{
    g_main_state.app_ready = true;
    g_main_state.last_heartbeat_unix = 0u;
}

static void app_service_heartbeat_(void)
{
    // Ejemplo de lógica simple: marcar un latido cada 60 s (según fuente de tiempo global).
    // La lectura del reloj del sistema NO debe hacerse en ISR.
}

static void app_idle_delay_ms_(uint32_t delay_ms)
{
    (void)delay_ms;
    // Si NO hay RTOS: busy-wait o sleep idóneo del MCU.
    // Si hay RTOS: tx_thread_sleep(ticks);
}

/* =========================== Public Bodies ================================ */
void FM_MAIN_Init(void)
{
    app_init_services_();
}

/*
 * @brief   Loop infinito Flowmeet.
 * @note    El programa principal se desarrolla en este módulo; no escribir
 *          lógica de usuario en archivos generados automáticamente por el IDE.
 */
void FM_MAIN_Main(void)
{
    for (;;)
    {
        app_service_heartbeat_();
        app_idle_delay_ms_(FM_MAIN_IDLE_SLICE_MS);
    }
}

/* =========================== Interrupts =================================== */
// ISR de ejemplo (no lógicas pesadas):
// void EXTI0_IRQHandler(void)
// {
//     // clear flag, set event, return
// }

/*** end of file ***/
