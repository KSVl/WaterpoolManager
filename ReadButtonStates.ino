/**************************************************************
Home waterpool manager
designed for the ESP8266/Arduino platform (https://github.com/esp8266/Arduino)
Intelligent managing water heating and heater overheat protection
Built by Sergey Korostel
Licensed under MIT license

Functions for reading physical button states, connected to PCF8574
**************************************************************/

void readButtonStates(bool buttonPressed[])
{
	byte bi;
	for (byte bi = 0; bi <= 4; bi++) 
		buttonPressed[bi] = false;

	// Read button states via PCF8574 port extender
	// Inputs should pull-up to Vcc by 10k resistors, button should put signal to ground
	//	if (digitalRead(PIN_INT) == LOW) 
	{
		byte b = relayPCF8574.read8();
		Serial.println("PCF8574 INT: " + String(b));

		byte keys = ((~b) >> 4) & 0x0F;
		if (CheckKey(keys, 0)) {
			Serial.println("KEY 0 Pressed");
			buttonPressed[0] = true;
		}
		if (CheckKey(keys, 1)) {
			Serial.println("KEY 1 Pressed");
			buttonPressed[1] = true;
		}
		if (CheckKey(keys, 2)) {
			Serial.println("KEY 2 Pressed");
			buttonPressed[2] = true;
		}
		if (CheckKey(keys, 3)) {
			Serial.println("KEY 3 Pressed");
			buttonPressed[3] = true;
		}
	}
}

bool CheckKey(byte key, byte num) { //0, 1, 2, 3
	return key & (1 << num);
}
