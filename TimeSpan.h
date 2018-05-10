/**************************************************************
Home waterpool manager
designed for the ESP8266/Arduino platform (https://github.com/esp8266/Arduino)
Intelligent managing water heating and heater overheat protection
Built by Sergey Korostel
Licensed under MIT license

TimeSpan class for time counting
**************************************************************/
#ifndef _TIMESPAN_h
#define _TIMESPAN_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

const uint32_t TicksPerSecond = 100;		// 1/100 of second allows save about 490 days in TimeSpan
const uint32_t TicksPerMinute = TicksPerSecond * 60;
const uint32_t TicksPerHour = TicksPerMinute * 60;
const uint32_t TicksPerDay = TicksPerHour * 24;

class TimeSpan
{
public:
	TimeSpan(uint32_t ticks = 0)
	{
		_ticks = ticks;
	}
	TimeSpan(uint8_t days, uint8_t hours, uint8_t minutes, uint8_t seconds);

	operator uint32_t() const
	{
		return _ticks;
	}

	uint8_t Seconds() const
	{
		return (_ticks / TicksPerSecond) % 60;
	}

	uint8_t Minutes() const
	{
		return (_ticks / TicksPerMinute) % 60;
	}

	uint8_t Hours() const
	{
		return (_ticks / TicksPerHour) % 24;
	}

	uint8_t Days() const
	{
		return _ticks / TicksPerDay;
	}

	void AddSeconds(uint32_t seconds);
	void AddMinutes(uint32_t minutes);
	void AddHours(uint32_t hours);

	uint32_t TotalSeconds() const
	{
		return _ticks / TicksPerSecond;
	}

	String ToString();

protected:
	uint32_t _ticks;
};

#endif _TIMESPAN_h
