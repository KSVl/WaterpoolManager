/**************************************************************
Home waterpool manager
designed for the ESP8266/Arduino platform (https://github.com/esp8266/Arduino)
Intelligent managing water heating and heater overheat protection
Built by Sergey Korostel
Licensed under MIT license

Functions, for logging to EEPROM
**************************************************************/

#include "EepromLogger.h"
#include <Wire.h>

#define EE24C_ADDRESS 0x50
#define EE24C_SIZE 32767

unsigned int loggingPeriodSeconds = 0;	// Logging period in seconds, if 0 - logging is turned off

#pragma pack(push, 1) 
struct LogEvent 
{
	unsigned char litersInMinute = 0;
	unsigned char tempCelsius[4]{ 0, 0, 0, 0 };
	unsigned char deviceStatus = 0;
}
logEvent;
#pragma pack(pop)

EepromLogger logger(&writer, &reader, &getCurrentTimestamp);

void InitLogger()
{
	if (loggingPeriodSeconds > 0)
	{
		unsigned int logRecordsInBlock = 60 * 60 / loggingPeriodSeconds;	// 1 block for hour
		while (logRecordsInBlock > 1000)	// If too large block, one block per half/hour or even smaller
			logRecordsInBlock = logRecordsInBlock / 2;

		logger.initialize(0, EE24C_SIZE-1,
			logRecordsInBlock, sizeof(LogEvent), true);
	}
}

uint32_t lastEventSeq = 0;
void WriteLogEvent()
{
	if (loggingPeriodSeconds > 0)
	{
		uint32_t seconds = currentDateTime.TotalSeconds();
		uint32_t eventSeq = seconds / loggingPeriodSeconds;
		if (eventSeq > lastEventSeq)	// Write event log only with defined periodicity
		{
			logEvent.litersInMinute = litersInMinute;
			char i;
			for (i = 0; i < 4; i++)
				logEvent.tempCelsius[i] = (tempData[i] + 8) >> 4;;
			logEvent.deviceStatus = pumpEnable | heaterEnable << 1 | relayHeatON << 2 | relayPumpON << 3 | heatingAllowed << 4;
			logger.writeNextRecord((unsigned char*)&logEvent);

			lastEventSeq = eventSeq;
		}
	}
}

void writer(unsigned long address, unsigned char data)
{
	yield();
	unsigned int ee_addr = address;
	Wire.beginTransmission(EE24C_ADDRESS);
	Wire.write((int)(ee_addr >> 8));	// MSB
	Wire.write((int)(ee_addr & 0xFF));  // LSB
	Wire.write(data);
	yield();
	delay(5); 
	bool sucess = Wire.endTransmission() == 0;
}

unsigned char reader(unsigned long address)
{
	yield();
	unsigned int ee_addr = address;
	unsigned char rdata = 0xFF;
	Wire.beginTransmission(EE24C_ADDRESS);
	Wire.write((int)(ee_addr >> 8));	// MSB
	Wire.write((int)(ee_addr & 0xFF));	// LSB
	if (Wire.endTransmission() == 0) {
		Wire.requestFrom(EE24C_ADDRESS, 1);
		if (Wire.available()) {
			rdata = (unsigned char)Wire.read();
		}
	}
	yield();
	return rdata;
}

uint32_t getCurrentTimestamp()
{
	return currentDateTime.Epoch32Time();
}