/*
 *
 * @brief Permire usar la flash como RAM. Configura la memoria RAM BACKUP
 *
 * El cube MX tiene la herramienta "Memory Managment" que ayuda a configurar bloques de FLASH, en nuestro
 * aso para usarlos como pseudo RAM.
 *
 *	MEMORY
 *	{
 *	  FLASH	(rx)	: ORIGIN = 0x08000000, LENGTH = 1024K
 *	  RAM_BACKUP	(xrw)	: ORIGIN = 0x40036400, LENGTH = 2K
 *	  RAM	(xrw)	: ORIGIN = 0x20000000, LENGTH = 768K
 *	  FLASH_LOG	(rx)	: ORIGIN = 0x08100000, LENGTH = 1024K
 *	  SRAM4	(xrw)	: ORIGIN = 0x28000000, LENGTH = 16K
 *	}
 *
 *
 *	Access permission, para cada bloque de flash que usemos debemos cambiar a: RW by any privilege level.
 *
 * 
 * Version 1
 * Autor: Daniel H Sagarra
 * Fecha 10/11/2024
 * Modificaciones: version inicial.
 * 
 * Version 2
 *
 *
 */

// Includes.
#include "main.h"
#include "fm_flash.h"
// Typedef.

/*
 *  Rango de memoria flash usado como pseudo ram, todos los usos posibles.
 */
#define FLASH_START	0x08100000
#define FLASH_END	0x081FFFFF

/*
 * La pagina es el minimo tamaño borrable. Antes de escribir hay que borrar y se borra de a n
 * paginas. Adicionalmente, toda seccion de memoria a ser usada como pseudo-flash tendía un tamaño
 * entero de paginas
 */
#define PAGE_SIZE  FLASH_PAGE_SIZE // 8KB en STM3U575

// Parte de la pseudo flash se la reserva para guardar información del chip.
#define FLASH_CHIP_INFO_START	0x08100000
#define FLASH_CHIP_INFO_END		0x08101FFF

// Parte de la pseudo ram se la usa para el equipo, configuraciones calibraciones, etc.
#define FLASH_DEVICE_START	0x08102000
#define FLASH_DEVICE_END	0x08103FFF

// Rango de memoria usado para data logger
#define FLASH_LOG_START	0x08104000
#define FLASH_LOG_END	0x081FFFFF

// Const data.

// Defines.

// Debug.

// Project variables, non-static, at least used in other file.

// External variables.

// Global variables, statics.

// Guarda información del chip.
flash_chip_info_t const chip_info __attribute__ ((section(".FLASH_CHIP_Section"))) =
{
    .reset_counter = 0, // cuenta cuantas veces se reseteo el equio
    .reset_factory = 0, // cuenta cuantas veces	se hiciero resets de fabrica.
    };// Conteo de resets.

// Private function prototypes.

// Private function bodies.

// Public function bodies.

/*
 * @brief	Lee información relacionada al funcionamiento del chip.
 * @note	Hasta el momento solo se implemento la cantidad de resets, se puede agregar temperatura,
 * 			alarmas tamper, etc.
 * @param 	Ninguno.
 * @retval 	Información del chip.
 */
flash_chip_info_t FM_FLASH_ChipInfoRead()
{
  flash_chip_info_t chip_info;
  FM_FLASH_Read(FLASH_CHIP_INFO_START, (uint8_t*) &chip_info, FM_FLASH_CHIP_INFO_SIZE);
  return chip_info;
}

/*
 *
 */
void FM_FLASH_ChipInfoWrite(flash_chip_info_t chip_info)
{
  FM_FLASH_Write(FLASH_CHIP_INFO_START, (uint8_t*) &chip_info, sizeof(chip_info));
}

/*
 * @brief	Ante un nuevo reset se incrementa el contador de resets.
 * @note
 *
 */
uint16_t FM_FLASH_NewReset()
{
  flash_chip_info_t chip_info;
  chip_info = FM_FLASH_ChipInfoRead();

  if (chip_info.reset_counter == 0)
  {
    /*
     * Es la primera vez que se enciende el microcontrolador luego de programar. En este bloque
     * se puede aprovechar para ejecutar codigo necesario en esta situacion.
     */
  }

  chip_info.reset_counter++;
  FM_FLASH_ChipInfoWrite(chip_info);

  return chip_info.reset_counter;
}

/*
 * @brief	Escribe datos en la flash
 * @note	La cantidad de bytes a escribir debe ser una calidad entera de blockes,
 * @param   address:	posición de memoria, desde 0 hasta tamño_de_memoria - 1.
 * @param   data:
 * @param   data_length
 * @retval  cantidad de bytes escritos
 *
 */
uint32_t FM_FLASH_Write(uint32_t memory_address, uint8_t *data, uint16_t data_length)
{
  uint8_t quad_word[FM_FLASH_BLOCK_SIZE]; // En flash, se deben agrupar en esta variable.
  uint32_t error_status = 0;
  uint32_t block_aling;

  // Antes de escribir hay que borrar la flash. Se debera inicializar esta estructura.
  FLASH_EraseInitTypeDef flash_erase_struct =
  {
      0 };

  // Verifico que la dirección de memoria inicial sea valida
  if ((memory_address) < FLASH_START)
  {
    return 0; // Datos a escribir por fuera de BANK 2
  }

  /*
   * La longitud  de datos a escribir debería ser una cantidad enteras de bloque.
   * Si no lo es ajusto a que lo sea, no se escribirían los datos que no completen un bloque.
   * Si data_lenth es menor que un bloque no se escribe nada.
   */
  block_aling = data_length % FM_FLASH_BLOCK_SIZE;
  data_length -= block_aling;

  // Verifico que la dirección de memoria final sea valida.
  if ((memory_address + data_length) > FLASH_END)
  {
    return 0;
  }

  // Verifico que la posición de memoria inicial esta al comienzo de un bloque
  if (memory_address % FM_FLASH_BLOCK_SIZE)
  {
    return 0;
  }

  // Para método de borrado se elige borrar paginas.
  flash_erase_struct.TypeErase = FLASH_TYPEERASE_PAGES;

  HAL_FLASH_Unlock();

  // Primera pagina a borrar.
  flash_erase_struct.Page = (memory_address - FLASH_START) / FLASH_PAGE_SIZE;

  /*
   *  Cantidad de paginas a borrar. Se necesita condicional para detectar si la cantidad de datos a escribir,
   *  primero se deben borrar, es múltiplo entero de una pagina, explicación:
   *  - Si la cantidad de datos es múltiplo entero, de una pagina, el cociente da el numero exacto de paginas
   *  a borrar
   *  - Si no la cantidad a borrar no es múltiplo entero, da resto, esto quiere decir que se necesita borrar
   *  una pagina completa para que entre dicho resto.
   */
  if(data_length % FLASH_PAGE_SIZE)
  {
    flash_erase_struct.NbPages = 1 + data_length / FLASH_PAGE_SIZE;
  }
  else
  {
    flash_erase_struct.NbPages = data_length / FLASH_PAGE_SIZE;
  }


  // Selecciono banco, solamente uso el BANK 2.
  flash_erase_struct.Banks = FLASH_BANK_2;

  // Debo borrar antes de escribir.
  HAL_FLASHEx_Erase(&flash_erase_struct, &error_status);

  // Loop para escritura de memoria.
  int i = 0;

  while (i < data_length)
  {
    quad_word[i % FM_FLASH_BLOCK_SIZE] = data[i];
    i++;

    // Solamente programo si quedo cargado el bloque completo en quad_word.
    if ((i % FM_FLASH_BLOCK_SIZE) == 0)
    {
      HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, memory_address + i - FM_FLASH_BLOCK_SIZE,
          (uint32_t) quad_word);
    }
  }

  HAL_FLASH_Lock();

  return 1;
}

/*
 * @brief 	leer un valor en memoria flash
 * @note	cuando se quiere copiar el valor algo almacenado en flash a una variable en RAM
 * 			no se pueden igualar, hay que usar esta funcion.
 * @param	post,
 * 		  	data,
 * 		  	data_length,
 * @retval	longitud  de datos leidos.
 *
 */
uint32_t FM_FLASH_Read(uint32_t memory_address, uint8_t *data, uint16_t data_length)
{
  for (int i = 0; i < data_length; i++)
  {
    *(data + i) = (*(uint8_t*) (memory_address + i));
  }

  return data_length;
}

// Interrupts

/*** end of file ***/
