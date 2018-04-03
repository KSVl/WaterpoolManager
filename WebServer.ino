/**************************************************************
Home waterpool manager
designed for the ESP8266/Arduino platform (https://github.com/esp8266/Arduino)
Intelligent managing water heating and heater overheat protection
Built by Sergey Korostel
Licensed under MIT license

HTTP Web Server functions
**************************************************************/

void setupWebServer()
{
	server.reset(new ESP8266WebServer(WiFi.localIP(), 80));

	server->on("/", handleRoot);

	server->on("/pump/on", []() {
		pumpEnable = true;
		server->send(200, "text/plain", "Pump is ON");
	});
	server->on("/pump/off", []() {
		pumpEnable = false;
		server->send(200, "text/plain", "Pump is OFF");
	});

	server->on("/heat/on", []() {
		heaterEnable = true;
		server->send(200, "text/plain", "Heat is ON");
	});
	server->on("/heat/off", []() {
		heaterEnable = false;
		server->send(200, "text/plain", "Heat is OFF");
	});

	server->onNotFound(handleNotFound);

	server->begin();
	Serial.println("HTTP server started");
}


const char ON_STR[] PROGMEM = "ON";
const char OFF_STR[] PROGMEM = "OFF";

void handleRoot() {
	char tempString[10];

	String message = "Waterpool flow automation.\n\n";

	for (byte t = 0; t <= lastSensorIndex; t++) {
		int tempInCelsius = (tempData[t] + 8) >> 4; // In celsius, with math rounding
		sprintf(tempString, "Temperature %d: %2d C\n", t + 1, tempInCelsius);
		message += tempString;
	}
	sprintf(tempString, "\nFlow: %2d L/min\n", litersInMinute);
	message += tempString;
	message += "\n";
	message += ("\n Pump enabled: " + (String)(pumpEnable ? FPSTR(ON_STR) : FPSTR(OFF_STR)));
	message += ("\n Heating enabled: " + (String)(heaterEnable ? FPSTR(ON_STR) : FPSTR(OFF_STR)));
	message += "\n";
	message += ("\n Pump relay status: " + (String)(relayPumpON ? FPSTR(ON_STR) : FPSTR(OFF_STR)));
	message += ("\n Heating relay status: " + (String)(relayHeatON ? FPSTR(ON_STR) : FPSTR(OFF_STR)));

	server->send(200, "text/plain", message);
}

void handleNotFound() {
	String message = "File Not Found\n\n";
	message += "URI: ";
	message += server->uri();
	message += "\nMethod: ";
	message += (server->method() == HTTP_GET) ? "GET" : "POST";
	message += "\nArguments: ";
	message += server->args();
	message += "\n";
	for (uint8_t i = 0; i < server->args(); i++) {
		message += " " + server->argName(i) + ": " + server->arg(i) + "\n";
	}
	server->send(404, "text/plain", message);
}