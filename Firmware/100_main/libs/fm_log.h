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


/*
 * Defino el tipo de dato que se guardara en memoria del log, es de 64 bytes. Se usan 28 bytes, quedan 36
 * bytes libres para uso futuro. La cantidad de memoria disponible 1MB - 16KB, con esto tenemos la siguiente
 * cantidad de registros:
 * Registros: (1.048.576 - 16384) / 64 = 16.128
 * Explicación, la memoria de pseudo RAM es de 1MB, pero reservo dos paginas de 8KB para datos de micro y
 * datos de configuración, el resto es para el logger.
 *
 * La cantidad de 16.128 registros, se agotaría en un año si se acumulan, 44 registros en un año.
 *
 */
typedef struct
{
  uint32_t  ttl;            // 4
  uint32_t  acm;            // 4+4=8
  uint32_t  factor_k;       // 8+4=12     // Ideal guardar un factor de calibración en pulsos/litro
  uint32_t  seconds;        // 12+4=16
  uint16_t  temp_rtd;       // 16+2=18
  uint16_t  temp_int;       // 18+2=20
  uint8_t   reserverd[12];   // 20+12=32
}fm_log_data_t; // No se puede modificar este typedef, explicación en sección de defines de fm_log.c

// Macros, defines, microcontroller pins (dhs).

// Varibles extern

// Defines.


// Function prototypes
void fm_log_data_add(fm_log_data_t data);
void fm_log_block_flash();
void fm_log_data_check();


#endif /* FM_LOGGER_H */

/*** end of file ***/
