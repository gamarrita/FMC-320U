/* @file fm_lcd.h
 *
 * @brief
 *
 * Este es el modulo que posee la inteligencia para manejar las opciones que
 * brinda la pantalla LCD, utilizando las funciones implementadas de impresión
 * de caracteres individuales en lcd.h.
 * Las funciones de este modulo serán usadas por librerías de mas alto nivel que
 * impriman menús o controlen máquinas de estado de pantallas, tales como
 * fm_menu_user.h.
 *
 * Modo de uso:
 *
 * 1) Al iniciar un nuevo proyecto puede usar el archivo lcd.ioc para configurar
 * automaticamente.
 *
 * 2) Se debe agregar la carpeta al proyecto, sobre el nombre del proyecto click
 * derecho new -> folder -> link to alternate location -> ....
 *
 * 3) Se debe indicar al IDE que hay una nueva carpeta con codigo fuente click
 * derecho en el nombre del proyecto new -> source folder-> browse -> ....
 *
 * 4) #include "../../../../libs/fm_lcd/fm_lcd.h"
 *
 * 5) fm_lcd_init();
 *
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 *
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
