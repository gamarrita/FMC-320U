/* @file fm_module.h
 *
 * @brief A description of the module’s purpose.
 *
 * @par
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
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

#endif /* MODULE_H */

/*** end of file ***/