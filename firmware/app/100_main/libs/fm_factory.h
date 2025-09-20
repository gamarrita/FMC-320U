/*
 * Autor: Daniel H Sagarra
 *
 * Notas:
 *
 */

#ifndef FM_FACTORY_H_
#define FM_FACTORY_H_

// includes
#include "fm_fmc.h"

// Macros, defines, microcontroller pins (dhs).

// Typedef.

// Defines.

// Function prototypes

fm_fmc_totalizer_t FM_FACTORY_TotalizerGet(sensors_list_t sel);
const char* FM_FACTORY_FirmwareVersionGet(void);
const char* FM_FACTORY_ReleaseGet(void);

#endif /* MODULE_H */

/*** end of file ***/
