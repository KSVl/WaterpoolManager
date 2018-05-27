/**************************************************************
Home waterpool manager
designed for the ESP8266/Arduino platform (https://github.com/esp8266/Arduino)
Intelligent managing water heating and heater overheat protection
Built by Sergey Korostel
Licensed under MIT license

HTTP Web Server functions
**************************************************************/

#include "index.h"	// HTML webpage contents with javascripts (index page)
#include "time.h"	// HTML webpage contents with javascripts (set date/time)
#include "settings.h"	// HTML webpage contents with javascripts (manage app settings)
#include "logtable.h"	// HTML webpage contents with javascripts (show logged data in table)
#include "loggraph.h"	// HTML webpage contents with javascripts (show logged data in chart)

#include "ArduinoJson.h"

// See https://arduinojson.org/assistant/
const size_t bufferSize = JSON_OBJECT_SIZE(10) + 170;
DynamicJsonBuffer jsonBuffer(bufferSize);

void setupWebServer()
{
	server.reset(new ESP8266WebServer(WiFi.localIP(), 80));

	server->on("/", handleRoot);

	server->on("/status", handleStatus);

	server->on("/pump/on", []() {
		pumpEnable = true;
		server->send(200, "text/plain", "ON");
	});
	server->on("/pump/off", []() {
		pumpEnable = false;
		server->send(200, "text/plain", "OFF");
	});

	server->on("/heat/on", []() {
		heaterEnable = true;
		server->send(200, "text/plain", "ON");
	});
	server->on("/heat/off", []() {
		heaterEnable = false;
		server->send(200, "text/plain", "OFF");
	});

	server->on("/time.html", []() {
		server->send(200, "text/html", TIME_page);
	});
	server->on("/gettime", handleGetTime);
	server->on("/settime", handleSetTime);

	server->on("/settings.html", []() {
		server->send(200, "text/html", SETTINGS_page);
	});
	server->on("/getsettings", handleGetSettings);
	server->on("/setsettings", handleSetSettings);

	server->on("/logs", handleLogs);
	server->on("/table.html", []() {
		server->send(200, "text/html", LOGTABLE_page);
	});
	server->on("/graph.html", []() {
		server->send(200, "text/html", LOGGRAPH_page);
	});

	server->onNotFound(handleNotFound);

	server->begin();
	Serial.println("HTTP server started");
}


const char ON_STR[] PROGMEM = "ON";
const char OFF_STR[] PROGMEM = "OFF";
const char JSON_RESP[] PROGMEM = R"=====(
{
	"temp":[%s],
	"flow":%d,
	"pump":"%s",
	"heat":"%s",
	"relayPump":"%s",
	"relayHeat":"%s"
}
)=====";

void handleRoot() {
	server->send(200, "text/html", MAIN_page);
}

void handleStatus() {
	char tempString[10];
	char jsonString[255];

	String temps = "";
	for (byte t = 0; t <= lastSensorIndex; t++) {
		int tempInCelsius = (tempData[t] + 8) >> 4; // In celsius, with math rounding
		sprintf(tempString, "%d,", tempInCelsius);
		temps += tempString;
	}
	temps = temps.substring(0, temps.length() - 1);

	sprintf_P(jsonString, JSON_RESP, temps.c_str(), litersInMinute,
		((String)FPSTR(pumpEnable ? ON_STR : OFF_STR)).c_str(),
		((String)FPSTR(heaterEnable ? ON_STR : OFF_STR)).c_str(),
		((String)FPSTR(relayPumpON ? ON_STR : OFF_STR)).c_str(),
		((String)FPSTR(relayHeatON ? ON_STR : OFF_STR)).c_str()
	); 
	
	server->send(200, "application/json", jsonString);
}

void handleGetTime() {
	String currentDateTime = getDateTime();
	server->send(200, "text/plain", currentDateTime);
}

void handleSetTime() {
	String dt = server->arg("dt");
	setDateTime(dt);
	server->send(200, "text/plain", "OK");
}

void handleGetSettings() {
	JsonObject& root = jsonBuffer.createObject();

	char tempString[3*8+1];
	char tempSname[10];
	for (byte index = 0; index < TSCOUNT; index++)
	{
		sprintf(tempSname, "t%d", index+1);
		root[tempSname] = tempLimits[index];
		sprintf(tempSname, "t%did", index+1);
		
		if (index <= lastSensorIndex)
		{
			char* tp = tempString;
			for (byte i = 0; i < 8; i++) {
				sprintf(tp, "%02X ", tempSensAddr[index][i]);
				tp += 3;
			}
			tp -= 1;
			*tp = '\0';
			root[tempSname] = tempString;
		}
		else
			root[tempSname] = "-";
	}
	root["f1"] = minimumAllowedFlow;
	root["lp"] = loggingPeriodSeconds;

	char jsonChar[300];
	root.printTo((char*)jsonChar, root.measureLength() + 1);
	server->send(200, "application/json", jsonChar);
}

void handleSetSettings() 
{
	String jsonString = server->arg("plain");
	JsonObject& root = jsonBuffer.parseObject(jsonString);
	if (!root.success()) {
		server->send(400, "text/plain", "400 Bad Request");
		return;
	}

	char tempSname[10];
	for (byte index = 0; index < TSCOUNT; index++)
	{
		sprintf(tempSname, "t%d", index + 1);
		tempLimits[index] = root[tempSname];
	}
	minimumAllowedFlow = root["f1"];
	loggingPeriodSeconds = root["lp"];
	SaveSettings();
	resetSensorLimitRanges();
}

void handleLogs() 
{
	server->setContentLength(CONTENT_LENGTH_UNKNOWN);
	server->send(200, "application/octet-stream", "");

	char data[sizeof(LogEvent)];
	uint32_t blockTimestamp;
	uint32_t timestamp;
	eeaddr addr = 0;
	int recNo = 0;
	while (logger.readNextRecord((unsigned char*)&data, blockTimestamp, addr))
	{
		if (blockTimestamp != 0)	// This is a start of next block
		{
			timestamp = blockTimestamp;
			recNo = 0;
		}
		else
		{
			recNo++;
			timestamp += loggingPeriodSeconds;
		}
		server->sendContent_P((char*)&timestamp, sizeof(timestamp));
		server->sendContent_P(data, sizeof(LogEvent));
	}
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