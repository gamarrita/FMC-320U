/*
 * @file    fmx.c
 * @brief   Implementación de la capa FMX sobre ThreadX (creación de hilos/objs).
 * @date    2025-09-14
 */

#include "fmx.h"

/* =========================== Private Defines ============================= */
#define FMX_QUEUE_DEPTH            (16u)
#define FMX_QUEUE_ITEM_BYTES       (4u)

/* =========================== Private Types =============================== */
typedef struct
{
    ULONG items[FMX_QUEUE_DEPTH];
} fmx_queue_storage_t;

/* =========================== Private Data ================================ */
static fmx_kernel_objs_t     g_fmx_objs;
static fmx_queue_storage_t   g_q_storage;

/* Nombres simbólicos (si usás tx_object_name_set en Azure RTOS) */
static CHAR g_thread_name[]   = "FMX THREAD MAIN";
static CHAR g_mutex_name[]    = "FMX MUTEX MAIN";
static CHAR g_queue_name[]    = "FMX QUEUE EVENTS";

/* =========================== Private Prototypes ========================== */
static void fmx_set_object_names_(void);

/* =========================== Private Bodies ============================== */
static void fmx_set_object_names_(void)
{
#ifdef TX_ENABLE_EVENT_TRACE
    tx_thread_name_set(&g_fmx_objs.tcb_main,  (CHAR*)g_thread_name);
    tx_mutex_name_set (&g_fmx_objs.mcb_main,  (CHAR*)g_mutex_name);
    tx_queue_name_set (&g_fmx_objs.qcb_events,(CHAR*)g_queue_name);
#endif
}

/* =========================== Public Bodies =============================== */
void FMX_InitKernel(fmx_kernel_objs_t *objs,
                    void (*entry_main)(ULONG),
                    void *stack_ptr,
                    ULONG stack_size,
                    UINT  priority)
{
    // Guarda puntero de objetos del usuario o usa default global
    if (objs == NULL) { objs = &g_fmx_objs; }

    // Crea mutex
    tx_mutex_create(&objs->mcb_main, (CHAR*)g_mutex_name, TX_NO_INHERIT);

    // Crea cola (usa storage estático para ejemplo)
    tx_queue_create(&objs->qcb_events,
                    (CHAR*)g_queue_name,
                    1, // items de 1 ULONG
                    g_q_storage.items,
                    sizeof(g_q_storage.items));

    // Crea thread principal con parámetros del usuario
    tx_thread_create(&objs->tcb_main,
                     (CHAR*)g_thread_name,
                     entry_main,
                     0,                // thread_input
                     stack_ptr,
                     stack_size,
                     priority,
                     priority,
                     TX_NO_TIME_SLICE,
                     TX_AUTO_START);

    fmx_set_object_names_();
}

void FMX_StartBackground(void)
{
    // Aquí podrías crear otros hilos/temporizadores con la misma convención
}

/*** end of file ***/
