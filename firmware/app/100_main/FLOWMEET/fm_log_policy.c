#include "fm_log_policy.h"
#include <string.h> // For memset

// --- Estado interno ---
typedef struct {
    uint8_t   credits;               // 0..max_credits
    uint32_t  last_flow_unix;        // última vez con flujo (para recarga)
    uint32_t  last_pause_unix;       // última pausa (para herencia)
    uint32_t  last_heartbeat_unix;   // marca de heartbeat
    uint32_t  last_rate_milli;       // cache para variación en ON
    bool      boot_seen;             // primer ciclo tras boot
    bool      flow_active;           // sesión ON abierta
    uint8_t   to_on_step;            // 0: nada, 1: zero pendiente, 2: current pendiente
} fm_log_policy_state_t;

static fm_log_policy_state_t s_policy;
static fm_log_policy_cfg_t s_cfg;

// --- Defaults ---
#define _DEF(x, def) ((x) ? (x) : (def))

// --- API ---


/**
 * @brief Inicializa la política de logging de caudal.
 * @param boot_unix Timestamp de arranque (s).
 * @param reset_count Cantidad de resets desde el arranque.
 * @param cfg_opt Configuración opcional (puede ser NULL para usar defaults).
 */
void FM_LOG_POLICY_Init(uint32_t boot_unix, uint16_t reset_count, const fm_log_policy_cfg_t *cfg_opt) {
    memset(&s_policy, 0, sizeof(s_policy));
    s_cfg.max_credits    = cfg_opt ? cfg_opt->max_credits    : FM_LOG_POLICY_MAX_CREDITS;
    s_cfg.heartbeat_sec  = cfg_opt ? cfg_opt->heartbeat_sec  : FM_LOG_POLICY_HEARTBEAT_SEC;
    s_cfg.variation_ppm  = cfg_opt ? cfg_opt->variation_ppm  : FM_LOG_POLICY_VARIATION_PPM;
    s_policy.credits = s_cfg.max_credits;
    s_policy.last_heartbeat_unix = boot_unix;
    s_policy.last_flow_unix = boot_unix;
    s_policy.last_pause_unix = boot_unix;
    s_policy.last_rate_milli = 0;
    s_policy.boot_seen = false;
    s_policy.flow_active = false;
    s_policy.to_on_step = 0;
    (void)reset_count; // Informativo, no usado internamente
}

static void policy_update_credits_(const fm_log_policy_inputs_t *in) {
    // Recarga en ON
    if (in->rate_state == FM_FMC_RATE_ON) {
        uint32_t mins = (in->now_unix - s_policy.last_flow_unix) / 60;
        if (mins > 0) {
            uint8_t add = (uint8_t)mins;
            if (s_policy.credits < s_cfg.max_credits) {
                uint8_t new_credits = s_policy.credits + add;
                s_policy.credits = (new_credits > s_cfg.max_credits) ? s_cfg.max_credits : new_credits;
            }
            s_policy.last_flow_unix += mins * 60;
        }
    }
    // Herencia en OFF
    else if (in->rate_state == FM_FMC_RATE_OFF) {
        uint32_t mins = (in->now_unix - s_policy.last_pause_unix) / 60;
        if (mins > 0) {
            uint8_t add = (uint8_t)mins;
            if (s_policy.credits < s_cfg.max_credits) {
                uint8_t new_credits = s_policy.credits + add;
                s_policy.credits = (new_credits > s_cfg.max_credits) ? s_cfg.max_credits : new_credits;
            }
            s_policy.last_pause_unix += mins * 60;
        }
    }
}

static bool policy_should_record_variation_(uint32_t prev, uint32_t now, uint32_t ppm) {
    uint32_t maxv = (prev > now) ? prev : now;
    if (maxv == 0) maxv = 1;
    uint32_t diff = (prev > now) ? (prev - now) : (now - prev);
    uint32_t rel_ppm = (diff * 1000000u) / maxv;
    return rel_ppm >= ppm;
}

/**
 * @brief Determina si la variación de caudal justifica un registro.
 * @param rate_prev_milli Caudal anterior (milésimas).
 * @param rate_now_milli Caudal actual (milésimas).
 * @param variation_ppm Umbral de variación (ppm).
 * @return true si la variación supera el umbral.
 */
bool FM_LOG_POLICY_ShouldRecordVariation(uint32_t rate_prev_milli, uint32_t rate_now_milli, uint32_t variation_ppm) {
    return policy_should_record_variation_(rate_prev_milli, rate_now_milli, variation_ppm);
}

/**
 * @brief Obtiene la cantidad actual de créditos disponibles.
 * @return Créditos disponibles.
 */
uint8_t FM_LOG_POLICY_GetCredits(void) {
    return s_policy.credits;
}

/**
 * @brief Obtiene el timestamp del último heartbeat registrado.
 * @return Timestamp (s).
 */
uint32_t FM_LOG_POLICY_GetLastHeartbeat(void) {
    return s_policy.last_heartbeat_unix;
}

/**
 * @brief Fuerza el tick de heartbeat para forzar un registro inmediato.
 * @param now_unix Timestamp actual (s).
 */
void FM_LOG_POLICY_ForceHeartbeatTick(uint32_t now_unix) {
    s_policy.last_heartbeat_unix = now_unix - s_cfg.heartbeat_sec;
}

/**
 * @brief Evalúa la política y determina si se debe registrar un evento.
 * @param in Puntero a la estructura de entrada con el estado actual.
 * @return Acción a realizar (ver fm_log_action_t).
 */
fm_log_action_t FM_LOG_POLICY_Step(const fm_log_policy_inputs_t *in) {
    // Recarga créditos si corresponde
    policy_update_credits_(in);

    // Heartbeat (baja prioridad)
    bool heartbeat_due = (in->now_unix - s_policy.last_heartbeat_unix) >= s_cfg.heartbeat_sec;

    // --- Prioridad: TO_ON (doble evento) ---
    if (in->rate_state == FM_FMC_RATE_STARTED) {
        if (s_policy.to_on_step == 0) {
            s_policy.to_on_step = 1;
            s_policy.flow_active = true;
            s_policy.last_rate_milli = 0;
            return FM_LOG_ACT_PUSH_ZERO_T_MINUS_1S;
        } else if (s_policy.to_on_step == 1) {
            s_policy.to_on_step = 2;
            s_policy.last_rate_milli = in->rate_milli;
            // Consume crédito si hay, pero emite aunque no haya
            if (s_policy.credits > 0) s_policy.credits--;
            return FM_LOG_ACT_PUSH_CURRENT;
        } else {
            s_policy.to_on_step = 0;
        }
    } else {
        s_policy.to_on_step = 0;
    }

    // --- ON: variación significativa ---
    if (in->rate_state == FM_FMC_RATE_ON && s_policy.flow_active) {
        if (policy_should_record_variation_(s_policy.last_rate_milli, in->rate_milli, s_cfg.variation_ppm)) {
            if (s_policy.credits > 0) {
                s_policy.credits--;
                s_policy.last_rate_milli = in->rate_milli;
                return FM_LOG_ACT_PUSH_CURRENT;
            }
        }
    }

    // --- TO_OFF: STOP ---
    if (in->rate_state == FM_FMC_RATE_STOPPED && s_policy.flow_active) {
        s_policy.flow_active = false;
        s_policy.last_pause_unix = in->now_unix;
        return FM_LOG_ACT_PUSH_STOP;
    }

    // --- Heartbeat ---
    if (heartbeat_due) {
        s_policy.last_heartbeat_unix = in->now_unix;
        return FM_LOG_ACT_PUSH_HEARTBEAT;
    }

    return FM_LOG_ACT_NONE;
}
