#include "fm_log_policy.h"
#include <stdio.h>

// Simulación de estados del caudalímetro
const char *rate_state_str[] = {"OFF", "TO_ON", "ON", "TO_OFF"};
const char *action_str[] = {
    "NONE", "PUSH_ZERO_T_MINUS_1S", "PUSH_CURRENT", "PUSH_STOP", "PUSH_HEARTBEAT"
};

int main(void) {
    // Configuración opcional (puede ser NULL)
    fm_log_policy_cfg_t cfg = {
        .max_credits = 5,
        .heartbeat_sec = 10, // Heartbeat cada 10s para demo
        .variation_ppm = 10000 // 1%
    };

    // Inicializar la política
    FM_LOG_POLICY_Init(1000, 0, &cfg);

    // Simular entradas
    fm_log_policy_inputs_t entradas[] = {
        {1001, FMX_RATE_TO_ON, 0},      // Arranque de flujo
        {1002, FMX_RATE_TO_ON, 1000},  // Confirmación de flujo
        {1003, FMX_RATE_ON, 1100},     // Variación pequeña
        {1004, FMX_RATE_ON, 1300},     // Variación significativa
        {1015, FMX_RATE_ON, 1300},     // Heartbeat (por tiempo)
        {1020, FMX_RATE_TO_OFF, 0},    // Parada de flujo
        {1030, FMX_RATE_OFF, 0},       // Sin flujo
    };

    size_t n = sizeof(entradas)/sizeof(entradas[0]);
    for (size_t i = 0; i < n; ++i) {
        fm_log_action_t act = FM_LOG_POLICY_Step(&entradas[i]);
        printf("t=%u, state=%s, rate=%u -> action=%s, credits=%u\n",
            entradas[i].now_unix,
            rate_state_str[entradas[i].rate_state],
            entradas[i].rate_milli,
            action_str[act],
            FM_LOG_POLICY_GetCredits()
        );
    }

    return 0;
}
