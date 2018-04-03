/**************************************************************
Home waterpool manager
designed for the ESP8266/Arduino platform (https://github.com/esp8266/Arduino)
Intelligent managing water heating and heater overheat protection
Built by Sergey Korostel
Licensed under MIT license

Functions for managing relays, connected to PCF8574
**************************************************************/

#define RELAY_HEAT_INDEX 1  // PCF8574 pin, where the switch relay is connected (turn ON/OFF heating boiler)
#define RELAY_PUMP_INDEX 0  // PCF8574 pin, where the switch relay is connected (turn ON/OFF waterpool pump)
#define RELAY_MASK 0xF0		// The mask of relay outputs, all the rest [4..7] are inputs (buttons)

void PumpRelayON()
{
	RelayOn(RELAY_PUMP_INDEX);
}

void PumpRelayOFF()
{
	RelayOff(RELAY_PUMP_INDEX);
}

void HeatRelayON()
{
	RelayOn(RELAY_HEAT_INDEX);
}

void HeatRelayOFF()
{
	RelayOff(RELAY_HEAT_INDEX);
}

// PCF8574 relay management
// num - the realy index at PCF8574 output [0..3]
void RelayOn(byte num) {
	byte b = relayPCF8574.read8();
	b = b | (RELAY_MASK | (1 << num));
	relayPCF8574.write8(b);
}

void RelayOff(byte num) {
	byte b = relayPCF8574.read8();
	b = b & (~(1 << num));
	relayPCF8574.write8(RELAY_MASK | b);
}

void AllRelaysOn() {
	relayPCF8574.write8(0xFF);
}

void AllRelaysOff () {
	relayPCF8574.write8(RELAY_MASK | 0);
}