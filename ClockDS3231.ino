#include "uRTCLib.h"

#define DS3231_ADDRESS 0x68
#define EE24C_ADDRESS 0x57

uRTCLib rtc(DS3231_ADDRESS, EE24C_ADDRESS);


// Set DateTime from string, given in format YYYYMMDD-HHMMSS
void setDateTime(String dateTimeStr) {
	int year = dateTimeStr.substring(2, 4).toInt();
	int month = dateTimeStr.substring(4, 6).toInt();
	int day = dateTimeStr.substring(6, 8).toInt();

	int hour = dateTimeStr.substring(9, 11).toInt();
	int minute = dateTimeStr.substring(11, 13).toInt();
	int second = dateTimeStr.substring(13, 15).toInt();
	
	byte dow = DayOfWeek(year, month, day);
	//  RTCLib::set(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year)
	rtc.set(second, minute, hour, dow, day, month, year);
}

String getDateTime() {
	char tempString[20];
	rtc.refresh();
	sprintf(tempString, "20%02d%02d%02d-%02d%02d%02d", rtc.year(), rtc.month(), rtc.day(), rtc.hour(), rtc.minute(), rtc.second());
	String result = tempString;
	return result;
}

// Day of week calculation, 1 <= DoW <= 7, with 1 = Sunday
byte DayOfWeek(int y, byte m, byte d) {   // y > 1752, 1 <= m <= 12
	static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
	y -= m < 3;
	return ((y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7) + 1; // 01 - 07, 01 = Sunday
}