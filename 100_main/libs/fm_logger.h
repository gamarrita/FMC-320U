/*
 * Autor: Daniel H Sagarra
 * 
 * Notas:
 * 
 */

#ifndef FM_LOGGER_H_
#define FM_LOGGER_H_

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
  uint64_t  ttl;            // 8
  uint64_t  acm;            // 8+8=16
  ufp3_t    factor_k;       // 16+4=20
  uint16_t  temp_ext;       // 20+2=22
  uint32_t  seconds;        // 22+4=26
  uint8_t   reg_1;          // 26+1=27
  uint8_t   reg_2;          // 27+1=28
  uint8_t   reserved[36];   // 28+36=64
}fm_logget_data_t;

// Macros, defines, microcontroller pins (dhs).

// Varibles extern

// Defines.

// Function prototypes

#endif /* FM_LOGGER_H */

/*** end of file ***/
