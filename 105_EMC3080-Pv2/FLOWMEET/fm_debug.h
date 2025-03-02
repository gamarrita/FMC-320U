/*
 * Autor: Daniel H Sagarra
 * 
 * Notas:
 * 
 */

#ifndef     FM_DEBUG_H_
#define     FM_DEBUG_H_

// includes.
#include "main.h"
#include "string.h"
#include "stdio.h"

// Macros, defines, pins del Micro pins.

// Typedef.

// Defines.

// Varibles extern usadas en otros módulos.


// Function prototypes
void FM_DEBUG_Init();
void FM_DEBUG_ItmMsg(const char *msg, uint8_t len);
void FM_DEBUG_LedActive(int status);
void FM_DEBUG_LedError(int status);
void FM_DEUBUG_LedSignal(int status);
void FM_DEBUG_UartMsg(const char *p_msg, uint8_t len);
void FM_DEBUG_UartUint8(uint8_t num);
void FM_DEBUG_UartUint16(uint16_t num);
void FM_DEBUG_UartUint32(uint32_t num);
void FM_DEBUG_UartInt32(int32_t num);
void FM_DEBUG_UartFloat(float num);

#endif /* FM_DEBUG_H */

/*** end of file ***/
