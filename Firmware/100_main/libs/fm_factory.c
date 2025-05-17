/*
 * @brief Este modulo contiene:
 * 		- Valores de configuración para sensores primarios muy usados. Una funcion para recuperarlos.
 * 		- Version del firmware
 *
 * Version 1
 * Autor: Daniel H Sagarra
 * Fecha: 10/11/2024
 * Modificaciones: version inicial.
 *
 */

// Includes.
#include "fm_factory.h"
#include "fm_fmc.h"

// Typedef.

// Const data.
const char msg_version_firmware[] = "01.01.009"; // Version.Revision.Release
const char msg_release[] = "B1"; // Version.Revision.Release

/*
 *  Esta constante se usa en dos situaciones:
 *  - La primera vez que se ejecuta el código, luego de ser programada la flash.
 *  - Si desde la configuración se quieren recuperar los valores de fabrica.
 *
 *  Si el micro controlador perdiera su alimentación, el valor que es el que recupera,
 *  se usara otro valor en flash, donde se guardo la ultima configuración del equipo
 */
const fm_fmc_totalizer_t sensor_0 =
{
    .pulse_acm = 0,
    .pulse_ttl = 0,
    .vol_pf_sel = 0,
    .factor_k = 1.000,
    .factor_cal = 1000,
    .vol_unit = VOL_UNIT_LT,
    .time_unit = TIME_UNIT_SECOND,
    .rate.factor_r = 1000,
    .rate.rate_pf_sel = 0,
    .rate.limit_high = 1500000, // frecuencia máxima 1500Hz
    .rate.limit_low = 250, // frecuencia minima 0,1 Hz
    .rate.delta_t = 1000, // filtro un segundo
    .rate.filter = 1, // filtro un segundo
    .ticket_number = 0,
    };

// Valor típicos para un sensor Serie AI DN25, turbina axial diámetro interno 1" paso total.
const fm_fmc_totalizer_t sensor_ai_25 =
{
    .pulse_acm = 0,
    .pulse_ttl = 0,
    .vol_pf_sel = 3,
    .factor_k = 123.456, // factor 123.456
    .factor_cal = 123456,   // Hay
    .vol_unit = VOL_UNIT_LT,
    .time_unit = TIME_UNIT_SECOND,
    .rate.factor_r = 1000,
    .rate.rate_pf_sel = 3,
    .rate.limit_high = 1500000, // frecuencia máxima 1500Hz
    .rate.limit_low = 250, // frecuencia minima 0,1 Hz
    .rate.delta_t = 1000, // filtro un segundo
    .rate.filter = 1, // filtro un segundo
    .ticket_number = 0,
    };

// Valor típicos para un sensor Serie AI DN80, turbina axial diámetro interno 3" paso total.
const fm_fmc_totalizer_t sensor_ai_80 =
{
    .pulse_acm = 0,
    .pulse_ttl = 0,
    .vol_pf_sel = 3,
    .factor_k = 23408, // factor 123.456
    .factor_cal = 1000,   // Hay
    .vol_unit = VOL_UNIT_M3,
    .time_unit = TIME_UNIT_SECOND,
    .rate.factor_r = 1000,
    .rate.rate_pf_sel = 3,
    .rate.limit_high = 1500000, // frecuencia máxima 1500Hz
    .rate.limit_low = 250, // frecuencia minima 0,1 Hz
    .rate.delta_t = 1000, // filtro un segundo
    .rate.filter = 1, // filtro un segundo
    .ticket_number = 0,
    };

// Defines.

//Debug.

// Project variables, non-static, at least used in other file.

// External variables.

// Global variables, statics.

// Private function prototypes.

// Private function bodies.

// Public function bodies.

/*
 * @brief
 * @Note
 * @param
 * @ret
 */
const char* FM_FACTORY_FirmwareVersionGet(void)
{
  return msg_version_firmware;
}

/*
 * @brief
 * @Note
 * @param
 * @ret
 */
const char* FM_FACTORY_ReleaseGet(void)
{
  return msg_release;
}

/*
 * brief    Devuelve los valores de fabrica para un sensor estándar.
 * note     En este mismo archivo se tienen valores típicos para varios sensores primarios, esto permite
 *          poder hacer pruebas con valores típicos que serian usados en la vida real del caudalímetro. sin
 *          Que luego de grabar la flash se tengan valores reales es util durante la etapa de desarrollo.
 * param    sel, modelo de sensor elegido como valores iniciales de configuración.
 * ret      Configuración inicial.
 */
fm_fmc_totalizer_t FM_FACTORY_TotalizerGet(sensors_list_t sel)
{
  fm_fmc_totalizer_t totalizer;

  switch (sel)
  {
  case FM_FACTORY_RAM_BACKUP:
    totalizer = FM_FMC_GetEnviroment();
    break;
  case FM_FACTORY_LAST_SETUP:
    totalizer = FM_FMC_GetEnviroment();
    break;
  case FM_FACTORY_SENSOR_0:
    totalizer = sensor_0;
    break;
  case FM_FACTORY_AI_25:
    totalizer = sensor_ai_25;
    break;
  case FM_FACTORY_AI_80:
    totalizer = sensor_ai_80;
    break;
  default:
    break;
  }
  return totalizer;
}

// Interrupts

/*** end of file ***/
