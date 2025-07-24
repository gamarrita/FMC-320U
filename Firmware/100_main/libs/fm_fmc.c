/*
 * @brief   Libreria desarrollada por FLOWMEET para sus FLOW METER COMPUTERS.
 *          Contiene definiciones, tipos de datos y funciones usadas en la implementacion de
 *          en un computador de caudales.
 *
 *
 * Version 1
 * Autor : Daniel H Sagarra
 * Fecha 10/11/2024
 * Modificaciones: version inicial
 *
 *
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
// Cantidad de segundos en otras unidades de tiempo
const uint32_t seconds_in[] =
{ 1, 60, 3600, 86400, };

/*
 * Esta lista de arreglos debe contener un elemento por cada elemento de la variable fm_fmc_vol_unit_t.
 */
fm_fmc_vol_data_t vol_unit_list[] =
{
{ .unit_convert = 1, .name = "00"
// Unidad adimensional.
        },
        { .unit_convert = 158.987304,  // Cantidad de litros en un barril.
                .name = "BL" },
        { .unit_convert = 3.78541, // Cantidad de litro en un galón.
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
 * La siguiente variable se guarda en la RAM de Backup, esta mantenida por la batería CR2032.
 * Al momento es la única variable de este tipo, de crear mas variables de este tipo se debera verificar
 * espacio disponible en RAM, se cuenta con solamente 2K disponibles en RAM_BACKUP_Section.
 */
fm_fmc_totalizer_t totalizer __attribute__((section(".RAM_BACKUP_Section")));

// Private function prototypes.

// Private function bodies.

// Public function bodies.

/*
 * @brief   inicializa a la variable global totalizer.
 * @note    en la FLASH del MCU se guardan valores de configuración que sirven para hacer testing.
 *          FM_FACTORY_SENSOR_0 configura al computador como si fuera un frecuencímetro. Los otros
 *          conjuntos de valores representan valores típicos de sensores, en sun factores de
 *          de calibración, y unidades de medida mas usadas, dependiendo del tamaño.
 * @param
 * @retval  ninguno.
 */
void FM_FMC_Init(sensors_list_t sensor)
{
    totalizer = FM_FACTORY_TotalizerGet(sensor);
    totalizer.factor_k = FM_FMC_FactorKCalc(totalizer.factor_cal, totalizer.vol_unit);
    totalizer.rate.factor_r = FM_FMC_FactorRateCalc(totalizer.factor_k, totalizer.time_unit);
}

/*
 * @brief   Calcula el valor actual del ACM y devuelve su valor.
 * @note
 * @param   Ninguno.
 * @retval  Volume actual en ACM
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

/*
 * @brief   Devuelve el ultimo valor calculado del ACM.
 * @note    No calcula el ACM actual, devuelve el calculado por FM_FMC_TtlCal();
 *          en su ultimo llamado.
 * @param   Ninguno.
 * @retval  Volume actual en ACM
 */
ufp3_t FM_FMC_AcmGet()
{
    return (totalizer.acm);
}

/*
 * @brief   Devuelve el ultimo valor calculado del ACM.
 * @note    No calcula el ACM actual, devuelve el calculado por FM_FMC_TtlCal();
 *          en su ultimo llamado.
 * @param   Ninguno.
 * @retval  Volume actual en ACM
 */
uint64_t FM_FMC_AcmGetPulse()
{
    return (totalizer.pulse_acm);
}

/*
 * @brief   Puesta a cero del volumen ACM.
 * @note
 * @param   Ninguno.
 * @retval  Ninguno.
 */
void FM_FMC_AcmReset()
{
    totalizer.acm = 0;
    totalizer.pulse_acm = 0;
}

/*
 * @brief   Agrega los últimos pulsos registrados.
 * @note
 * @param
 * @retval
 */
void FM_FMC_CaptureSet(uint16_t pulse, uint16_t time)
{
    totalizer.rate.delta_p = pulse;
    totalizer.rate.delta_t = time;
}

/*
 * @brief	retorna factor de calibración
 */
ufp3_t FM_FMC_FactorCalGet()
{
    return totalizer.factor_cal;
}

/*
 * @brief	modifica el factor de calibración en la variable de entorno totalizer.
 * @note
 * @retval	retorna 0, si no pudo escribir el valor
 * 			retorna 1, si pudo escribir el valor en la variable totalizer de entorno.
 *
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

/*
 * @brief   Calcula factor K
 * @note    El fator K se calcula a partir del factor de calibración y las unidades seleccionadas.
 * @param   Ninguno.
 * @retval  Factor K.
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

/*
 * @brief   Retorna factor K.
 * @note
 * @param   Ninguno.
 * @retval  Factor K.
 *
 */
double FM_FMC_FactorKGet()
{
    return totalizer.factor_k;
}

/*
 * @brief   Modifica el factor K en la variable de entorno totalizer.
 * @note
 * @retval  retorna 0, si no pudo escribir el valor
 *          retorna 1, si pudo escribir el valor en la variable totalizer de entorno.
 *
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

/*
 * @brief	Calcula el factor de caudal.
 * @note	Se calcula el coeficiente que permite convertir de frecuencia de entrada a caudal.
 * @param   Ninguno.
 * @retval  Factor de conversión de frecuencia a caudal.
 */
double FM_FMC_FactorRateCalc(double factor_k, fm_fmc_time_unit_t time_unit)
{
    double factor_r;

    factor_r = 32768.0;
    factor_r /= factor_k;
    factor_r *= seconds_in[time_unit];

    return factor_r;
}

/*
 * @brief   Calcula el factor de caudal.
 * @note    Se calcula el coeficiente que permite convertir de frecuencia de entrada a caudal.
 * @param   Ninguno.
 * @retval  Factor de conversión de frecuencia a caudal.
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

/*
 * @brief	retorna los valores actualmente usados.
 */
fm_fmc_totalizer_t FM_FMC_GetEnviroment(void)
{
    return totalizer;
}

/*
 * @brief   agrega los últimos pulsos registrados.
 * @note
 * @param
 * @retval
 */
void FM_FMC_PulseAdd(uint32_t pulse_delta)
{
    totalizer.pulse_acm += pulse_delta;
    totalizer.pulse_ttl += pulse_delta;
}

/*
 * @brief   Calcula el valor actual del TTL y devuelve su valor.
 * @note
 * @param   Ninguno.
 * @retval  Volume actual en TTL
 */
ufp3_t FM_FMC_RateCalc()
{
    double rate;

    rate = totalizer.rate.delta_p;
    rate /= totalizer.rate.delta_t;
    rate *= totalizer.rate.factor_r;

    // Convierto a punto fijo 3 decimales
    rate *= 1000;
    totalizer.rate.rate = (ufp3_t) rate;

    return (totalizer.rate.rate);
}

/*
 *	@brief	retorna la información de con cuantos decimales se esta mostrando el caudal.
 *	@retval	0, resolución en unidades.
 *			1, resolución en décimas.
 *			2, resolución en centécimas.
 *			3, resolución en milésimas.
 */
uint8_t FM_FMC_RateFpSelGet()
{
    return totalizer.rate.rate_pf_sel;
}

/*
 * @brief
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

/*
 * @brief
 */
ufp3_t FM_FMC_RateGet()
{
    return totalizer.rate.rate;
}

/*
 * @brief
 */
void FM_FMC_RateClear()
{
    totalizer.rate.rate = 0;
}

/*
 * @brief   Calcula el valor actual del TTL y devuelve su valor.
 * @note
 * @param   Ninguno.
 * @retval  Volume actual en TTL
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

/*
 * @brief   Devuelve el ultimo valor calculado del TTL.
 * @note    No calcula el TTL actual, devuelve el calculado por FM_FMC_TtlCal();
 *          en su ultimo llamado.
 * @param   Ninguno.
 * @retval  Volume actual en TTL
 */
ufp3_t FM_FMC_TtlGet()
{
    return (totalizer.ttl);
}

/*
 *
 */
uint64_t FM_FMC_TtlPulseGet()
{
    return totalizer.pulse_ttl;
}

/*
 *
 */
fm_fmc_vol_unit_t FM_FMC_TotalizerVolUnitGet()
{
    return (totalizer.vol_unit);
}

/*
 *
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

/*
 * @brief retorna la unidad de tiempo de la variable de entorno totalizer
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

/*
 * @brief   Puesta a cero del volumen ACM.
 * @note
 * @param   Ninguno.
 * @retval  Ninguno.
 */
void FM_FMC_TtlReset()
{
    totalizer.ttl = 0;
    totalizer.pulse_ttl = 0;
}

/*
 * @brief   Devuelve la posición del punto fijo para la medición de volumen.
 * @note    El usuario del computador de caudales puede elegir con cuantos puntos decimales se muestran los
 *          los valores del ACM y TTL. Esta funcion retorna esta informacion para que se puedan formatear
 *          correctamente los datos.
 * @param   Ninguno
 * @retval  0,1,2,3, son la cantidad decimales possibles.
 */
uint8_t FM_FMC_TotalizerFpSelGet()
{
    return totalizer.vol_pf_sel;
}

/*
 *
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

/*
 * @brief	devuelve un puntero al alias de la unidad como parámetro.
 * @note
 * @retval	como referencia, puntero a un alias en la lista de alias para la unidad de volumen.
 */
void FM_FMC_TotalizerStrUnitGet(char **string, fm_fmc_vol_unit_t vol_unit)
{
    *string = vol_unit_list[vol_unit].name;
}

/*
 * @brief   Selecciona que unidad de tiempo se esta mostrando
 * @note    Esta función tiene capacidad de parpadeo si se la llama continuamente.
 * @param
 * @retval
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
