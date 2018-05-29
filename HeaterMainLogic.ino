/**************************************************************
Home waterpool manager
designed for the ESP8266/Arduino platform (https://github.com/esp8266/Arduino)
Intelligent managing water heating and heater overheat protection
Built by Sergey Korostel
Licensed under MIT license

Main business logic which checks that the sensor values is in limits and allow relay switch heating or pumps.
**************************************************************/

// Variables
unsigned int timePassedAfterHeatOff = 0;			 // Time passed after heater was switched off, millis

// Check if the current sensor values is in limits. Then heater can be switched ON
bool HeaterCanBeON()
{
	bool canBeON = false;
	if (heaterEnable)	// In case if heater device is ON, then check sensor limits...
	{
		int tempInCelsius[]{ 0, 0, 0, 0 };
		for (byte t = 0; t <= lastSensorIndex; t++) {
			tempInCelsius[t] = (tempData[t] + 8) >> 4; // In celsius, with math rounding
		}

		// Sensors is found and temperature less than max allowed
		// or temp sensor limits is switched off (max is set to 0)
		bool temperatureInRanges = lastSensorIndex >= 0;
		for (byte t = 0; t <= lastSensorIndex; t++)
		{
			// Once temperature gone over the bounds, then wait till temperature goes down to some delta value (avoid switch bouncing).
			int delta = tempInRange[t] ? 0 : maximumAllowedTempDelta;  
			tempInRange[t] = tempLimits[t] == 0 || (tempInCelsius[t] >= 0 && tempInCelsius[t] <= tempLimits[t] - delta);
			temperatureInRanges = temperatureInRanges && tempInRange[t];
		}

		// If flow is more or equal of minimum alowed,
		// this include zero (sensor limit is switched off).
		bool flowInRange = (litersInMinute >= minimumAllowedFlow);

		canBeON = (temperatureInRanges && flowInRange);
	}
	return canBeON;
}

void resetSensorLimitRanges()
{
	for (byte t = 0; t < TSCOUNT; t++)
		tempInRange[t] = true;
}

// Switch Heat relay ON or OFF depends on limit values
void SwitchHeatRelay()
{
	heatingAllowed = HeaterCanBeON();

	bool prevRelayHeatON = relayHeatON;
	relayHeatON = heatingAllowed && heaterEnable;
	bool relayStateChanged = relayHeatON != prevRelayHeatON;

	if (relayStateChanged)
	{
		if (relayHeatON)
		{
			SwitchPumpRelay();
			HeatRelayON();
		}
		else
		{
			HeatRelayOFF();
			timePassedAfterHeatOff = millis();
			if (!pumpEnable)
			{
				// ToDo: start timer to switch off pump relay after specified RELAY_PUMP_DELAY
			}
		}
	}

	if (relayStateChanged)
		resetTimer();
}

// Switch Pump relay ON or OFF depends on heater relay status and pump user pereference
void SwitchPumpRelay()
{
	bool prevRelayPumpON = relayPumpON;
	relayPumpON = pumpEnable;

	// Pump relay shpould always work when the heating is ON and some time after that.
	if (relayHeatON)
		relayPumpON = true;
	else
	{
		// If heating is OFF, then we check how long it was switched off. If just switched off, then we left pump ON for the specified time.
		if (millis() - timePassedAfterHeatOff < RELAY_PUMP_DELAY * 1000)
			relayPumpON = true;
	}

	bool relayStateChanged = relayPumpON != prevRelayPumpON;

	if (relayStateChanged)
	{
		if (relayPumpON)
			PumpRelayON();
		else
			PumpRelayOFF();
	}
}

bool prevRelay3ON = !relay3ON;
bool prevRelay4ON = !relay4ON;
void SwitchAdditionalRelays()
{
	if (prevRelay3ON != relay3ON)
	{
		if (relay3ON)
			RelayOn(3 - 1);
		else
			RelayOff(3 - 1);
		prevRelay3ON = relay3ON;
	}
	if (prevRelay4ON != relay4ON)
	{
		if (relay4ON)
			RelayOn(4 - 1);
		else
			RelayOff(4 - 1);
		prevRelay4ON = relay4ON;
	}
}

