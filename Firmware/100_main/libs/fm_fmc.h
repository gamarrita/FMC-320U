/*
 * Autor: Daniel H Sagarra
 *
 * Notas:
 *
 */

#ifndef FM_FMC_H_
#define FM_FMC_H_

// includes
#include "fm_lcd_ll.h"
#include "fmx.h"
#include <stdint.h> // defeniciones del tipo uint32_t

// Macros, defines, microcontroller pins (dhs).

// Typedef.

// Este tipo de datos es punto fijo, 3 decimales,
typedef uint32_t ufp3_t;

//
typedef enum
{
  FM_FACTORY_RAM_BACKUP,	// Usa el conjunto de datos guardados en la RAM.
  FM_FACTORY_LAST_SETUP,
  FM_FACTORY_SENSOR_0,	// Frecuencímetro, factor 1, base de tiempo segundos.
  FM_FACTORY_AI_25,		// Valor típico para turbinas de 1".
  FM_FACTORY_AI_80,		// Valor típico para turbina de 3".
} sensors_list_t;

//
typedef enum
{
  FM_FMC_FP_SEL_0 = 0, FM_FMC_FP_SEL_1, FM_FMC_FP_SEL_2, FM_FMC_FP_SEL_3,
} fm_fmc_fp_sel;

/*
 * La unidad de calibración por es siempre litro.
 */
typedef enum
{
  VOL_UNIT_BLANK = 0,	// Unidad adimensional debe estar siempre primera!!!!!!!!.
  VOL_UNIT_BR,    	// Barriles Americanos.
  VOL_UNIT_GL,    	// Galones.
  VOL_UNIT_KG,    	// Kilogramos.
  VOL_UNIT_LT,		// Litros, es la unidad principal ya que se calibra en litros.
  VOL_UNIT_M3,    	// Metro cubico.
  VOL_UNIT_ME,    	// Metro cubico equivalente, para un liquido criogénico circulando el computador
                    // indica el volumen de gas equivalente, no el volumen del liquido.
  VOL_UNIT_END,
} fm_fmc_vol_unit_t;

/*
 *
 */
typedef struct
{
  float unit_convert;		// litros = unidad * unit converter | unidad = litros / unit_converter
  char name[3];				// Nombre corto del la unidad
} fm_fmc_vol_data_t;

/*
 *
 */
typedef enum
{
  TIME_UNIT_SECOND = 0, TIME_UNIT_MINUTE, TIME_UNIT_HOUR, TIME_UNIT_DAY, TIME_UNIT_END
} fm_fmc_time_unit_t;

/*
 *
 */
typedef struct
{
  double factor_r;		// Se usa para calcular el caudal. Tiene en cuenta factor k + unidad de tiempo
  ufp3_t delta_t;     	// Tiempo transcurrido.
  ufp3_t delta_p;     	// Pulsos caumulados en delta_t.
  ufp3_t rate;        	// rate = r_factor * delta_p / delta_t.
  uint8_t rate_pf_sel;	// Decimales a mostrar para
  ufp3_t limit_high;  	// Caudal máximo nominal del sensor.
  ufp3_t limit_low;   	// Caudal mínimo nominal del sensor.
  uint32_t filter;
} fm_fmc_rate_t;

/*
 * Se crea el siguiente tipo de dato para contabilizar el volumen. El volumen es el resultado de
 * dividir los pulsos acumulador por el factor. El factor
 *
 */
typedef struct
{
  ufp3_t acm;			// El tipo de dato para el ACM es punto fijo con 3 decimales.
  ufp3_t ttl;			// El tipo de dato para el TTL es punto fijo con 3 decimales.
  uint8_t vol_pf_sel;	// Decimales a mostrar para ACM y TTL máximo 3 mínimo 0.
  uint64_t pulse_acm; // Pulsos del ACM
  uint64_t pulse_ttl; // Pulsos del TTL
  ufp3_t factor_cal;    // Factor de calibración, pulsos / litro, siempre se calibra en litro.
  double factor_k;   	// Factor k, se calcula a partir de factor_c y unidades seleccionadas.
  fm_fmc_vol_unit_t vol_unit; // Unidades de volumen, esta tambien se usa en el caudal.
  fm_fmc_time_unit_t time_unit;
  fm_fmc_rate_t rate;
  uint16_t ticket_number;
} fm_fmc_totalizer_t;

// Defines.

#define FM_FMC_FACTOR_CAL_MAX 99999999
#define FM_FMC_FACTOR_CAL_MIN 1000

// Function prototypes

// Las siguientes funciones operan en totalizer.acm
ufp3_t FM_FMC_AcmCalc();
ufp3_t FM_FMC_AcmGet();
void FM_FMC_AcmReset();

// Las siguientes funciones operan sobre los factores K y calibración.
//void 	FM_FMC_FactorCalcRenew();
ufp3_t  FM_FMC_FactorCalGet();
uint32_t FM_FMC_FactorCalSet(ufp3_t factor_cal);
double FM_FMC_FactorKCalc(ufp3_t factor_k, fm_fmc_vol_unit_t unit);
double FM_FMC_FactorKGet();
uint32_t FM_FMC_FactorKSet(ufp3_t factor_k);
double FM_FMC_FactorRateCalc(double factor_k, fm_fmc_time_unit_t unit);
fmx_status_t FM_FMC_FactorRateSet(double factor_rate);

fm_fmc_totalizer_t FM_FMC_GetEnviroment(void);
void FM_FMC_Init(sensors_list_t);

// Las siguientes funciones operan en totalizar.rate
void    FM_FMC_CaptureSet(uint16_t pulse, uint16_t time);
ufp3_t  FM_FMC_RateCalc();
void    FM_FMC_RateClear();
ufp3_t  FM_FMC_RateGet();
uint8_t FM_FMC_RateFpSelGet();
void    FM_FMC_RateFpInc();

// Las siguientes funciones operan en totalizer.
uint8_t FM_FMC_TotalizerFpSelGet();
void FM_FMC_TotalizerFpInc();
void FM_FMC_TotalizerStrUnitGet(char **string, fm_fmc_vol_unit_t);
void FM_FMC_TotalizerTimeUnitSel(fm_fmc_time_unit_t sel);
fm_fmc_time_unit_t FM_FMC_TotalizerTimeUnitGet();
fmx_status_t FM_FMC_TotalizerTimeUnitSet(fm_fmc_time_unit_t time_unit);
fm_fmc_vol_unit_t FM_FMC_TotalizerVolUnitGet();
uint32_t FM_FMC_TotalizerVolUnitSet(fm_fmc_vol_unit_t);
uint16_t FM_FMC_TicketNumberGet();


// Las siguientes funciones operan en totalizar.factor_cal

// Las siguientes funciones operan en totalize.ttl
ufp3_t FM_FMC_TtlCalc();
ufp3_t FM_FMC_TtlGet();
uint64_t FM_FMC_TtlPulseGet();
void FM_FMC_TtlReset();

// Las siguientes funciones operan sobre...

void FM_FMC_PulseAdd(uint32_t pulse_delta);

#endif

/*** end of file ***/
