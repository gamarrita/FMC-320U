/**
 * @file fm_fmc.h
 * @brief Flow-meter computation helpers: totalizer bookkeeping and rate math.
 */

#ifndef FM_FMC_H_
#define FM_FMC_H_

#include "fm_lcd_ll.h"
#include "fmx.h"
#include <stdint.h>

/** Fixed-point format used across the module (value x 1000). */
typedef uint32_t ufp3_t;

/** Data source used when loading factory defaults. */
typedef enum {
    FM_FACTORY_RAM_BACKUP,
    FM_FACTORY_LAST_SETUP,
    FM_FACTORY_SENSOR_0,
    FM_FACTORY_AI_25,
    FM_FACTORY_AI_80,
} sensors_list_t;

/** Decimal point selection for rate/volume rendering. */
typedef enum {
    FM_FMC_FP_SEL_0 = 0,
    FM_FMC_FP_SEL_1,
    FM_FMC_FP_SEL_2,
    FM_FMC_FP_SEL_3,
} fm_fmc_fp_sel;

/** Volume units accepted by the totalizer. */
typedef enum {
    VOL_UNIT_BLANK = 0, ///< Dimensionless placeholder (must remain first).
    VOL_UNIT_BR,        ///< US oil barrel.
    VOL_UNIT_GL,        ///< US gallon.
    VOL_UNIT_KG,        ///< Kilogram.
    VOL_UNIT_LT,        ///< Liter (primary calibration unit).
    VOL_UNIT_M3,        ///< Cubic meter.
    VOL_UNIT_ME,        ///< Equivalent cubic meter (e.g. gas equivalent).
    VOL_UNIT_END,
} fm_fmc_vol_unit_t;

/** Metadata for each supported volume unit. */
typedef struct {
    float unit_convert; ///< Conversion factor relative to liters.
    char  name[3];      ///< Short label displayed on the LCD.
} fm_fmc_vol_data_t;

/** Time bases used by rate/totalizer calculations. */
typedef enum {
    TIME_UNIT_SECOND = 0,
    TIME_UNIT_MINUTE,
    TIME_UNIT_HOUR,
    TIME_UNIT_DAY,
    TIME_UNIT_END,
} fm_fmc_time_unit_t;

/** Runtime data associated with instantaneous flow rate. */
typedef struct {
    double  factor_r;     ///< Rate factor (includes K factor and time unit).
    ufp3_t  delta_t;      ///< Elapsed time in milliseconds (x1000).
    ufp3_t  delta_p;      ///< Pulses accumulated during delta_t.
    ufp3_t  rate;         ///< Cached rate using fixed-point notation.
    uint8_t rate_pf_sel;  ///< Decimal point for rendered rate.
    ufp3_t  limit_high;   ///< Nominal upper rate limit.
    ufp3_t  limit_low;    ///< Nominal lower rate limit.
    uint32_t filter;      ///< Additional filter configuration.
} fm_fmc_rate_t;

/** Aggregated counters and configuration for the totalizer. */
typedef struct {
    ufp3_t            acm;          ///< Accumulated volume (x1000).
    ufp3_t            ttl;          ///< Trip volume (x1000).
    uint8_t           vol_pf_sel;   ///< Decimal point for ACM/TTL display.
    uint64_t          pulse_acm;    ///< Pulse accumulator for ACM.
    uint64_t          pulse_ttl;    ///< Pulse accumulator for TTL.
    ufp3_t            factor_cal;   ///< Calibration factor (pulses per liter).
    double            factor_k;     ///< K factor derived from calibration and units.
    fm_fmc_vol_unit_t vol_unit;     ///< Active volume unit.
    fm_fmc_time_unit_t time_unit;   ///< Active time base.
    fm_fmc_rate_t     rate;         ///< Instantaneous rate tracking.
    uint16_t          ticket_number;///< Sequential ticket number for reports.
} fm_fmc_totalizer_t;

#define FM_FMC_FACTOR_CAL_MAX 99999999u
#define FM_FMC_FACTOR_CAL_MIN 1000u

// --- API ---

ufp3_t      FM_FMC_AcmCalc(void);
ufp3_t      FM_FMC_AcmGet(void);
uint64_t    FM_FMC_AcmGetPulse(void);
void        FM_FMC_AcmReset(void);

ufp3_t      FM_FMC_FactorCalGet(void);
uint32_t    FM_FMC_FactorCalSet(ufp3_t factor_cal);
double      FM_FMC_FactorKCalc(ufp3_t factor_k, fm_fmc_vol_unit_t unit);
double      FM_FMC_FactorKGet(void);
uint32_t    FM_FMC_FactorKSet(ufp3_t factor_k);
double      FM_FMC_FactorRateCalc(double factor_k, fm_fmc_time_unit_t unit);
fmx_status_t FM_FMC_FactorRateSet(double factor_rate);

fm_fmc_totalizer_t FM_FMC_GetEnviroment(void);
void              FM_FMC_Init(sensors_list_t sensor);

void     FM_FMC_CaptureSet(uint16_t pulse, uint16_t time);
ufp3_t   FM_FMC_RateCalc(void);
void     FM_FMC_RateClear(void);
ufp3_t   FM_FMC_RateGet(void);
uint8_t  FM_FMC_RateFpSelGet(void);
void     FM_FMC_RateFpInc(void);

uint8_t          FM_FMC_TotalizerFpSelGet(void);
void             FM_FMC_TotalizerFpInc(void);
void             FM_FMC_TotalizerStrUnitGet(char **string, fm_fmc_vol_unit_t unit);
void             FM_FMC_TotalizerTimeUnitSel(fm_fmc_time_unit_t sel);
fm_fmc_time_unit_t FM_FMC_TotalizerTimeUnitGet(void);
fmx_status_t      FM_FMC_TotalizerTimeUnitSet(fm_fmc_time_unit_t time_unit);
fm_fmc_vol_unit_t FM_FMC_TotalizerVolUnitGet(void);
uint32_t          FM_FMC_TotalizerVolUnitSet(fm_fmc_vol_unit_t unit);
uint16_t          FM_FMC_TicketNumberGet(void);

ufp3_t   FM_FMC_TtlCalc(void);
ufp3_t   FM_FMC_TtlGet(void);
uint64_t FM_FMC_TtlPulseGet(void);
void     FM_FMC_TtlReset(void);

void     FM_FMC_PulseAdd(uint32_t pulse_delta);

#endif // FM_FMC_H_

