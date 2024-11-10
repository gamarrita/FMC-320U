/*
 *
 * Instrucciones:
 * - #include "fm_main.h // copiar esta linea en main.c
 * - Nueva carpeta con el nombre FLOWMEET en el workspace
 * - Hacer que la carpeta FLOWWMEET ser "source floder"
 * - En las propiedades del proyecto la carpeta FLOWMEET de ser "include paths" en C/C++ Build->Settings MCU/MPU GVV Compiler
 * - Agregar la sentencia FM_MAIN_Main(); en el while infinico del main.c
 */

#ifndef FM_EMC3080_H_
#define FM_EMC3080_H_

// includes

// Typedef y enum.

// Macros, defines, microcontroller pins (dhs).

// Varibles extern

// Defines.

// Function prototypes
void FM_EMC3080_Power(int mode);
void FM_EMC3080_Plus();
void FM_EMC3080_Standby();

#endif  // FM_MAIN_H

/*** end of file ***/
