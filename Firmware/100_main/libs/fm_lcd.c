/* @file fm_lcd.c
 *
 * @brief Este es el modulo que posee la inteligencia para manejar las opciones
 * que brinda la pantalla LCD, utilizando las funciones implementadas de
 * impresión de caracteres individuales en lcd.h.
 * Las funciones de este modulo serán usadas por librerías de mas alto nivel que
 * impriman menús o controlen máquinas de estado de pantallas, tales como
 * fm_menu_user.h.
 *
 * Version 1
 * Autor: Daniel H Sagarra
 * Fecha 10/11/2024
 * Modificaciones: version inicial.
 *
 *
 */

// Includes.
#include "fm_lcd.h"
#include "stdio.h"

// Typedef.

// Const data.

// Defines.
#define LCD_BUFFER_SIZE 20

// Project variables, non-static, at least used in other file.

// External variables.

// Global variables, statics.

char lcd_buffer[LCD_BUFFER_SIZE];

// Private function prototypes.

// Private function bodies.

// Public function bodies.

/*
 * @brief	Es la primera instrucción a llamar para usar el LCD.
 * @param 	Fill, en terminos practico solamente los valores 0x0 y 0xFF tienen sentido, para el
 * 			valor el LCD se inicia con todos los segmentos apagados, con el segundo con todos los
 * 			segmentos encendido. El driver PCF8553 tiene un mapa de memoria 20 bytes, son 160
 * 			segmentos. Esta función copia a cada byte del driver el valor de fill. La codificación
 * 			para tener caracteres en la pantalla tiene una distribución "compleja", los bits de
 * 			cada caracter están distribuidos por el mapa de memoria.
 * @retval	None
 *
 */
void FM_LCD_Init(uint8_t fill)
{
  FM_LCD_LL_Init(fill);
}

/*
 * @brief   Escribe en la linea de 7 segmentos
 * @param
 * @retval
 */
void FM_LCD_PutChar(char *ptr)
{
  FM_LCD_LL_PutChar_1(*ptr);
  FM_LCD_LL_PutChar_2(*(ptr + 1));
}

/*
 * @brief   Imprime un string en la filas 1 y 2 (8 y 7 caracteres) del LCD.
 * @note
 * @param	my_str, string a imprimir.
 * @param	len, longitud del string.
 * @param	row selecciona la fila superior, 8 caracteres, o la inferior 7 caracteres.
 * @retval 	ninguno.
 *
 */
void FM_LCD_PutString(const char *my_str, uint32_t len, fm_lcd_ll_row_t row)
{
  uint8_t str_index = 0;
  uint8_t lcd_index = 0;

  int index_end;

  if (row == FM_LCD_LL_ROW_1)
  {
    // Se ajustan los caracteres a imprimir a la cantidad máxima que se puede imprimir en esta linea.
    index_end = FM_LCD_LL_ROW_1_COLS;
  }
  else
  {
    // Se ajustan los caracteres a imprimir a la cantidad máxima que se puede imprimir en esta linea.
    index_end = FM_LCD_LL_ROW_2_COLS;
  }

  if (len < index_end)
  {
    // Se necesita imprimir menos columnas que la cantidad disponibles.
    index_end = len;
  }

  while ((lcd_index < index_end) && my_str[str_index])
  {
    FM_LCD_LL_PutChar(my_str[str_index], lcd_index, row);
    str_index++;

    /*
     * Si el proximo caracter a escribir es un punto ".", se debe manejar de manera diferente
     * a caracteres normales. El punto no es el siguiente caracter, es parte del mismo
     * símbolo ya escrito.
     */
    if (my_str[str_index] != '.')
    {
      lcd_index++;
    }
  }
}

/*
 * @brief	Función que formatea información pasada como parámetro para ser
 * 			colocada en una de las dos filas de la pantalla LCD.
 * @param	data, valor numero a imprimir.
 * @param	row, seleccion la fila superior, 8 caracteres, o la inferior 7 caracteres.
 * @retval 	Ninguno.
 *
 */
void FM_LCD_PutUnsignedInt32(uint32_t num, fm_lcd_ll_row_t row)
{
  uint8_t index_lcd; // apunta a la posicion del la fila del LCD que hay que escribir
  int index_num; // apunta al digito del dato que hay que escribir

  // Apunta al ultimo caracter del la fila selecciona del LCD
  index_lcd = FM_LCD_LL_GetRowSize(row);

  // Convierto el numero a texto, index_num es igual al numero de cifras.
  index_num = snprintf(lcd_buffer, sizeof(lcd_buffer), "%lu", num);

  if (index_num > index_lcd)
  {
    index_num = index_lcd; // no se pueden imprimir mas cifras de la capacidad de la linea.
  }

  while (index_num)
  {
    // La primera columna es la 0 y la ultima el tamaño -1, resto 1 a las columnas.
    index_lcd--;
    index_num--;
    FM_LCD_LL_PutChar(lcd_buffer[index_num], index_lcd, row);
  }
}

// Interrupts

/*** end of file ***/

