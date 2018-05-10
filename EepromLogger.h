#ifndef __EEPROMLOGGER_H__
#define __EEPROMLOGGER_H__

typedef unsigned long eeaddr;

#pragma pack(push, 1) 
struct Block_Header
{
	uint32_t timestamp;
	unsigned char crc;
};
#pragma pack(pop)

typedef unsigned char* Logger_Record;

class EepromLogger
{

public:
	typedef void Write_Handler(unsigned long, const unsigned char);
	typedef unsigned char Read_Handler(unsigned long);
	typedef uint32_t GetTimestamp_Handler();

	EepromLogger(Write_Handler *, Read_Handler *, GetTimestamp_Handler *);

	void initialize(const eeaddr start_address, const eeaddr end_address,
		const unsigned int records_in_block, const unsigned int record_size, const bool crcForEachRecord);
	void writeNextRecord(const Logger_Record);
	bool readNextRecord(Logger_Record, uint32_t &timestamp, eeaddr &addr);

private:
	eeaddr start_address;
	eeaddr end_address;
	unsigned int records_in_block;
	unsigned int record_size;
	bool crcForEachRecord;

	eeaddr total_storage_size;
	unsigned int block_size;
	unsigned int block_count;

	eeaddr current_Address;		// Current address for writing

	Write_Handler *writeByte;
	Read_Handler *readByte;
	GetTimestamp_Handler *getTimestamp;

	bool readBlockHeader(Block_Header &block_Header, eeaddr &address);
	bool readRecord(Logger_Record &record, eeaddr &address);
	bool findFirstValidBlock();
	bool findLastValidBlock();
	void moveToNextBlock(eeaddr & readAddress);

	void writeBytes(eeaddr address, const unsigned char* data, unsigned int recsize);
	void readBytes(eeaddr address, unsigned char *data, unsigned int datasize);
};

#endif // __EEPROMLOGGER_H__
