/**************************************************************
Home waterpool manager
designed for the ESP8266/Arduino platform (https://github.com/esp8266/Arduino)
Intelligent managing water heating and heater overheat protection
Built by Sergey Korostel
Licensed under MIT license

All functions for priting at LiquidCrystal LCD (1602A or same)
**************************************************************/

void setupLcd()
{
	lcd.begin();
	lcd.noBacklight();  // Backlight signal is reversed in my hardware
	lcd.setCursor(0, 0);
}

void printPresets()
{
	lcd.setCursor(0, 1);
	lcd.print("f");
	lcd.setCursor(1, 1);
	lcd.print(minimumAllowedFlow);

	lcd.setCursor(4, 1);
	lcd.print("t");
	for (byte t = 0; t < TSCOUNT; t++)
	{
		lcd.setCursor(5+t*3, 1);
		lcd.print(tempLimits[t]);
	}
}

void printTempLimits()
{
	char tempString[10];
	lcd.clear();
	sprintf(tempString, "max temp1 %2d", tempLimits[0]); // Print with leading spaces
	lcd.setCursor(0, 0);
	lcd.print(tempString);
	sprintf(tempString, "min flow %2d", minimumAllowedFlow); // Print with leading spaces
	lcd.setCursor(0, 1);
	lcd.print(tempString);
}

void printSensorValues()
{
	// Print all values to console
	for (byte t = 0; t <= lastSensorIndex; t++) {
		int tempInCelsius = (tempData[t] + 8) >> 4; // In celsius, with math rounding
		Serial.print(tempInCelsius, DEC); // Print temperature
		Serial.println(" C");
	}
	Serial.print(litersInMinute, DEC); // Print litres/minute
	Serial.println(" L/min");

	// Print all values at LCD
	byte cursorPos = 0;
	char tempString[10];
	lcd.setCursor(cursorPos, 0);
	char sign = litersInMinute < minimumAllowedFlow ? ' ' : '!';
	int len = sprintf(tempString, "%03d%c", litersInMinute, sign); // Print with leading spaces
	lcd.print(tempString);
	cursorPos += len;
	for (byte t = 0; t <= lastSensorIndex; t++)
	{
		int tempInCelsius = (tempData[t] + 8) >> 4; // In celsius, with math rounding
		sign = tempInRange[t] ? ' ' : '!';
		len = sprintf(tempString, "%2d%c", tempInCelsius, sign); // Print with leading spaces
		lcd.setCursor(cursorPos, 0);
		lcd.print(tempString);
		cursorPos += len;
	}
}

void printSensorLimits()
{
	// Print limit values at LCD
	byte cursorPos = 0;
	char tempString[10];
	lcd.setCursor(cursorPos, 1);
	int len = sprintf(tempString, "%03d ", minimumAllowedFlow); // Print with leading spaces
	lcd.print(tempString);
	cursorPos += len;
	for (byte t = 0; t < TSCOUNT; t++)
	{
		len = sprintf(tempString, "%2d ", tempLimits[t]); // Print with leading spaces
		lcd.setCursor(cursorPos, 1);
		lcd.print(tempString);
		cursorPos += len;
	}
}

void printStatusValues()
{
	lcd.setCursor(0, 1);
	if (heaterEnable)
		lcd.print("H");		// Heater device is in HEAT mode
	else
		lcd.print(" ");

	lcd.setCursor(1, 1);
	if (relayHeatON)
		lcd.print("+");		// Display: heat relay is ON
	else
		lcd.print(" ");

	lcd.setCursor(2, 1);
	if (pumpEnable)
		lcd.print("P");		// Pump is enabled
	else
		lcd.print(" ");

	lcd.setCursor(3, 1);
	if (relayPumpON)
		lcd.print("+");		// Display: pump relay is ON
	else
		lcd.print(" ");
}

void printClock()
{
	lcd.setCursor(4, 1);
	lcd.print(heatStatusChangeTimeSpan.ToString());
}

// Show the init temp. sensor values and last bytes of address before cycle start.
void printInitialTemperatureData()
{
	lcd.clear();
	byte cursorPos = 0;
	char tempString[10];
	for (byte addr = 0; addr <= lastSensorIndex; addr++) {
		int tempInCelsius = (tempData[addr] + 8) >> 4; // In celsius, with math rounding
		Serial.print(tempInCelsius, DEC); // Print temperature
		Serial.println(" C");
		int len = sprintf(tempString, "%2d", tempInCelsius); // Print with leading spaces
		lcd.setCursor(cursorPos, 0);
		lcd.print(tempString);
		sprintf(tempString, "%02X", tempSensAddr[addr][7]); // Print last digit of sensor address
		lcd.setCursor(cursorPos, 1);
		lcd.print(tempString);
		cursorPos += len + 1;
	}
}
