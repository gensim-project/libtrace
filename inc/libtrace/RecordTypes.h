#ifndef RECORDS_H
#define RECORDS_H

#include <cstdint>

namespace libtrace {

	enum TraceRecordType
	{
		Unknown,
		
		InstructionHeader,
		InstructionCode,
		RegRead,
		RegWrite,
		BankRegRead,
		BankRegWrite,
		MemReadAddr,
		MemReadData,
		MemWriteAddr,
		MemWriteData,
		
		DataExtension
	};
		
	struct Record
	{
	public:
		Record(uint32_t header=0, uint32_t data=0) : header(header), data(data) {}
	protected:
		uint32_t GetHeader() const { return header; }
		uint32_t GetData() const { return data; }
	private:
		uint32_t header, data;
	};

	struct TraceRecord : public Record
	{
	public:
		TraceRecord(TraceRecordType type, uint16_t data16, uint32_t data32) : Record((((uint32_t)type) << 16) | data16, data32) {}
		TraceRecord(const TraceRecord &tr) : Record((((uint32_t)tr.GetType()) << 16) | tr.GetData16(), tr.GetData32()) {}
		TraceRecordType GetType() const { return (TraceRecordType)(GetHeader() >> 16); }
		
		uint16_t GetData16() const { return GetHeader() & 0xffff; }
		uint32_t GetData32() const { return GetData(); }
	};

	struct InstructionHeaderRecord : public TraceRecord
	{
	public:
		InstructionHeaderRecord(uint8_t isa_mode, uint32_t pc) : TraceRecord(InstructionHeader, isa_mode, pc) {}
		
		uint8_t GetIsaMode() const { return GetData16(); }
		uint32_t GetPC() const { return GetData32(); }
	};

	struct InstructionCodeRecord : public TraceRecord
	{
	public:
		InstructionCodeRecord(uint16_t irq_mode, uint32_t ir) : TraceRecord(InstructionCode, irq_mode, ir) {}
		
		uint16_t GetIRQMode() const { return GetData16(); }
		uint32_t GetIR() const { return GetData32(); }
	};

	struct RegReadRecord : public TraceRecord
	{
	public:
		RegReadRecord(uint16_t regnum, uint32_t data) : TraceRecord(RegRead, regnum, data) {}
		
		uint16_t GetRegNum() const { return GetData16(); }
		uint32_t GetData() const { return GetData32(); }
	};

	struct RegWriteRecord : public TraceRecord
	{
	public:
		RegWriteRecord(uint16_t regnum, uint32_t data) : TraceRecord(RegWrite, regnum, data) {}
		
		uint16_t GetRegNum() const { return GetData16(); }
		uint32_t GetData() const { return GetData32(); }
	};

	struct BankRegReadRecord : public TraceRecord
	{
	public:
		BankRegReadRecord(uint8_t bank, uint8_t regnum, uint32_t data) : TraceRecord(BankRegRead, ((uint16_t)bank << 8) | regnum, data) {}
		
		uint8_t GetBank() const { return GetData16() >> 8; }
		uint8_t GetRegNum() const { return GetData16() & 0xff; }
		uint32_t GetData() const { return GetData32(); }
	};

	struct BankRegWriteRecord : public TraceRecord
	{
	public:
		BankRegWriteRecord(uint8_t bank, uint8_t regnum, uint32_t data) : TraceRecord(BankRegWrite, ((uint16_t)bank << 8) | regnum, data) {}
		
		uint8_t GetBank() const { return GetData16() >> 8; }
		uint8_t GetRegNum() const { return GetData16() & 0xff; }
		uint32_t GetData() const { return GetData32(); }
	};

	struct MemReadAddrRecord : public TraceRecord
	{
	public:
		MemReadAddrRecord(uint8_t width, uint32_t address) : TraceRecord(MemReadAddr, width, address) {}
		
		uint8_t GetWidth() const { return GetData16(); }
		uint32_t GetAddress() const { return GetData32(); }
	};

	struct MemReadDataRecord : public TraceRecord
	{
	public:
		MemReadDataRecord(uint8_t width, uint32_t address) : TraceRecord(MemReadData, width, address) {}
		
		uint8_t GetWidth() const { return GetData16(); }
		uint32_t GetData() const { return GetData32() & ((1ULL << (GetWidth()*8))-1); }
	};

	struct MemWriteAddrRecord : public TraceRecord
	{
	public:
		MemWriteAddrRecord(uint8_t width, uint32_t address) : TraceRecord(MemWriteAddr, width, address) {}
		
		uint8_t GetWidth() const { return GetData16(); }
		uint32_t GetAddress() const { return GetData32(); }
	};

	struct MemWriteDataRecord : public TraceRecord
	{
	public:
		MemWriteDataRecord(uint8_t width, uint32_t address) : TraceRecord(MemWriteData, width, address) {}
		
		uint8_t GetWidth() const { return GetData16(); }
		uint32_t GetData() const { return GetData32() & ((1ULL << (GetWidth()*8))-1); }
	};

	struct DataExtensionRecord : public TraceRecord
	{
	public:
		DataExtensionRecord(uint16_t prevtype, uint32_t data) : TraceRecord(DataExtension, prevtype, data) {}
		
		uint32_t GetData() const { return GetData32(); }
	};
	
}

#endif
