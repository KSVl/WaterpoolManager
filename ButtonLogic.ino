/**************************************************************
Home waterpool manager
designed for the ESP8266/Arduino platform (https://github.com/esp8266/Arduino)
Intelligent managing water heating and heater overheat protection
Built by Sergey Korostel
Licensed under MIT license

Here is the business logic, which happen after button press
**************************************************************/

#define BUTTON_TEMP_ADJ_INDEX 1
#define BUTTON_FLOW_ADJ_INDEX 2 
#define HEATER_SWITCH_INDEX 0 // The index of pushbutton, which switch heat ON or OFF (should be pressed 3 seconds to switch)

unsigned long prevButtonHeaterSwitchMillis = 0;

bool processButtons()
{
	bool buttonPressed[4]{ false, false, false, false };
//	readButtonStates(buttonPressed);

	bool button_heater_switch = buttonPressed[HEATER_SWITCH_INDEX];
	bool button_temp_adj_value = buttonPressed[BUTTON_TEMP_ADJ_INDEX];
	bool button_flow_adj_value = buttonPressed[BUTTON_FLOW_ADJ_INDEX];

	// Check, if heater switch button pressed
	if (button_heater_switch)
	{
		if (millis() - prevButtonHeaterSwitchMillis > 3000)	// Only long press enable heat mode
			button_heater_switch = false;
		if (prevButtonHeaterSwitchMillis == 0)
			prevButtonHeaterSwitchMillis = millis();	// Start count
	}
	else
		prevButtonHeaterSwitchMillis = 0;
	if (button_heater_switch)
		heaterEnable = !heaterEnable;

	bool saveSettings = false;
	// Check, if setup buttons pressed
	if (button_temp_adj_value)
	{
		saveSettings = true;
		maximumAllowedTemp += 10;
		if (maximumAllowedTemp > 99)
			maximumAllowedTemp = 0;
	}
	if (button_flow_adj_value)
	{
		saveSettings = true;
		minimumAllowedFlow += 5;
		if (minimumAllowedFlow > 99)
			minimumAllowedFlow = 0;
	}
	if (saveSettings)
		SaveSettings();


	char tempString[10];
	if (button_temp_adj_value || button_flow_adj_value)
	{
		printTempLimits();
	}

	return button_temp_adj_value || button_flow_adj_value;
}
