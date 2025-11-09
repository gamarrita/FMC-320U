
/*
 * @file    fmx.h
 * @brief   Capa de integración con ThreadX (naming y API pública).
 * @date    2025-09-14
 *
 * @details
 *   - Prefijo del módulo: FMX_ para funciones públicas, fmx_ para tipos públicos.
 *   - Se respeta el naming original de ThreadX (TX_*), no se renombra.
 *   - Este header solo declara API pública y tipos propios de la capa FMX.
 */

#ifndef FMX_H_
#define FMX_H_

/* =========================== Includes ==================================== */
#include <stdint.h>
#include <stdbool.h>
#include "tx_api.h"  // ThreadX API (TX_THREAD, TX_MUTEX, etc.)

/* =========================== Public Defines ============================== */
#define FMX_DEFAULT_THREAD_STACK_BYTES   (2048u)
#define FMX_DEFAULT_THREAD_PRIO          (5u)
#define FMX_TICK_PER_SEC                 (1000u)

/* =========================== Public Types ================================ */
// Convención para nombres de control blocks de ThreadX (ejemplo):
//   TX_THREAD    fmx_tcb_xyz;   // thread control block
//   TX_MUTEX     fmx_mcb_xyz;   // mutex control block
//   TX_SEMAPHORE fmx_scb_xyz;   // semaphore control block
//   TX_EVENT_FLAGS_GROUP fmx_ecb_xyz; // event flags
//   TX_QUEUE     fmx_qcb_xyz;   // queue control block

typedef struct
{
    TX_THREAD  tcb_main;
    TX_MUTEX   mcb_main;
    TX_QUEUE   qcb_events;
} fmx_kernel_objs_t;

/* =========================== Public API ================================== */
/**
 * @brief  Inicializa el kernel de ThreadX y los objetos base (thread, mutex, queue).
 */
void FMX_InitKernel(fmx_kernel_objs_t *objs,
                    void (*entry_main)(ULONG),
                    void *stack_ptr,
                    ULONG stack_size,
                    UINT  priority);

/**
 * @brief  Entry de ejemplo para crear tareas adicionales.
 */
void FMX_StartBackground(void);

#endif /* FMX_H_ */

/*** end of file ***/
