#include "SST25VF080B.h"
#include <string.h>
#include <stdio.h>

extern int SPI_Write(uint8_t* bytes, uint32_t len);
extern int SPI_Read(uint8_t* bytes, uint32_t len);
extern int SPI_Transfer(uint8_t* writeBytes, uint8_t* readBytes, uint32_t len);
extern void Delay_ms(uint32_t ms);
static bool isWriteEnabled = false;

static inline void AdaptAddress(uint32_t address, uint8_t* target)
{
	target[0] = (uint8_t)(address >> 16);
	target[1] = (uint8_t)(address >> 8);
	target[2] = (uint8_t)(address);
}

static inline void WaitForOkay()
{
	SS25_Status status = Status_Okay;
	while (true) {
		SS25_GetStatus(&status);
		if (((uint8_t)status & 0x01) == Status_Okay) {
			return;
		}
		Delay_ms(1);
	}
}

SS25_Error SS25_Init()
{
	//TODO no initialization necessary?
	return ERR_Okay;
}

SS25_Error SS25_Read(uint32_t address, uint8_t* bytes, uint32_t numBytes)
{
	//TODO probably poorly sized buffer. 
	uint8_t writeBuffer[64 + 4] = { 0 };
	writeBuffer[0] = READ;
	AdaptAddress(address, &writeBuffer[1]);
	SPI_Transfer(writeBuffer, bytes, numBytes + 4);
	return ERR_Okay;
}

SS25_Error SS25_Write(uint32_t address, uint8_t* bytes, uint32_t numBytes)
{
	SS25_Status stat;
	if (!isWriteEnabled) {
		SS25_SetWriteEnable(true);
		isWriteEnabled = true;
	}
	SS25_GetStatus(&stat);
	if (stat != Status_WriteEnabled) {
		return ERR_Fail;
	}
	
	//Single byte
	if (numBytes == 1)
	{
		uint8_t writeBuffer[5] = { 0, 0, 0, 0, 0 };
		writeBuffer[0] = BYTE_PROGRAM;
		AdaptAddress(address, &writeBuffer[1]);
		writeBuffer[4] = bytes[0];
		SPI_Write(writeBuffer, sizeof(writeBuffer));
	}
	else
	{
		uint8_t writeBuffer[64 + 4] = { 0 };
		writeBuffer[0] = AAI_WORD_PROGRAM;
		AdaptAddress(address, &writeBuffer[1]);
		//TODO super unsafe:
		memcpy(&writeBuffer[4], bytes, numBytes);
		SPI_Write(writeBuffer, numBytes + 4);
	}
	WaitForOkay();
	return ERR_Okay;
}

SS25_Error SS25_Erase(uint32_t address, SS25_EraseSize sze)
{
	
	uint8_t writeBuffer[4] = { 0,0,0,0 };
	uint8_t writeSize = 0;
	switch (sze)
	{
	case Size_4kb:
		{
			writeBuffer[0] = FOUR_KB_SECTOR_ERASE;
			writeSize = sizeof(writeBuffer);
			break;
		}
	case Size_32kb:
		{
			writeBuffer[0] = THIRTY_TWO_KB_BLOCK_ERASE;
			writeSize = sizeof(writeBuffer);
			break;
		}
	case Size_64kb:
		{
			writeBuffer[0] = SIXTY_FOUR_KB_BLOCK_ERASE;
			writeSize = sizeof(writeBuffer);
			break;
		}
	case Size_Chip:
		{
			writeBuffer[0] = CHIP_ERASE;
			//Special case - only writes one byte as no address is necessary.
			writeSize = 1;
			break;
		}
	default:
		return ERR_Fail;
		break;
	}
	AdaptAddress(address, &writeBuffer[1]);
	
	uint32_t bytes = SPI_Write(writeBuffer, writeSize);
	WaitForOkay();
	
	if (bytes == writeSize)
	{
		return ERR_Okay;
	}
	else
	{
		return ERR_Fail;
	}
}

SS25_Error SS25_GetStatus(SS25_Status* status)
{
	uint8_t writeBuffer[2] = { RDSR, 0 };
	uint8_t readBuffer[2] = { 0, 0 };
	uint32_t bytes = SPI_Transfer(writeBuffer, readBuffer, sizeof(writeBuffer));
	*status = (SS25_Status)readBuffer[1]; //TODO confirm this is the right byte.
	if (bytes == sizeof(writeBuffer))
	{
		return ERR_Okay;
	}
	else
	{
		return ERR_Fail;
	}
}

SS25_Error SS25_SetWriteEnable(bool doEnable)
{
	uint8_t writeBuffer = 0;
	if (doEnable)
	{
		writeBuffer = (uint8_t)WREN;
	}
	else
	{
		writeBuffer = (uint8_t)WRDI;
	}
	uint32_t bytes = SPI_Write(&writeBuffer, sizeof(writeBuffer));
	if (bytes == sizeof(writeBuffer))
	{
		return ERR_Okay;
	}
	else
	{
		return ERR_Fail;
	}
}

SS25_Error SS25_SetSREnable(bool doEnable)
{
	uint8_t writeBuffer = 0;
	if (doEnable)
	{
		writeBuffer = EWSR;
	}
	else
	{
		//TOD Not implemented yet.
		return ERR_Fail;
	}
	uint32_t bytes = SPI_Write(&writeBuffer, sizeof(writeBuffer));
	if (bytes == sizeof(writeBuffer))
	{
		return ERR_Okay;
	}
	else
	{
		return ERR_Fail;
	}
}

SS25_Error SS25_WriteSR(uint8_t sr)
{
	uint8_t writeBuffer[2] = { WRSR, sr };
	uint32_t bytes = SPI_Write(writeBuffer, sizeof(writeBuffer));
	if (bytes == sizeof(writeBuffer))
	{
		return ERR_Okay;
	}
	else
	{
		return ERR_Fail;
	}
}