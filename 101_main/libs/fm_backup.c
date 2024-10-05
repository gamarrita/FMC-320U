/*
 *
 * Author: DHS
 * Version:  1.1
 * 
 * @brief Uso de la BACKUP RAM
 *
 * La BACKUP RAM, es un bolque de memoria ram que se puede mantener alimentado con una bateria
 * de backup, tipicamente una tipo boton, es una bateria de respaldo que se usa para este motivo
 * y para mantener el conteo del reloj de tiempo real.
 *
 * El cube MX tiene la herramienta "Memory Managment" que ayuda a configurar la BACKUP RAM. Al usar
 * el memoru magnamet se modifica el archivos ...._FLASH.ld, este contiene un scrip que configura
 * nomrbres y comportamiento de los bloques de memoria. El memory managment no realiza cambios en el
 * ..._FLASH.ld a menos que tildemos dos opciones en "Code generetion configuration", que son:
 * 	> Apply application regions settings to peipheral: ON
 * 	> Apply application regions settings to linkers fines: ON
 *
 * 	El memory managment tiene un problema y es que no termina de configura correctamente el bloque
 *
 * .RAM_BACKUP_Section(NOLOAD) :
 * {
 *   . = ALIGN(4);
 *   KEEP (*(.RAM_BACKUP_Section))
 *   . = ALIGN(4);
 * } >RAM_BACKUP
 *
 *
 * El "(NOLOAD)" es necesario para que no obtengamos un error en tiempo deprogramacion, el memory
 * mangment no incluye el parametro (NOLOAD). Se agrea por el programador. Esto tiene la implicancia
 * de que si las opciones de "Code generation congiguration" las dejamos en ON, cada vez que
 * modificamos algo en MX, tenemos que volver a editar el archivo ..._FLASH.ld, para agregar
 * (NOLOAD) como paramtro.
 *
 * Ademas del bloque que se describio arriba se tiene el siguiente bloque, donde se ve el
 * mapeo de cada una de las secciones de memoria.
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
 * Versión 1
 * Autor: Daniel H Sagarra
 * Fecha: 08/09/2024
 * Modificaciones: version inicial.
 * 
 */

// Includes.
#include "main.h"
#include "fm_debug.h"

// Typedef.

// Const data.

// Defines.

// Debug.

// Project variables, non-static, at least used in other file.

// External variables.

// Global variables, statics.

// Private function prototypes.

// Private function bodies.

// Public function bodies.

/*
 * @brief	Prepara BACKUP SRAM para ser ledida/escrita y  retener en standby y VBAT mode.
 * @note
 *
 * @param
 * @ret
 *
 *
 */
void FM_BACKUP_Init()
{
	// Habilito el acceso a la lectura/escritura de la RAM BACKUP
	__HAL_RCC_PWR_CLK_ENABLE();
	HAL_PWR_EnableBkUpAccess();
	__HAL_RCC_BKPSRAM_CLK_ENABLE();

	// Encieno regulador para que retenga memoria en standby y VBAT mode.
	HAL_PWREx_EnableBkupRAMRetention();
}

// Interrupts

/*** end of file ***/
