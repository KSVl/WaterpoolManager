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
bool waitWhenTemperatureDownToDelta = false; // True means that heater was overheated and we wait when temperature down by given maximumAllowedTempDelta value below maximum allowed 

// Check if the current sensor values is in limits. Then heater can be switched ON
// Parameter heaterIsOn shows that relay is switched ON for now (is used to calculate delta cut off)
bool HeaterCanBeON(bool heaterIsOn)
{
	bool canBeON = false;
	if (heaterEnable)	// In case if heater device is ON, then check sensor limits...
	{
		int maxTemp = -100;
		for (byte t = 0; t <= lastSensorIndex; t++) {
			int tempInCelsius = (tempData[t] + 8) >> 4; // In celsius, with math rounding
			if (maxTemp < tempInCelsius)
				maxTemp = tempInCelsius;
		}
		// This is temperature gap in order to avoid frequent triggering
		bool onAfterOff = true;
		if (waitWhenTemperatureDownToDelta)
			onAfterOff = heaterIsOn || maxTemp < maximumAllowedTemp - maximumAllowedTempDelta;

		// Sensors is found (t > 0) and temperature less than max allowed
		// or temp sensor limits is switched off (max is set to 0)
		bool temperatureInRange = ((maxTemp >= 0 && maxTemp < maximumAllowedTemp) || maximumAllowedTemp == 0);
		// If flow is more or equal of minimum alowed,
		// this include zero (sensor limit is switched off).
		bool flowInRange = (litersInMinute >= minimumAllowedFlow);

		canBeON = (temperatureInRange && flowInRange && onAfterOff);

		if (!temperatureInRange)
			waitWhenTemperatureDownToDelta = true;
		else if (canBeON)
			waitWhenTemperatureDownToDelta = false;
	}
	return canBeON;
}

// Switch Heat relay ON or OFF depends on limit values
void SwitchHeatRelay()
{
	heatingAllowed = HeaterCanBeON(heatingAllowed);

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
		resetVirtualClock();
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

