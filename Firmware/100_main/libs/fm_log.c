/*
 * Fecha 28 Junio 2025
 * Autor: Daniel Hernan Sagarra
 *
 * El tamaño de la memoria Flash en bytes es de (FM_FLASH_LOG_END - FLASH_LOG_START) + 1 = 1.032.192
 * El Tamaño de los datos, estructura que se guarda en flash, es de 32 bytes, DATA_SIZE.
 * La cantidad total de registros ese (1.032.192 / 32) = 32.256
 * El tamaño de la BACKUP RAM es de 2KB, preso se destina 1KB como buffer intermedio para almacenar
 *
 *
 */

// Includes.
#include <fm_log.h>
#include <fm_rtc.h>
#include "main.h"
#include "fm_flash.h"
#include "fm_debug.h"
#include "fm_fmc.h"
#include "fmx.h"

// Defines.
#define FALSE 0
#define TRUE 1

/*
 * Tamaño, en bytes, de cada dato que guarda el LOG. Es el tamaño de la variable fm_log_data_t.
 * Este valor no debería cambiar en la vida util del computador de caudales.
 */
#define DATA_SIZE 32

// Tamaño en bytes de la backup ram usada como memoria intermedia del logger para grabar en flash.
#define BACKUP_RAM_SIZE 1024

// Cantidad de datos (del tipo fm_log_data_t) que entran en la ram backup, la cuenta da 32.
#define DATA_BACKUP_RAM_LENGTH (BACKUP_RAM_SIZE / DATA_SIZE)

// Cantidad de datos (del tipo fm_log_data_t) que entran en la memoria FLASH.
#define DATA_FLASH_LENGTH (FM_FLASH_LOG_SIZE / DATA_SIZE)

/*
 * Limite a la cantidad de escrituras que se pueden hacer si no transcurre un tiempo mínimo, promedio.
 * Un contador inicia con este valor, cada vez que un registro se escribe el contador se decrementa, el mismo
 * contador se incrementa, como maximo hasta este valor.
 */
#define LOG_LIMIT_BY_WRITES 10

/*
 * Lapso de tiempo para incremento del contador medido en minutos.
 */
#define LOG_LIMIT_BY_TIME (TX_TIMER_TICKS_PER_SECOND * 60) * 5 // limite en un registro cada 5 minutos.

// Typedef.

// Const data.

// Debug.

// Project variables, non-static, at least used in other file.

// External variables.

// Global variables, statics.

// Posición de memoria donde se grabara el proximo registro del log.
uint32_t log_data_address = FM_FLASH_LOG_START;

// Las siguientes variables se usar para controlar el tiempo transcurrido entre llamadas a FM_LOG_Monitor();
ULONG monitor_this_time;  // Tick del RTOS en
ULONG monitor_last_time;
ULONG monitor_elapsed_ticks;

// Variable que controla cuantos registros quedan disponibles para escritura inmediata.
uint8_t monitor_index = LOG_LIMIT_BY_WRITES + 1; // valor inicial para detectar encendido.

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
fm_log_data_t data_buffer[DATA_BACKUP_RAM_LENGTH] __attribute__((section(".RAM_BACKUP_Section")));

// Contador en RAM BACKUP, cuenta la cantidad de resets:

// Private function prototypes.
void LogRam(fm_log_event_t event);
void LogFlash();

// Private function bodies.

// Public function bodies.

/*
 * @brief   Guarda datos en la RAM BACKUP hasta completar un bloque que se escribe en la FLASH.
 * @note
 * @param   data, datos a agregar a la BACKUP RAM
 * @retval  ninguno
 */
void LogRam(fm_log_event_t event)
{

    static uint8_t data_index = 0;

    data_buffer[data_index].ttl_pulses = FM_FMC_TtlPulseGet();
    data_buffer[data_index].acm_pulses = FM_FMC_AcmGetPulse();
    data_buffer[data_index].factor_cal = FM_FMC_FactorCalGet();
    data_buffer[data_index].seconds = FM_RTC_GetUnixTime();
    data_buffer[data_index].event = event;

    data_index++;

    if (data_index < DATA_BACKUP_RAM_LENGTH)  // length
    {
        //
    }
    else
    {
        data_index = 0;
        LogFlash();
    }
}

/*
 * @brief   Guarda el contenido de la RAM BACKUP en la FLASH.
 * @param
 * @retval  Cantidad de registros escritos, si no fueron DATA_BACKUP_RAM_LENGTH algo salio mal.
 */
void LogFlash()
{

    FM_FLASH_Write(log_data_address, (uint8_t*) data_buffer, DATA_SIZE);

    log_data_address += (DATA_BACKUP_RAM_LENGTH);

    if (log_data_address >= FM_FLASH_LOG_END)
    {
        log_data_address = FM_FLASH_LOG_START;
    }
}

/*
 * @brief
 * @note
 * @param
 * @retval
 */
void FM_LOG_Monitor(fmx_rate_status_t mode)
{
    if (monitor_index == LOG_LIMIT_BY_TIME + 1)
    {
        monitor_this_time = tx_time_get();
        monitor_last_time = monitor_this_time;
        monitor_elapsed_ticks = monitor_last_time;

        // "todo" si estoy en este punto el equipo se reinicio gravar evento con correspondiente flag.
    }

    if (monitor_index < LOG_LIMIT_BY_WRITES)
    {
        if (monitor_elapsed_ticks > LOG_LIMIT_BY_TIME)
        {
            monitor_index++;
        }
    }

    switch (mode)
    {
    case FMX_RATE_OFF:
        // No registra datos si el caudal es cero.
        break;
    case FMX_RATE_TO_ON:
        LogRam(FM_LOG_RATE_TO_ON);
        break;
    case FMX_RATE_ON:
        break;
    case FMX_RATE_TO_OFF:
        LogRam(FM_LOG_RATE_TO_OFF);
        break;
    default:
        FM_DEBUG_LedError(1);
        break;
    }
}

/*
 * @brief   Lee datos guardados del log, en FLASH, son bloques de tamaño DATA_BACKUP_RAM_LENGTH.
 * @param   data_index  0 no valido, es la próxima posición de memoria a escribir, no se puede leer.
 *                      1 lee el ultimo registro escrito en memoria FLASH.
 *                      2 lee el anteúltimo  registro escrito en memoria FLASH.
 *                      n lee el n-esimo registro.
 *                      (DATA_FLASH_LENGTH - 1) es el registro mas antiguo que se puede leer
 *          data_ptr,
 * @retval  cantidad de datos que se pudieron leer.
 */
fmx_status_t FM_LOG_ReadLog(uint32_t data_index, uint8_t *data_ptr)
{
    fmx_status_t ret_status = FMX_STATUS_OK;
    uint32_t memory_address;    // Posición inicial de memoria a leer
    uint32_t data_offset_index; //

    if (data_index >= DATA_FLASH_LENGTH)
    {
        // No se puede retroceder mas posiciones que la cantidad de registros que entran en flash.
        return FMX_STATUS_OUT_OF_RANGE;
    }

    // La memoria flash es circular, debe decidir si data_index retrocede mas alla  de FM_FLASH_LOG_START
    data_offset_index = (log_data_address - FM_FLASH_LOG_START) / DATA_SIZE;

    if (data_index <= data_offset_index)
    {
        // Se retrocede a un registro antes de llegar a FM_FLASH_LOG_START
        memory_address = log_data_address;
        memory_address -= (data_offset_index * DATA_SIZE);
    }
    else
    {
        data_offset_index -= (data_index + 1);
        memory_address = FM_FLASH_LOG_END - (data_offset_index * DATA_SIZE);
    }

    FM_FLASH_Read(memory_address, data_ptr, DATA_SIZE);

    return ret_status;
}

// Interrupts

/*** end of file ***/
