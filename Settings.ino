#include <EEPROM.h>

#define SAVE_ADDR_BASE 0
#define SETTINGS_SIZE 4

void SaveSettings()
{
	EEPROM.begin(SETTINGS_SIZE);
	WriteDataBytes(SAVE_ADDR_BASE + 0, maximumAllowedTemp, 1);
	WriteDataBytes(SAVE_ADDR_BASE + 1, minimumAllowedFlow, 1);
	WriteDataBytes(SAVE_ADDR_BASE + 2, loggingPeriodSeconds, 2);
	EEPROM.commit();
}

void LoadSettings()
{
	EEPROM.begin(SETTINGS_SIZE);
	byte eeprom_maxtemp = ReadDataBytes(SAVE_ADDR_BASE + 0, 1);
	byte eeprom_minflow = ReadDataBytes(SAVE_ADDR_BASE + 1, 1);
	if (eeprom_maxtemp < 100)
		maximumAllowedTemp = eeprom_maxtemp;
	if (eeprom_minflow < 100)
		minimumAllowedFlow = eeprom_minflow;
	loggingPeriodSeconds = ReadDataBytes(SAVE_ADDR_BASE + 2, 2);
}


void WriteDataBytes(u16 WriteAddr, u32 DataToWrite, u8 NumOfBytes)
{
	u8 t;
	for (t = 0; t < NumOfBytes; t++)
	{
		EEPROM.write(WriteAddr + t, (DataToWrite >> (8 * t)) & 0xff);
	}
}

u32 ReadDataBytes(u16 ReadAddr, u8 NumOfBytes)
{
	u8 t;
	u32 temp = 0;
	for (t = 0; t < NumOfBytes; t++)
	{
		temp <<= 8;
		temp += EEPROM.read(ReadAddr + NumOfBytes - t - 1);
	}
	return temp;
}
