/**************************************************************
Home waterpool manager
designed for the ESP8266/Arduino platform (https://github.com/esp8266/Arduino)
Intelligent managing water heating and heater overheat protection
Built by Sergey Korostel
Licensed under MIT license

All functions for working with sensors: flow meter hall sensor and dallas temperature sensors (1Wire bus, DS18B20)
**************************************************************/

#define TEMP_MEASURE_PERIOD 20 // Time of measuring, * TEMP_TIMER_PERIODICITY ms
#define TEMP_TIMER_PERIODICITY 50 // Periodicity of timer calling, ms

OneWire ds(TEMP_SENSOR_PIN);
Ticker timer;

void initSensors()
{
	// Set flow sensor interrupts and variables
	pinMode(FLOW_SENSOR_PIN, INPUT);
	attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), flow, RISING); // Setup Interrupt

	timer.attach_ms(TEMP_TIMER_PERIODICITY, tempReadTimer); // 10ms period 
}


void searchTempSensors()
{
	// Search for available sensors, fill sensor address array
	while (ds.search(tempSensAddr[lastSensorIndex]) && lastSensorIndex < 4)
	{
		Serial.print("ROM =");
		for (byte i = 0; i < 8; i++) {
			Serial.print(' ');
			Serial.print(tempSensAddr[lastSensorIndex][i], HEX);
		}
		if (OneWire::crc8(tempSensAddr[lastSensorIndex], 7) != tempSensAddr[lastSensorIndex][7]) {
			Serial.print(" CRC is not valid!");
		}
		else
			lastSensorIndex++;
		Serial.println();
	}
	ds.reset_search();
	lastSensorIndex--;
	Serial.print("\r\nTemperature sensor count: ");
	Serial.print(lastSensorIndex + 1, DEC);
	Serial.println("\r\n");
}

// Read temperature sensor values in blocking mode (used at start to get initial values)
void readSensorValues(int tempData[])
{
	// Read sensor values and print temperatures
	ds.reset();
	ds.write(0xCC, TEMP_SENSOR_POWER_MODE); // Request all sensors at the one time
	ds.write(0x44, TEMP_SENSOR_POWER_MODE); // Acquire temperatures
	delay(1000); // Delay is required by temp. sensors

	char tempString[10];
	for (byte addr = 0; addr <= lastSensorIndex; addr++) {
		ds.reset();
		ds.select(tempSensAddr[addr]);
		ds.write(0xBE, TEMP_SENSOR_POWER_MODE); // Read Scratchpad
		tempData[addr] = ds.read() | (ds.read() << 8); // Read first 2 bytes which carry temperature data
		int tempInCelsius = (tempData[addr] + 8) >> 4; // In celsius, with math rounding
		Serial.print(tempInCelsius, DEC); // Print temperature
		Serial.println(" C");
	}
}

volatile int flow_frequency; // Flow sensor pulses
int tempMeasureCycleCount = 0;
int flowMeasureCycleCount = 0;

void flow() // Flow sensor interrupt function
{
	flow_frequency++;
}

void calcFlowTimer() // It called every second, calculates litres/minute
{
	litersInMinute = (flow_frequency / FLOW_SENSOR_CONST); // Pulse frequency (Hz) = FLOW_SENSOR_CONST*Q, Q is flow rate in L/min.
	flow_frequency = 0; // Reset Counter
}

void tempReadTimer() // Called many times in second, perform only one small operation per call
{
	flowMeasureCycleCount++;
	if (flowMeasureCycleCount >= 1000 / TEMP_TIMER_PERIODICITY)
	{
		flowMeasureCycleCount = 0;
		calcFlowTimer();
		incrementClock();
	}

	tempMeasureCycleCount++;
	if (tempMeasureCycleCount >= TEMP_MEASURE_PERIOD) {
		tempMeasureCycleCount = 0; // Start cycle again
	}

	if (tempMeasureCycleCount == 0)
	{
		ds.reset();
		ds.write(0xCC, TEMP_SENSOR_POWER_MODE); // Request all sensors at the one time
		ds.write(0x44, TEMP_SENSOR_POWER_MODE); // Acquire temperatures
	}
	// Between phases above and below should be > 750 ms
	int addr = TEMP_MEASURE_PERIOD - tempMeasureCycleCount - 1;
	if (addr >= 0 && addr <= lastSensorIndex)
	{
		ds.reset();
		ds.select(tempSensAddr[addr]);
		ds.write(0xBE, TEMP_SENSOR_POWER_MODE); // Read Scratchpad
		tempData[addr] = ds.read() | (ds.read() << 8); // Read first 2 bytes which carry temperature data
	}
}