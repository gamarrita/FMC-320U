/**
 * @file fm_style_example.h
 * @brief API de referencia que ejemplifica el estilo de nombres FM.
 *
 * Este header muestra como declarar macros, tipos y funciones siguiendo la
 * jerarquia de normas del proyecto. Usalo como guia al crear nuevos modulos.
 */

#ifndef FM_STYLE_EXAMPLE_H_
#define FM_STYLE_EXAMPLE_H_

#include <stdint.h>
#include "tx_api.h"

#ifdef __cplusplus
extern "C" {
#endif

// --- Public Constants ---

/** Tamano maximo del buffer de muestras. */
#define FM_STYLE_EXAMPLE_MAX_BUFFER   (64u)

/** Periodo default del timer de muestreo en milisegundos. */
#define FM_STYLE_EXAMPLE_TIMER_MS     (500u)

// --- Public Types ---

/**
 * Estados posibles del procesador de muestras de referencia.
 */
typedef enum {
    FM_STYLE_EXAMPLE_STATE_INIT = 0u,
    FM_STYLE_EXAMPLE_STATE_ACTIVE,
    FM_STYLE_EXAMPLE_STATE_IDLE,
    FM_STYLE_EXAMPLE_STATE_ERROR
} fm_style_example_state_t;

/**
 * Contexto de trabajo expuesto al exterior y compartido con otros modulos.
 */
typedef struct {
    fm_style_example_state_t state;
    uint32_t                 sample_total;
    uint16_t                 buffer_count;
    uint32_t                 last_tick_ms;
} fm_style_example_context_t;

// --- Public API ---

/**
 * Inicializa el modulo de ejemplo y lo deja listo para tomar muestras.
 * @param context Contexto persistente donde se almacenan los datos.
 * @param lock Mutex externo opcional para sincronizar acceso a recursos.
 */
void FM_STYLE_EXAMPLE_Init(fm_style_example_context_t *context, TX_MUTEX *lock);

/**
 * Procesa un nuevo valor y actualiza el acumulado.
 * @param context Contexto persistente.
 * @param raw_count Conteo crudo proveniente del sensor (unidades de pulsos).
 * @return Conteo filtrado aplicado a la logica actual.
 */
uint32_t FM_STYLE_EXAMPLE_ProcessSample(fm_style_example_context_t *context,
                                        uint32_t raw_count);

/**
 * Solicita el apagado ordenado del procesador.
 */
void FM_STYLE_EXAMPLE_RequestShutdown(void);

/**
 * Callback asociado al timer periodico de ThreadX.
 * @param timer_key Clave generada por el RTOS al activar el timer.
 */
void FM_STYLE_EXAMPLE_OnTick(ULONG timer_key);

#ifdef __cplusplus
}
#endif

#endif // FM_STYLE_EXAMPLE_H_
/*** END OF FILE ***/
