/**************************************************************
Home waterpool manager
designed for the ESP8266/Arduino platform (https://github.com/esp8266/Arduino)
Intelligent managing water heating and heater overheat protection
Built by Sergey Korostel
Licensed under MIT license

Class for the circular logging, resistant to power off at any time
**************************************************************/

#include "crc8.h"
#include "EepromLogger.h"
#include <limits.h>

EepromLogger::EepromLogger(Write_Handler *writeHandler, Read_Handler *readHandler, GetTimestamp_Handler *getTimestampHandler)
{
	writeByte = writeHandler;
	readByte = readHandler;
	getTimestamp = getTimestampHandler;
}

// Initialize the storage, set current position to the right position to continue writing
void EepromLogger::initialize(const eeaddr start_address, const eeaddr end_address,
	const unsigned int records_in_block, const unsigned int record_size, const bool crcForEachRecord)
{
	this->start_address = start_address;
	this->end_address = end_address;
	this->records_in_block = records_in_block;
	this->record_size = record_size;
	this->crcForEachRecord = crcForEachRecord;

	total_storage_size = end_address+1 - start_address;
	unsigned char crcSize = crcForEachRecord ? sizeof(unsigned char) : 0;
	block_size = sizeof(Block_Header) + records_in_block * (record_size + crcSize);
	block_count = total_storage_size / block_size;

	// Continue write since the last valid block, so find the most recent block for this purpose
	if (!findLastValidBlock())
		current_Address = start_address;
	else
	{	// Go to the next empty block
		current_Address += block_size;
		// If last block doesn't fit completely, go to beginning
		if (current_Address + block_size > end_address+1)
			current_Address = start_address;
	}
}

// Save the new record to the memory storage
// The call of this function should go in the uniform period of time.
void EepromLogger::writeNextRecord(const Logger_Record record)
{
	// If address reached the start of the next block, then write the header
	if ((current_Address - start_address) % block_size == 0)
	{ 
		Block_Header block_Header;;
		// If last block doesn't fit completely, start from beginning
		if (current_Address + block_size > end_address + 1)
			current_Address = start_address;
		block_Header.timestamp = getTimestamp();
		block_Header.crc = calculate_crc8((unsigned char *)&block_Header.timestamp, sizeof(block_Header.timestamp));
		writeBytes(current_Address, (unsigned char*)&block_Header, sizeof(block_Header));
		current_Address += sizeof(block_Header);
	}

	// Write the record itself
	writeBytes(current_Address, record, record_size);
	current_Address += record_size;
	if (crcForEachRecord)
	{
		unsigned char crc = calculate_crc8(record, record_size);
		writeByte(current_Address, crc);
		current_Address += sizeof(crc);
	}
}

// Read the next record from the memory storage. The record time may calculated from the blockTimestamp value and number of record read.
// Return false if no more valid records can be read
bool EepromLogger::readNextRecord(Logger_Record record, uint32_t &timestamp, eeaddr &readAddress)
{
	if (readAddress == 0)
		readAddress = start_address;
	bool read = false;
	Block_Header block_Header;
	while (!read)
	{
		// If address reached the start of the next block, read the header
		if ((readAddress - start_address) % block_size == 0)
		{
			// Last block that can't fit completely and is not used, end reading
			if (readAddress + block_size > end_address + 1)
				return false;
			if (!readBlockHeader(block_Header, readAddress))
			{
				moveToNextBlock(readAddress);
				continue;
			}
		}
		timestamp = block_Header.timestamp;
		read = readRecord(record, readAddress);
		if (!read)
			moveToNextBlock(readAddress);
	}
}

// Private members

// Read the block header from the address, update the address after reading
// Return false if the current block is not valid
bool EepromLogger::readBlockHeader(Block_Header &block_Header, eeaddr &address)
{
	readBytes(address, (unsigned char*)&block_Header, sizeof(block_Header));
	address += sizeof(block_Header);
	unsigned char crc = calculate_crc8((unsigned char *)&block_Header.timestamp, sizeof(block_Header.timestamp));
	return block_Header.crc == crc;
}

// Read the data record from the address, update the address after reading
// Return false if the read record is not valid
bool EepromLogger::readRecord(Logger_Record &record, eeaddr &address)
{
	bool validRecord = true;
	readBytes(address, record, record_size);
	address += record_size;
	if (crcForEachRecord)
	{
		unsigned char crc = readByte(address);
		address += sizeof(crc);
		unsigned char crc1 = calculate_crc8((unsigned char *)&record, record_size);
		validRecord = crc == crc1;
	}
	return validRecord;
}

// Find 1st (most oldest) valid block, set current position to the start of this block
// Return false if no any valid blocks
bool EepromLogger::findFirstValidBlock()
{
	bool found = false;
	uint32_t minimalTimestamp = UINT_MAX;
	unsigned int block_address;
	Block_Header block_Header;
	unsigned int i;
	for (i = 0; i < block_count; i++) {
		unsigned int address = start_address + i * block_size;
		current_Address = address;
		if (readBlockHeader(block_Header, current_Address))
			if (block_Header.timestamp != 0 && block_Header.timestamp < minimalTimestamp)
			{
				minimalTimestamp = block_Header.timestamp;
				block_address = address;
				found = true;
			}
	}
	current_Address = block_address;
	return found;
}

// Find last (most recent) valid block, set current position to the start of this block
// Return false if no any valid blocks
bool EepromLogger::findLastValidBlock()
{
	bool found = false;
	uint32_t maximumTimestamp = 0;
	unsigned int block_address;
	Block_Header block_Header;
	unsigned int i;
	for (i = 0; i < block_count; i++) {
		unsigned int address = start_address + i * block_size;
		current_Address = address;
		if (readBlockHeader(block_Header, current_Address))
			if (block_Header.timestamp > maximumTimestamp)
			{
				maximumTimestamp = block_Header.timestamp;
				block_address = address;
				found = true;
			}
	}
	current_Address = block_address;
	return found;
}

// Move address pointer to the beginning of following block
void EepromLogger::moveToNextBlock(eeaddr & address)
{
	unsigned int nextBlockNo = (address - start_address) / block_size + 1;
	address = start_address + (eeaddr)nextBlockNo * block_size;
}

void EepromLogger::writeBytes(eeaddr address, const unsigned char* data, unsigned int datasize)
{
	for (unsigned int i = 0; i < datasize; i++)
		writeByte(address++, *data++);
}

void EepromLogger::readBytes(eeaddr address, unsigned char *data, unsigned int datasize) 
{
	for (unsigned i = 0; i < datasize; i++)
		*data++ = readByte(address++);
}
