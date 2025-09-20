/**
 * @file fm_fmc.c
 * @brief Totalizer and rate bookkeeping for the FMC-320U flow computer.
 *
 * The module stores runtime counters in backup SRAM and provides helper
 * routines to compute accumulated volume, trip totals, and instantaneous
 * flow.
 */

// Includes.
#include "fm_fmc.h"
#include "fm_factory.h"
#include "fm_lcd.h"
#include "fm_debug.h"
#include "fmx.h"
// Typedef.

/*
 * The names of all new data types, including structures, unions, and
 * enumerations, shall consist only of lowercase characters and internal
 * underscores and end with ‘_t’.
 *
 * All new structures, unions, and enumerations shall be named via a typedef.
 *
 */

// Const data.
// Seconds per supported time unit
const uint32_t seconds_in[] =
{ 1, 60, 3600, 86400, };

/*
 * Each entry must match a value in fm_fmc_vol_unit_t.
 */
fm_fmc_vol_data_t vol_unit_list[] =
{
{ .unit_convert = 1, .name = "00"
// Dimensionless placeholder.
        },
        { .unit_convert = 158.987304,  // Liters per barrel.
                .name = "BL" },
        { .unit_convert = 3.78541, // Liters per US gallon.
                .name = "GL", },
        { .unit_convert = 1, .name = "KG", },
        { .unit_convert = 1, .name = "LT", },
        { .unit_convert = 1000, .name = "M3", },
        { .unit_convert = 1, .name = "ME", }, };

//Debug.

// Project variables, non-static, at least used in other file.

// External variables.

// Global variables, statics.

/*
 * Runtime environment persisted in backup SRAM (powered from VBAT).
 * Ensure additional backup variables fit within the 2 KB .RAM_BACKUP_Section.
 */
fm_fmc_totalizer_t totalizer __attribute__((section(".RAM_BACKUP_Section")));

// Private function prototypes.

// Private function bodies.

// Public function bodies.

/**
 * @brief Initializes the global totalizer state.
 * @param sensor Factory preset to load.
 * @note  Also recomputes K and rate factors based on the loaded configuration.
 */
void FM_FMC_Init(sensors_list_t sensor)
{
    totalizer = FM_FACTORY_TotalizerGet(sensor);
    totalizer.factor_k = FM_FMC_FactorKCalc(totalizer.factor_cal, totalizer.vol_unit);
    totalizer.rate.factor_r = FM_FMC_FactorRateCalc(totalizer.factor_k, totalizer.time_unit);
}

/**
 * @brief Computes the accumulated volume (ACM) from pulse counters.
 * @return Fixed-point volume (x1000).
 */
ufp3_t FM_FMC_AcmCalc()
{
    double acm;

    acm = totalizer.pulse_acm;
    acm /= totalizer.factor_k;
    acm *= 1000;
    totalizer.acm = (ufp3_t) acm;

    return totalizer.acm;
}

/**
 * @brief Returns the cached ACM volume.
 */
ufp3_t FM_FMC_AcmGet()
{
    return (totalizer.acm);
}

/**
 * @brief Returns the pulse accumulator backing ACM.
 */
uint64_t FM_FMC_AcmGetPulse()
{
    return (totalizer.pulse_acm);
}

/**
 * @brief Clears the ACM accumulator and associated pulse counter.
 */
void FM_FMC_AcmReset()
{
    totalizer.acm = 0;
    totalizer.pulse_acm = 0;
}

/**
 * @brief Stores the latest pulse delta and time delta for rate calculations.
 */
void FM_FMC_CaptureSet(uint16_t pulse, uint16_t time)
{
    totalizer.rate.delta_p = pulse;
    totalizer.rate.delta_t = time;
}

/**
 * @brief Returns the calibration factor currently in use.
 */
ufp3_t FM_FMC_FactorCalGet()
{
    return totalizer.factor_cal;
}

/**
 * @brief Updates the calibration factor when the value is within bounds.
 * @param factor_cal New calibration factor (pulses per liter x1000).
 * @return 1 if the factor was accepted, 0 otherwise.
 */
uint32_t FM_FMC_FactorCalSet(ufp3_t factor_cal)
{
    // Esta implementación es provisoria, hay que hacer chequeo de contorno antes de modificar totalizer.

    if ((factor_cal >= FM_FMC_FACTOR_CAL_MIN) && (factor_cal <= FM_FMC_FACTOR_CAL_MAX))
    {
        totalizer.factor_cal = factor_cal;
    }
    else
    {
        return 0;
    }
    return 1;
}

/**
 * @brief Computes the K factor from the calibration factor and volume unit.
 * @param factor_cal Calibration factor (pulses per liter x1000).
 * @param unit       Active volume unit.
 * @return K factor expressed in the selected unit.
 */
double FM_FMC_FactorKCalc(ufp3_t factor_cal, fm_fmc_vol_unit_t unit)
{
    double factor_k;

    // Factor de conversion de unidad calibración, litro, a unidad de visualización.
    factor_k = factor_cal;

    // El factor K es el de calibración, en litros, convertido a la unidad de volumen seleccionada.
    factor_k *= vol_unit_list[unit].unit_convert;

    factor_k /= 1000;  // Se necesita para convertir de ufp_t a double.

    return factor_k;
}

/**
 * @brief Returns the current K factor stored in the environment.
 */
double FM_FMC_FactorKGet()
{
    return totalizer.factor_k;
}

/**
 * @brief Updates the K factor when the provided value is within limits.
 * @param factor_k New K factor expressed in fixed-point form.
 * @return 1 if the value was accepted, 0 otherwise.
 */
uint32_t FM_FMC_FactorKSet(ufp3_t factor_k)
{
    // Esta implementación es provisoria, hay que hacer chequeo de contorno antes de modificar totalizer.

    if ((factor_k > FM_FMC_FACTOR_CAL_MIN) && (factor_k < FM_FMC_FACTOR_CAL_MAX))
    {
        totalizer.factor_k = factor_k;
    }
    else
    {
        return 0;
    }
    return 1;
}

/**
 * @brief Converts a pulse frequency into volumetric rate units.
 * @param factor_k   K factor expressed in the active unit.
 * @param time_unit  Time base used for the rate.
 * @return Conversion factor from pulses/s to volume/time.
 */
double FM_FMC_FactorRateCalc(double factor_k, fm_fmc_time_unit_t time_unit)
{
    double factor_r;

    factor_r = 32768.0;
    factor_r /= factor_k;
    factor_r *= seconds_in[time_unit];

    return factor_r;
}

/**
 * @brief Stores a precomputed rate conversion factor.
 * @param factor_rate Conversion factor from pulses to volume/time.
 * @return FMX_STATUS_OK if the value is positive, FMX_STATUS_ERROR otherwise.
 */
fmx_status_t FM_FMC_FactorRateSet(double factor_rate)
{
    fmx_status_t status = FMX_STATUS_OK;

    if (factor_rate > 0)
    {
        totalizer.rate.factor_r = factor_rate;
    }
    else
    {
        totalizer.rate.factor_r = 1;
        status = FMX_STATUS_ERROR;
    }

    return status;
}

/**
 * @brief Returns a copy of the current totalizer environment.
 */
fm_fmc_totalizer_t FM_FMC_GetEnviroment(void)
{
    return totalizer;
}

/**
 * @brief Adds a pulse delta to both ACM and TTL accumulators.
 */
void FM_FMC_PulseAdd(uint32_t pulse_delta)
{
    totalizer.pulse_acm += pulse_delta;
    totalizer.pulse_ttl += pulse_delta;
}

/**
 * @brief Computes the instantaneous rate using the latest capture.
 * @return Fixed-point rate (x1000).
 */
ufp3_t FM_FMC_RateCalc()
{
    double rate;

    rate = totalizer.rate.delta_p;
    rate /= (totalizer.rate.delta_t-1);
    rate *= totalizer.rate.factor_r;

    // Convierto a punto fijo 3 decimales
    rate *= 1000;
    totalizer.rate.rate = (ufp3_t) rate;

    return (totalizer.rate.rate);
}

/**
 * @brief Returns the decimal position used to render the rate.
 */
uint8_t FM_FMC_RateFpSelGet()
{
    return totalizer.rate.rate_pf_sel;
}

/**
 * @brief Cycles through the available decimal positions for the rate display.
 */
void FM_FMC_RateFpInc()
{
    if (totalizer.rate.rate_pf_sel < FM_FMC_FP_SEL_3)
    {
        totalizer.rate.rate_pf_sel++;
    }
    else
    {
        totalizer.rate.rate_pf_sel = FM_FMC_FP_SEL_0;
    }
}

/**
 * @brief Returns the cached instantaneous rate.
 */
ufp3_t FM_FMC_RateGet()
{
    return totalizer.rate.rate;
}

/**
 * @brief Clears the instantaneous rate cache.
 */
void FM_FMC_RateClear()
{
    totalizer.rate.rate = 0;
}

/**
 * @brief Computes the current trip total (TTL) from pulse counters.
 * @return Fixed-point volume (x1000).
 */
ufp3_t FM_FMC_TtlCalc()
{
    double ttl = 0;

    ttl = totalizer.pulse_ttl;
    ttl /= totalizer.factor_k;

    // Paso a punto fijo tres decimales.
    ttl *= 1000;
    totalizer.ttl = (ufp3_t) ttl;

    return (totalizer.ttl);
}

/**
 * @brief Returns the cached trip total (TTL).
 */
ufp3_t FM_FMC_TtlGet()
{
    return (totalizer.ttl);
}

/**
 * @brief Returns the raw pulse accumulator used for TTL.
 */
uint64_t FM_FMC_TtlPulseGet()
{
    return totalizer.pulse_ttl;
}

/**
 * @brief Returns the current volume unit configured for the totalizer.
 */
fm_fmc_vol_unit_t FM_FMC_TotalizerVolUnitGet()
{
    return (totalizer.vol_unit);
}

/**
 * @brief Updates the volume unit if the requested value is valid.
 * @param vol_unit Volume unit to apply.
 * @return 1 if the unit was accepted, 0 otherwise.
 */
uint32_t FM_FMC_TotalizerVolUnitSet(fm_fmc_vol_unit_t vol_unit)
{
    const char debug_error[] = "FM_FMC_TotalizerVolUnitSet ERROR";

    if ((vol_unit >= VOL_UNIT_BLANK) && (vol_unit < VOL_UNIT_END))
    {
        totalizer.vol_unit = vol_unit;
    }
    else
    {
        FM_DEBUG_LedError(1);
        FM_DEBUG_UartMsg(debug_error, sizeof(debug_error));
        return 0;
    }
    return 1;
}

/**
 * @brief Returns the current time unit used by the totalizer.
 */
fm_fmc_time_unit_t FM_FMC_TotalizerTimeUnitGet()
{
    return (totalizer.time_unit);
}

/*
 *
 */
fmx_status_t FM_FMC_TotalizerTimeUnitSet(fm_fmc_time_unit_t time_unit)
{
    const char debug_error[] = "FM_FMC_TotalizerTimeUnitSet ERROR";

    if ((time_unit >= TIME_UNIT_SECOND) && (time_unit < TIME_UNIT_END))
    {
        totalizer.time_unit = time_unit;
    }
    else
    {
        FM_DEBUG_LedError(1);
        FM_DEBUG_UartMsg(debug_error, sizeof(debug_error));
        return FMX_STATUS_ERROR;
    }
    return FMX_STATUS_OK;
}

/**
 * @brief Clears the trip total (TTL) accumulator and pulse counter.
 */
void FM_FMC_TtlReset()
{
    totalizer.ttl = 0;
    totalizer.pulse_ttl = 0;
}

/**
 * @brief Returns the decimal position used to display ACM/TTL.
 */
uint8_t FM_FMC_TotalizerFpSelGet()
{
    return totalizer.vol_pf_sel;
}

/**
 * @brief Cycles through the decimal positions for ACM/TTL display.
 */
void FM_FMC_TotalizerFpInc()
{
    if (totalizer.vol_pf_sel < FM_FMC_FP_SEL_3)
    {
        totalizer.vol_pf_sel++;
    }
    else
    {
        totalizer.vol_pf_sel = FM_FMC_FP_SEL_0;
    }
}

/**
 * @brief Returns the short label for a given volume unit.
 * @param string Output pointer where the label is stored.
 * @param vol_unit Unit whose label is requested.
 */
void FM_FMC_TotalizerStrUnitGet(char **string, fm_fmc_vol_unit_t vol_unit)
{
    *string = vol_unit_list[vol_unit].name;
}

/**
 * @brief Highlights the active time unit symbol on the LCD.
 * @param sel Time unit to toggle.
 */
void FM_FMC_TotalizerTimeUnitSel(fm_fmc_time_unit_t sel)
{
    static fm_fmc_time_unit_t sel_old = -1;

    // Se borran
    if (sel != sel_old)
    {
        FM_LCD_LL_SymbolWrite(FM_LCD_LL_SYM_S, 0);
        FM_LCD_LL_SymbolWrite(FM_LCD_LL_SYM_M, 0);
        FM_LCD_LL_SymbolWrite(FM_LCD_LL_SYM_H, 0);
        FM_LCD_LL_SymbolWrite(FM_LCD_LL_SYM_D, 0);
    }

    sel_old = sel;

    switch (sel)
    {
    case TIME_UNIT_SECOND:
        FM_LCD_LL_SymbolWrite(FM_LCD_LL_SYM_S, 1);
        break;
    case TIME_UNIT_MINUTE:
        FM_LCD_LL_SymbolWrite(FM_LCD_LL_SYM_M, 1);
        break;
    case TIME_UNIT_HOUR:
        FM_LCD_LL_SymbolWrite(FM_LCD_LL_SYM_H, 1);
        break;
    case TIME_UNIT_DAY:
        FM_LCD_LL_SymbolWrite(FM_LCD_LL_SYM_D, 1);
        break;
    default:
        break;
    }
}

uint16_t FM_FMC_TicketNumberGet()
{
    totalizer.ticket_number++;
    return totalizer.ticket_number;
}

// Interrupts

/*** end of file ***/











