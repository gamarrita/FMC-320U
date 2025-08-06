/*
 * Autor: Daniel H Sagarra
 * 
 * Notas:
 * 
 */

#ifndef FM_LOG_H_
#define FM_LOG_H_

// includes
#include "fm_fmc.h"
// Typedef y enum.

typedef enum
{
    FM_LOG_POWER_OFF,  // Baterias primarias desconectadas
    FM_LOG_POWER_ON,  // Se enciende computador de caudales
    FM_LOG_RATE_TO_ON,  // Caudal inicia
    FM_LOG_RATE_TO_OFF,  // Caudal se detiene
    FM_LOG_RATE_HIGH_ALARM,  // Caudal por encima del limite
    FM_LOG_RATE_LOW_ALARM,  // Caudal por debajo del limite
} fm_log_event_t;

/*
 * Defino el tipo de dato que se guardara en memoria del log, es de 32 bytes. Se usan 29 bytes, quedan 3
 * bytes libres para uso futuro. La cantidad de memoria disponible 1MB - 16KB, con esto tenemos la siguiente
 * cantidad de registros:
 * Registros: (1.048.576 - 16384) / 32 = 32.256
 * Explicación, la memoria de pseudo RAM es de 1MB, pero reservo dos paginas de 8KB para datos de micro y
 * datos de configuración, el resto es para el logger.
 *
 */
typedef struct
{
    uint64_t ttl_pulses;  // 8
    uint64_t acm_pulses;  // 8+8=16
    uint32_t factor_cal; // 16+4=20     // Factor de calibración en pulsos/litro
    uint32_t seconds;  // 20+4=24
    uint16_t temp_rtd;  // 24+2=26
    uint16_t temp_int;  // 26+2=28
    fm_log_event_t event;  // 28+1=29
    uint8_t reserved_1;  // 29+1=30
    uint16_t reserved_2;  // 30+2=32
} fm_log_data_t;  // No modificar el tamaño.

// Macros, defines, microcontroller pins (dhs).

// Varibles extern

// Defines.

// Function prototypes
void FM_LOG_Monitor(fmx_rate_status_t mode);
uint16_t FM_LOG_ReadFlash(uint16_t data_start, uint16_t data_count, fm_log_data_t **data_ptr);

#endif /* FM_LOGGER_H */

/*** end of file ***/
