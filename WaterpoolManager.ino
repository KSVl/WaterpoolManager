/**************************************************************
Home waterpool manager
designed for the ESP8266/Arduino platform (https://github.com/esp8266/Arduino) 
Intelligent managing water heating and heater overheat protection
Built by Sergey Korostel
Licensed under MIT license

Main runtime file
**************************************************************/

#include <ESP8266WiFi.h>         // https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager

#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <Ticker.h>

#include <Wire.h>
#include <pcf8574_esp.h>		// https://github.com/WereCatf/PCF8574_ESP

#define FLOW_SENSOR_PIN 12 // The pin number of flow sensor (should able to set extrernal interrupt)
#define TEMP_SENSOR_PIN 14 // The pin number of temperature sensors

// I2C bus default pins
#define PIN_SDA 4
#define PIN_SCL 5
//#define PIN_INT_PCF8574 D5

#define FLOW_SENSOR_CONST 5.3 // The flow sensor const depends on model. Used for the flow rate calculation.
#define TEMP_SENSOR_POWER_MODE 0 // Power mode of DS18B20, 0 - external, 1 - parasite

#define RELAY_PUMP_DELAY 10*60 // Delay, in seconds, when waterflow PUMP may OFF after heat is OFF (allow heater cool down).
#define EEPROM_ADDR_MAXTEMP 2
#define EEPROM_ADDR_MINFLOW 4

// Pins of LCD like 1602A
#define LCD_I2C_ADDR 0b0100010
#define LCD_SIZE_CHARS 16
#define LCD_SIZE_LINES 2

// Current values, acquired from sensors
int litersInMinute = 0;
byte tempSensAddr[4][8];		// Addresses of the first 4 found temperature sensors
short lastSensorIndex = 0;		// Last index of found temperature sensors
int tempData[4] {0, 0, 0, 0};	// Temperatures (raw data from temperature sensor).

// Current state of the device
bool pumpEnable = false;		// User command: Turn ON waterpool recirculation pump
bool heaterEnable = false;		// User command: Turn ON water heating
bool relayHeatON = false;		// Heat relay actual status: ON or OFF
bool relayPumpON = false;		// Pump relay actual status: ON or OFF
bool heatingAllowed = false;	// Heating is allowed becasue of the current sensor values is in limits. 

// The initial limit values (preset ranges where heating can be ON). They can be adjusted and saved to EEPROM
int maximumAllowedTemp = 40;		// Maximum allowed temperature at heater body, degrees C
int maximumAllowedTempDelta = 5;	// After ocverheat, temperature should down by given value of degrees C
int minimumAllowedFlow = 5;			// Minimum allowed water flow in heater, Liters/min

// Class library instances
PCF857x relayPCF8574(0b0100001, &Wire);
LiquidCrystal_I2C lcd(LCD_I2C_ADDR, LCD_SIZE_CHARS, LCD_SIZE_LINES);
std::unique_ptr<ESP8266WebServer> server;

// Startup code
void setup()
{
    Serial.begin(9600);

	Serial.println("Init relay module. Turn them OFF...");

	Wire.pins(PIN_SDA, PIN_SCL);
	Wire.begin();
	Wire.setClock(100000L);  // 100KHz I2C-bus

    //pinMode(PIN_INT_PCF8574, INPUT_PULLUP);
	relayPCF8574.begin(0xF0); //4 pin input (P4-P7), 4 pin output (P0-P3)
	relayPCF8574.resetInterruptPin();

	Serial.println("Start connection to WiFi...");

	//WiFiManager
	//Local intialization. Once its business is done, there is no need to keep it around
	WiFiManager wifiManager;
	//reset saved settings
	//wifiManager.resetSettings();

	//wifiManager.setHostname("WpFlowMeter");	// Only in Dev version for now

	//set custom ip for portal
	//wifiManager.setAPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

	//fetches ssid and pass from eeprom and tries to connect
	//if it does not connect it starts an access point with the specified name
	//here  "WpFlowMeter"m default password is 17050000
	//and goes into a blocking loop awaiting configuration
	wifiManager.autoConnect("WpFlowMeter", "17050000");
	//or use this for auto generated name ESP + ChipID
	//wifiManager.autoConnect();

	Serial.println("Connected to WiFi");
	Serial.println(WiFi.localIP());

	setupWebServer();
	setupLcd();

	lcd.setCursor(0, 0);
	lcd.print("Initialization..");

    byte eeprom_maxtemp = EEPROM.read(EEPROM_ADDR_MAXTEMP);
    byte eeprom_minflow = EEPROM.read(EEPROM_ADDR_MINFLOW);
    if (eeprom_maxtemp < 100)
      maximumAllowedTemp = eeprom_maxtemp;
    if (eeprom_minflow < 100)
      minimumAllowedFlow = eeprom_minflow;
    Serial.print("Max temp. = ");
    Serial.println(maximumAllowedTemp);
    Serial.print("Min flow = ");
    Serial.println(minimumAllowedFlow);

	printPresets();

    // Search for available sensors, fill sensor address array
	searchTempSensors();

    // Read sensor values and print temperatures
	readSensorValues(tempData);

    // Show the init values before cycle start.
	printInitialTemperatureData();
    Serial.println("------");
    delay(1000); // Delay in order that user can see the initialization message
    lcd.clear();

    // Set flow sensor interrupts and variables
	initSensors();
}

// Main loop code
void loop ()
{
	server->handleClient();

	SwitchHeatRelay();
	SwitchPumpRelay();

	onMinuteChangeAction();

	printSensorValues();
	printStatusValues();
    printClock();

	bool anyButtonWasPressed = processButtons();

	// Delay allow other functions to run, like WiFi and TCP/IP libraries
    delay(1000); // Delay between two printing iterations

	if (anyButtonWasPressed)
		lcd.clear();
}
