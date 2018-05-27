#include <EEPROM.h>

#define SAVE_ADDR_BASE 0
#define SETTINGS_SIZE 4 + TSCOUNT

void SaveSettings()
{
	EEPROM.begin(SETTINGS_SIZE);
	WriteDataBytes(SAVE_ADDR_BASE + 0, loggingPeriodSeconds, 2);
	WriteDataBytes(SAVE_ADDR_BASE + 2, minimumAllowedFlow, 1);
	for (byte t = 0; t < TSCOUNT; t++)
		WriteDataBytes(SAVE_ADDR_BASE + 3 + t, tempLimits[t], 1);
	EEPROM.commit();
}

void LoadSettings()
{
	EEPROM.begin(SETTINGS_SIZE);
	loggingPeriodSeconds = ReadDataBytes(SAVE_ADDR_BASE + 0, 2);
	byte eeprom_minflow = ReadDataBytes(SAVE_ADDR_BASE + 2, 1);
	if (eeprom_minflow < 100)
		minimumAllowedFlow = eeprom_minflow;
	for (byte t = 0; t < TSCOUNT; t++)
	{
		byte eeprom_maxtemp = ReadDataBytes(SAVE_ADDR_BASE + 3 + t, 1);
		if (eeprom_maxtemp < 100)
			tempLimits[t] = eeprom_maxtemp;
	}
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
