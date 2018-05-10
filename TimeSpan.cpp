/**************************************************************
Home waterpool manager
designed for the ESP8266/Arduino platform (https://github.com/esp8266/Arduino)
Intelligent managing water heating and heater overheat protection
Built by Sergey Korostel
Licensed under MIT license

TimeSpan class for time counting
**************************************************************/
#include "TimeSpan.h"

TimeSpan::TimeSpan(uint8_t days, uint8_t hours, uint8_t minutes, uint8_t seconds) 
{
	_ticks = ((uint32_t)days * TicksPerDay + (uint32_t)hours * TicksPerHour + (uint32_t)minutes * TicksPerMinute + seconds) * TicksPerSecond;
}

void TimeSpan::AddSeconds(uint32_t seconds)
{
	_ticks += seconds * TicksPerSecond;
}

void TimeSpan::AddMinutes(uint32_t minutes)
{
	_ticks += minutes * TicksPerMinute * TicksPerSecond;
}

void TimeSpan::AddHours(uint32_t hours)
{
	_ticks += hours * TicksPerHour * TicksPerMinute * TicksPerSecond;
}

String TimeSpan::ToString()
{
	char datestring[16];
	snprintf_P(datestring,
		sizeof(datestring),
		PSTR("%03u.%02u:%02u:%02u"),
		Days(),
		Hours(),
		Minutes(),
		Seconds());
	return datestring;
}
