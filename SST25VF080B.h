#include <stdint.h>
#include <stdbool.h>
typedef enum {
	READ                      = 0x03,
	HIGH_SPEED_READ           = 0x0B, 
	FOUR_KB_SECTOR_ERASE      = 0x20,
	THIRTY_TWO_KB_BLOCK_ERASE = 0x52,
	SIXTY_FOUR_KB_BLOCK_ERASE = 0xD8,
	CHIP_ERASE                = 0x60,
	BYTE_PROGRAM              = 0x02,
	AAI_WORD_PROGRAM          = 0xAD,
	RDSR                      = 0x05,
	EWSR                      = 0x50,
	WRSR                      = 0x01,
	WREN                      = 0x06,
	WRDI                      = 0x04,
	RDID                      = 0x90,
	JEDECID                   = 0x9F,
	EBSY                      = 0x70,
	DBSY                      = 0x80,
	STATUS					  = 0x00,
	
} SS25_Commands;

typedef enum {
	ERR_Okay = 0,
	ERR_Fail,
} SS25_Error;

typedef enum {
	Status_Okay = 0, //TODO
	Status_Error = 1, //TODO
	Status_WriteEnabled = 2,
} SS25_Status;

typedef enum {
	Size_4kb,
	Size_32kb,
	Size_64kb,
	Size_Chip,
} SS25_EraseSize;

SS25_Error SS25_Init();
SS25_Error SS25_Read(uint32_t address, uint8_t* bytes, uint32_t numBytes);
SS25_Error SS25_Write(uint32_t address, uint8_t* bytes, uint32_t numBytes);
SS25_Error SS25_Erase(uint32_t address, SS25_EraseSize sze);
SS25_Error SS25_GetStatus(SS25_Status* status);
SS25_Error SS25_SetWriteEnable(bool doEnable);
SS25_Error SS25_SetSREnable(bool doEnable);
SS25_Error SS25_WriteSR(uint8_t sr);