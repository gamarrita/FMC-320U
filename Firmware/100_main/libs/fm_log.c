/*
 * @brief Manejo del registro de datos.
 *
 *
 *
 *
 *
 * Autor: Daniel Hernan Sagarra
 */

// Includes.
#include <fm_log.h>
#include <fm_rtc.h>
#include "main.h"
#include "fm_flash.h"
#include "fm_debug.h"
#include "fm_fmc.h"

// Defines.

/*
 * Tamaño, en bytes, de cada dato que guarda el LOG. Es el tamaño de la variable fm_log_data_t.
 * Este valor no debería cambiar en la vida util del computador de caudales.
 */
#define DATA_LENGTH 32

 // Tamaño de la backup ram como memoria intermedia a la flash.
#define BACKUP_RAM_LENGTH 1024

 // Cantidad de datos que entra en la ram backup, la cuenta da 32.
#define BUFFER_SIZE (BACKUP_RAM_LENGTH / DATA_LENGTH)


// Typedef.

// Const data.

// Debug.

// Project variables, non-static, at least used in other file.

// External variables.

// Global variables, statics.

/*
 * Buffer en RAM, almacena varios registros del data logger antes de grabar en flash.
 * La cantidad de registros debe responder a varías relaciones de compromiso..
 * - No se quiere grabar la flash de a un registro, es ineficiente para el consumo y procesamiento. Se
 *   prefiere guardar de a paquetes.
 * - Es deseable que la RAM del buffer sea la BACKUP RAM, que se alimenta con la batería secundaria.
 *   En la vida util el computador no se agota reemplaza la batería secundaria, si la primaria.
 *   El tamaño de la BACKUP RAM es de 2KB, y solo 1 KB se podría destinar a este buffer.
 *   Con 1 KB para el buffer tenemos 1024 / 32 = 32 registros.
 *   Se presume que 16 registros son suficientes, estaría optimizado para el bajo consumo, si esto
 *   no resulta en la practica, se buscara otro esquena, posiblemente mixto, usando BACKUP RAM Y RAM.
 */
fm_log_data_t data_buffer[BUFFER_SIZE] __attribute__((section(".RAM_BACKUP_Section")));

uint8_t data_index = 0;

// Contador en RAM BACKUP, cuenta la cantidad de resets:

// Private function prototypes.

// Private function bodies.

// Public function bodies.

/*
 * @brief   Guarda datos en la RAM BACKUP hasta completar un bloque que se escribe en la FLASH.
 * @note
 * @param   data, datos a agregar a la BACKUP RAM
 * @retval  ninguno
 */
void FM_LOG_Ram(fm_log_data_t data)
{

  data.ttl = FM_FMC_TtlGet();
  data.acm = FM_FMC_AcmGet();
  data.factor_cal =  FM_FMC_FactorCalGet();
  data.seconds = FM_RTC_GetUnixTime();
  data_buffer[data_index] = data;

  data_index++;

  if(data_index >= BUFFER_SIZE) // length
  {
    data_index = 0;
    FM_LOG_Flash();
  }
}

/*
 * @brief   Guarda el contenido de la RAM BACKUP en la FLASH.
 * @note
 * @param
 * @retval  ninguno
 */
void FM_LOG_Flah()
{
  int a = 1;

  while(a)
  {
    FM_DEBUG_LedError(1);
  }
}

void FM_LOG_Monitor()
{

}



// Interrupts

/*** end of file ***/
