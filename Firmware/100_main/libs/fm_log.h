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
  uint64_t  ttl;        // 8
  uint64_t  acm;        // 8+8=16
  uint32_t  factor_cal; // 16+4=20     // Factor de calibración en pulsos/litro
  uint32_t  seconds;    // 20+4=24
  uint16_t  temp_rtd;   // 24+2=26
  uint16_t  temp_int;   // 26+2=28
  uint8_t   flag;       // 28+1=29
  uint8_t   reserved_1; // 29+1=30
  uint16_t  reserved_2; // 30+2=32
}fm_log_data_t; // No modificar el tamaño.

// Macros, defines, microcontroller pins (dhs).

// Varibles extern

// Defines.


// Function prototypes
void FM_LOG_Monitor();
void FM_LOG_Ram(fm_log_data_t data);
void FM_LOG_Flash();



#endif /* FM_LOGGER_H */

/*** end of file ***/
