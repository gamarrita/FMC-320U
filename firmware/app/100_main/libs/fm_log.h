/**
 * @file fm_log.h
 * @brief Flow event and diagnostic logger for the FMC-320U.
 */

#ifndef FM_LOG_H_
#define FM_LOG_H_

#include "fm_fmc.h"
#include "fmx.h"

typedef enum {
    FM_LOG_POWER_OFF,
    FM_LOG_POWER_ON,
    FM_LOG_RATE_TO_ON,
    FM_LOG_RATE_TO_OFF,
    FM_LOG_RATE_HIGH_ALARM,
    FM_LOG_RATE_LOW_ALARM,
} fm_log_event_t;

typedef struct {
    uint64_t       ttl_pulses;
    uint64_t       acm_pulses;
    uint32_t       factor_cal;
    uint32_t       seconds;
    uint16_t       temp_rtd;
    uint16_t       temp_int;
    fm_log_event_t event;
    uint8_t        reserved_1;
    uint16_t       reserved_2;
} fm_log_data_t;

void FM_LOG_Monitor(fmx_rate_status_t mode);
fmx_status_t FM_LOG_ReadLog(uint32_t data_index, uint8_t *data_ptr);

#endif // FM_LOG_H_

