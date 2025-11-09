/**
 * @file fm_log.c
 * @brief	Gestiona los datos guardados en la BACKUP RAM de manera directa.
 * 			Para la memoria flash comprueba los limites y datos a guardar, la
 * 			escritura de la flash se hace con una API externa.
 *
 */

#include "fm_log.h"
#include "fm_rtc.h"
#include "main.h"
#include "fm_flash.h"
#include "fm_debug.h"
#include "fm_fmc.h"
#include "fmx.h"

// --- Constants ---

#define DATA_SIZE                 (32u)
#define BACKUP_RAM_SIZE           (1024u)
#define DATA_BACKUP_RAM_LENGTH    (BACKUP_RAM_SIZE / DATA_SIZE)
#define DATA_FLASH_LENGTH         (FM_FLASH_LOG_SIZE / DATA_SIZE)

// --- Logger state ---

static uint32_t log_data_address = FM_FLASH_LOG_START;
static fm_log_data_t data_buffer[DATA_BACKUP_RAM_LENGTH] __attribute__((section(".RAM_BACKUP_Section")));

// --- Private functions ---

static void LogRam();
static void LogFlash(void);


/**
 * Reads log entries from Flash in reverse chronological order.
 * @param data_index Relative index: 1 returns the latest entry, etc.
 * @param data_ptr Destination buffer to copy the entry into.
 * @return Number of bytes read or an error code.
 */
static void LogRam()
{
    static uint8_t data_index = 0u;

    data_buffer[data_index].ttl_pulses = FM_FMC_TtlPulseGet();
    data_buffer[data_index].acm_pulses = FM_FMC_AcmGetPulse();
    data_buffer[data_index].factor_cal = FM_FMC_FactorCalGet();
    data_buffer[data_index].time_unix  = FM_RTC_GetUnixTime();
    data_buffer[data_index].event      = 0;

    data_index++;

    if (data_index >= DATA_BACKUP_RAM_LENGTH) {
        data_index = 0u;
        // TODO llamado a LogFlash()
    }
}

/**
 * Reads log entries from Flash in reverse chronological order.
 * @param data_index Relative index: 1 returns the latest entry, etc.
 * @param data_ptr Destination buffer to copy the entry into.
 * @return Number of bytes read or an error code.
 */
static void LogFlash(void)
{
    FM_FLASH_Write(log_data_address, (const uint8_t *)data_buffer, DATA_SIZE);
    log_data_address += DATA_BACKUP_RAM_LENGTH;

    if (log_data_address >= FM_FLASH_LOG_END) {
        log_data_address = FM_FLASH_LOG_START;
    }
}

// --- API ---

/**
 * Reads log entries from Flash in reverse chronological order.
 * @param data_index Relative index: 1 returns the latest entry, etc.
 * @param data_ptr Destination buffer to copy the entry into.
 * @return Number of bytes read or an error code.
 */
fmx_status_t FM_LOG_ReadLog(uint32_t data_index, uint8_t *data_ptr)
{
    fmx_status_t ret_status = FMX_STATUS_OK;
    uint32_t memory_address;
    uint32_t data_offset_index;

    if (data_index >= DATA_FLASH_LENGTH) {
        return FMX_STATUS_OUT_OF_RANGE;
    }

    data_offset_index = (log_data_address - FM_FLASH_LOG_START) / DATA_SIZE;

    if (data_index <= data_offset_index) {
        memory_address = log_data_address;
        memory_address -= (data_offset_index * DATA_SIZE);
    } else {
        data_offset_index -= (data_index + 1u);
        memory_address = FM_FLASH_LOG_END - (data_offset_index * DATA_SIZE);
    }

    FM_FLASH_Read(memory_address, data_ptr, DATA_SIZE);
    return ret_status;
}

