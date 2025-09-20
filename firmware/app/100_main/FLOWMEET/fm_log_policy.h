#ifndef FM_LOG_POLICY_H
#define FM_LOG_POLICY_H

#include <stdint.h>
#include <stdbool.h>
#include "../FLOWMEET/fmx.h"

// --- Macros por defecto ---
#define FM_LOG_POLICY_MAX_CREDITS      (10u)
#define FM_LOG_POLICY_HEARTBEAT_SEC    (60u * 60u)   // 3600 s
#define FM_LOG_POLICY_VARIATION_PPM    (20000u)      // 2 %

// --- Tipos públicos ---

// Estructura con los datos de entrada para la política de logging de caudal.
typedef struct {
    uint32_t          now_unix;     // timestamp actual (s)
    fmx_rate_status_t rate_state;   // OFF, TO_ON, ON, TO_OFF
    uint32_t          rate_milli;   // caudal * 1000 (opcional para variación)
} fm_log_policy_inputs_t;


typedef enum {
    FM_LOG_ACT_NONE = 0,
    FM_LOG_ACT_PUSH_ZERO_T_MINUS_1S, // punto “0” en t-1s
    FM_LOG_ACT_PUSH_CURRENT,         // punto actual
    FM_LOG_ACT_PUSH_STOP,            // evento de parada
    FM_LOG_ACT_PUSH_HEARTBEAT        // marca de vida
} fm_log_action_t;

typedef struct {
    uint8_t   max_credits;     // default: 10
    uint32_t  heartbeat_sec;   // default: 3600 (60 min)
    uint32_t  variation_ppm;   // default: 20000 (2 %)
} fm_log_policy_cfg_t;

// --- API pública ---

void     FM_LOG_POLICY_Init(uint32_t boot_unix, uint16_t reset_count, const fm_log_policy_cfg_t *cfg_opt);
fm_log_action_t FM_LOG_POLICY_Step(const fm_log_policy_inputs_t *in);
bool     FM_LOG_POLICY_ShouldRecordVariation(uint32_t rate_prev_milli, uint32_t rate_now_milli, uint32_t variation_ppm);
uint8_t  FM_LOG_POLICY_GetCredits(void);
uint32_t FM_LOG_POLICY_GetLastHeartbeat(void);
void     FM_LOG_POLICY_ForceHeartbeatTick(uint32_t now_unix);

#endif // FM_LOG_POLICY_H
