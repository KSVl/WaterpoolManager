#include <EEPROM.h>

#define SAVE_ADDR_BASE 0
#define SETTINGS_SIZE 4 + TSCOUNT + 2*RELNAMESIZE

void SaveSettings()
{
	EEPROM.begin(SETTINGS_SIZE);
	WriteDataBytes(SAVE_ADDR_BASE + 0, loggingPeriodSeconds, 2);
	WriteDataBytes(SAVE_ADDR_BASE + 2, minimumAllowedFlow, 1);
	for (byte t = 0; t < TSCOUNT; t++)
		WriteDataBytes(SAVE_ADDR_BASE + 3 + t, tempLimits[t], 1);
	WriteString(SAVE_ADDR_BASE + 3 + TSCOUNT, relay3name, RELNAMESIZE);
	WriteString(SAVE_ADDR_BASE + 3 + TSCOUNT + RELNAMESIZE, relay4name, RELNAMESIZE);
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
	ReadString(SAVE_ADDR_BASE + 3 + TSCOUNT, relay3name, RELNAMESIZE);
	ReadString(SAVE_ADDR_BASE + 3 + TSCOUNT + RELNAMESIZE, relay4name, RELNAMESIZE);
	if (relay3name[0] < 0x20)
		relay3name[0] = 0x00;
	if (relay4name[0] < 0x20)
		relay4name[0] = 0x00;
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

void ReadString(int addr, char* buffer, int bufSize) 
{
	if (bufSize == 0) 
		return;
	char ch = 1;
	int charsRead = 0;
	while ((ch != 0x00) && (charsRead < bufSize))
	{
		ch = EEPROM.read(addr + charsRead);
		buffer[charsRead] = ch;
		charsRead++;
	}
	if (ch != 0x00)
		buffer[charsRead - 1] = 0;
}

void WriteString(int addr, char* string, int maxSize)
{
	int charsRead = 0;
	char ch = 0x00;
	do 
	{
		ch = string[charsRead];
		EEPROM.write(addr + charsRead, ch);
		charsRead++;
	} while (ch != 0x00 && charsRead < maxSize);
}

