/**
 * @file fm_style_example.c
 * @brief Implementacion de referencia para el estilo FM.
 *
 * Demuestra como estructurar un archivo C con secciones, nombres y comentarios
 * consistentes. Mantiene compatibilidad con ThreadX y HAL sin modificar su API.
 */

// --- Includes ---
#include "fm_style_example.h"
#include "fm_debug.h"

#include <string.h>

// --- Module Defines ---

#define TRUE    (1u)
#define FALSE   (0u)

#define FM_STYLE_EXAMPLE_SAFETY_MARGIN   (4u)

// --- Static Data ---

static fm_style_example_context_t *s_context = NULL;
static TX_MUTEX                     *s_lock = NULL;
static uint32_t                      s_last_filtered = 0u;
static uint8_t                       s_shutdown_requested = FALSE;

// --- Static Functions ---

// Resetea el contexto a valores conocidos.
static void ResetContext(fm_style_example_context_t *context)
{
    memset(context, 0, sizeof(*context));
    context->state = FM_STYLE_EXAMPLE_STATE_INIT;
}

// Actualiza el estado segun la muestra reciente.
static void UpdateStateOnSample(uint32_t raw_count)
{
    if (s_context == NULL) {
        return;
    }

    if (raw_count == 0u) {
        s_context->state = FM_STYLE_EXAMPLE_STATE_IDLE;
    } else {
        s_context->state = FM_STYLE_EXAMPLE_STATE_ACTIVE;
    }
}

// Publica un mensaje de diagnostico opcional.
static void PublishDiagnostics(const char *message)
{
#ifdef FM_STYLE_ENABLE_LOGS
    FM_DEBUG_Print(message);
#else
    (void)message;
#endif
}

// --- Public API ---

void FM_STYLE_EXAMPLE_Init(fm_style_example_context_t *context, TX_MUTEX *lock)
{
    if ((context == NULL) || (lock == NULL)) {
        PublishDiagnostics("Init fail: invalid args");
        return;
    }

    s_context = context;
    s_lock = lock;

    tx_mutex_get(s_lock, TX_WAIT_FOREVER);
    ResetContext(s_context);
    tx_mutex_put(s_lock);

    s_shutdown_requested = FALSE;
    PublishDiagnostics("Init ok");
}

uint32_t FM_STYLE_EXAMPLE_ProcessSample(fm_style_example_context_t *context,
                                        uint32_t raw_count)
{
    uint32_t filtered = raw_count;

    if (context == NULL) {
        PublishDiagnostics("Process fail: null ctx");
        return 0u;
    }

    tx_mutex_get(s_lock, TX_WAIT_FOREVER);

    if (raw_count > FM_STYLE_EXAMPLE_MAX_BUFFER) {
        filtered = raw_count - FM_STYLE_EXAMPLE_SAFETY_MARGIN;
    }

    context->sample_total += filtered;
    context->buffer_count++;
    context->last_tick_ms += FM_STYLE_EXAMPLE_TIMER_MS;

    s_last_filtered = filtered;
    UpdateStateOnSample(filtered);

    tx_mutex_put(s_lock);

    return filtered;
}

void FM_STYLE_EXAMPLE_RequestShutdown(void)
{
    s_shutdown_requested = TRUE;
}

void FM_STYLE_EXAMPLE_OnTick(ULONG timer_key)
{
    (void)timer_key;

    if ((s_context == NULL) || (s_shutdown_requested == TRUE)) {
        return;
    }

    if (s_context->buffer_count == 0u) {
        PublishDiagnostics("Tick: no samples");
    }
}

// --- HAL / RTOS Callbacks ---

void HAL_GPIO_EXTI_Falling_Callback(uint16_t gpio_pin)
{
    (void)gpio_pin;

    if (s_context == NULL) {
        return;
    }

    UpdateStateOnSample(s_last_filtered);
}

/*** END OF FILE ***/
