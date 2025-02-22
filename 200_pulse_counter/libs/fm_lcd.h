/*
 * Autor: Daniel H Sagarra
 *
 * Notas:
 */

#ifndef     FM_LCD_H_
#define     FM_LCD_H_

// Includes.
#include "fm_lcd_ll.h"
// Macros, defines, microcontroller pins (dhs).

// Typedef.

typedef enum
{
  USER, CONFIGURATION
} user_or_configuration_t;

// Defines.

// Function prototypes.
void FM_LCD_Init(uint8_t fill);
void FM_LCD_fill(uint8_t fill);
void FM_LCD_PutString(const char *str, uint32_t str_len, fm_lcd_ll_row_t row);
void FM_LCD_PutUnsignedInt32(uint32_t, fm_lcd_ll_row_t);
void FM_LCD_PutChar(char*);

#endif /* FM_LCD_H_ */

/*** end of file ***/
