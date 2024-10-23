/* @file fm_debugh.h
 *
 * Author: DHS
 * Version: beta 1
 * 
 * @brief 
 * 
 * Date: 11/02/2024
 * Name: DHS
 * - Se crea este archivo a partir de uno anterior.
 * -
 * Resultado Version 1.0
 * 
 * 
 * 
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 */

#ifndef     FM_DEBUG_H_
#define     FM_DEBUG_H_

// includes.
#include "main.h"
#include "string.h"
#include "stdio.h"

// Macros, defines, microcontroller pins (dhs).

// Typedef.

// Defines.

// Varibles extern usadada en otros modulos

extern const uint32_t fm_debug_uart_msg;
extern const uint32_t fm_debug_uart_error;

// Function prototypes

void FM_DEBUG_ItmMsg(const char *msg, uint8_t len);
void FM_DEUBUG_LedError();
void FM_DEBUG_UartMsg(const char *p_msg, uint8_t len);
void FM_DEBUG_UartUint8(uint8_t num);
void FM_DEBUG_UartUint16(uint16_t num);
void FM_DEBUG_UartUint32(uint32_t num);
void FM_DEBUG_UartInt32(int32_t num);
void FM_DEBUG_UartFloat(float num);

#endif /* FM_DEBUG_H */

/*** end of file ***/
