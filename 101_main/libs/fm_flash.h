/*
 *
 * Author: DHS
 * Version: 1
 * 
 * @brief 
 * 
 * Date: 29/06/2024
 * Name: DHS
 * - Version inicial
 * 
 * 
 */

#ifndef FM_FLASH_H_
#define FM_FLASH_H_

// includes

// Defines.

/*
 * La memoria flash se escribe de a n Bytes, esta catidad en se la llama BLOCK_SIZE, se define
 * cuanto es n para este microcontrolador.
 */
#define FM_FLASH_BLOCK_SIZE  16

/*
 * Informacion del chip se guarda en flash, en lo que sigue se define el tamaño, en bytes,
 * de la estructura que guardara la informacion del chip.
 */
#define FM_FLASH_CHIP_INFO_SIZE (FM_FLASH_BLOCK_SIZE * 1)

// Typedef y enum.

typedef union
{
	uint8_t data[FM_FLASH_CHIP_INFO_SIZE];
	struct
	{
		uint16_t reset_counter;
		uint8_t reset_factory;
		uint32_t reserved_3;
		uint32_t reserved_4;
	};
} flash_chip_info_t;

// Macros, defines, microcontroller pins (dhs).

// Varibles extern

// Function prototypes

flash_chip_info_t FM_FLASH_ChipInfoRead();
void FM_FLASH_ChipInfoWrite(flash_chip_info_t);
uint16_t FM_FLASH_NewReset();
uint32_t FM_FLASH_Read(uint32_t address, uint8_t *data, uint16_t data_length);
uint32_t FM_FLASH_Write(uint32_t pos, uint8_t *data, uint16_t data_length);

#endif /* FM_FLASH_H */

/*** end of file ***/