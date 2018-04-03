/**************************************************************
Home waterpool manager
designed for the ESP8266/Arduino platform (https://github.com/esp8266/Arduino)
Intelligent managing water heating and heater overheat protection
Built by Sergey Korostel
Licensed under MIT license

Functions, working with clock, counting time from heater start
**************************************************************/

int sec = 0, mins = 0, hrs = 0, days = 0;
int prevMins = -1, currMins = 0;

void incrementClock()
{
	sec++;
	if (sec > 59)
	{
		sec = 0;
		mins++;
	}
	if (mins > 59)
	{
		mins = 0;
		hrs++;
	}
	if (hrs > 23)
	{
		hrs = 0;
		days++;
	}
}

void resetVirtualClock()
{
	sec = 0; mins = 0; hrs = 0; days = 0;
}

void onMinuteChangeAction()
{
	currMins = mins;
	if (prevMins != currMins)
		printOnMinuteChange();
	prevMins = currMins;
}


