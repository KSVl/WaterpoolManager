/**************************************************************
Home waterpool manager
designed for the ESP8266/Arduino platform (https://github.com/esp8266/Arduino)
Intelligent managing water heating and heater overheat protection
Built by Sergey Korostel
Licensed under MIT license

Functions, working with realtime clock & timers
**************************************************************/

#include <Wire.h> 
#include <RtcDS3231.h>
//#define USE_SOFTWARE_CLOCK	// Uncomment if DS3231 is not present

RtcDS3231<TwoWire> Rtc(Wire);

RtcDateTime currentDateTime(0);
TimeSpan heatStatusChangeTimeSpan(0);	// Count the time passed from the heater change status (On/Off)

// Set DateTime from string, given in format YYYYMMDD-HHMMSS
void setDateTime(String dateTimeStr) 
{
	int year = dateTimeStr.substring(0, 4).toInt();
	int month = dateTimeStr.substring(4, 6).toInt();
	int day = dateTimeStr.substring(6, 8).toInt();

	int hour = dateTimeStr.substring(9, 11).toInt();
	int minute = dateTimeStr.substring(11, 13).toInt();
	int second = dateTimeStr.substring(13, 15).toInt();
	
	RtcDateTime dt = RtcDateTime(year, month, day, hour, minute, second);
	Rtc.SetDateTime(dt);
}

String getDateTime() {
	char datestring[20];

	RtcDateTime dt = Rtc.GetDateTime();

	snprintf_P(datestring,
		sizeof(datestring),
		PSTR("%04u%02u%02u-%02u%02u%02u"),
		dt.Year(),
		dt.Month(),
		dt.Day(),
		dt.Hour(),
		dt.Minute(),
		dt.Second());

	String result = datestring;
	return result;
}

void incrementClock()
{
#ifdef USE_SOFTWARE_CLOCK
	currentDateTime += 1;	// Add one second
	heatStatusChangeTimeSpan.AddSeconds(1);
#endif
}

#ifndef USE_SOFTWARE_CLOCK
uint32_t prevCurrentTimeTotalSeconds = 0;
#endif
void updateClock()
{
#ifndef USE_SOFTWARE_CLOCK
	currentDateTime = Rtc.GetDateTime();
	uint32_t currentTimeTotalSeconds = currentDateTime.TotalSeconds();
	if (prevCurrentTimeTotalSeconds > 0)
		heatStatusChangeTimeSpan.AddSeconds(currentTimeTotalSeconds - prevCurrentTimeTotalSeconds);
	prevCurrentTimeTotalSeconds = currentTimeTotalSeconds;
#endif
}

void resetTimer()
{
	heatStatusChangeTimeSpan = 0;
}

bool secondReminder(byte sec)
{
	return currentDateTime.Second() % sec == 0;
}
