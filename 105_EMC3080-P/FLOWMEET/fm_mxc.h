/*
 *
 * Instrucciones:
 * - #include "fm_main.h // copiar esta linea en main.c
 * - Nueva carpeta con el nombre FLOWMEET en el workspace
 * - Hacer que la carpeta FLOWWMEET ser "source floder"
 * - En las propiedades del proyecto la carpeta FLOWMEET de ser "include paths" en C/C++ Build->Settings MCU/MPU GVV Compiler
 * - Agregar la sentencia FM_MAIN_Main(); en el while infinico del main.c
 */

#ifndef FM_MXC_H_
#define FM_MXC_H_

// includes

// Typedef y enum.

typedef enum
{
  FM_MXC_AT_UARTE_ID, FM_MXC_AT_UARTE_ON_ID, FM_MXC_AT_UARTE_OFF_ID, FM_MXC_AT_STANDBY_ID,
} fm_cmx_at_id_t;

typedef enum
{
  FM_MXC_FAIL = 0, FM_MXC_OK = 1
} fm_mxc_status_t;

// Macros, defines, microcontroller pins (dhs).

// Varibles extern

// Defines.
#define FM_MXC_DISABLE 0    // MXChip disable.
#define FM_MXC_ENABLE  1    // MXChip enable.
#define FM_MXC_RESET   2    //

#define FM_MXC_FAIL     0
#define FM_MXC_OK       1

// Function prototypes
void FM_MXC_Enable(int mode);
void FM_MXC_InitPtr();
void FM_MXC_Plus();

fm_mxc_status_t FMC_MXC_ReciveStatus();
fm_mxc_status_t FM_MXC_SendAT(fm_cmx_at_id_t id, int retry);

#endif  // FM_MAIN_H

/*** end of file ***/
