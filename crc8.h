// crc8.h

#ifndef _CRC8_h
#define _CRC8_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

unsigned char calculate_crc8(const unsigned char* buf, unsigned char length);

#endif // _CRC8_h

