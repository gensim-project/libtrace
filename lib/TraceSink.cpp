#include "ArchInterface.h"
#include "TraceSink.h"
#include "TraceSource.h"

using namespace libtrace;


TraceSink::TraceSink()
{

}

BinaryFileTraceSink::BinaryFileTraceSink(FILE *outfile) : TraceSink(), outfile_(outfile)
{

}

BinaryFileTraceSink::~BinaryFileTraceSink()
{
	Flush();
	fclose(outfile_);
}

void BinaryFileTraceSink::Flush()
{
	fwrite(records_.data(), records_.size(), sizeof(TraceRecord), outfile_);
	fflush(outfile_);
	records_.clear();
}

void BinaryFileTraceSink::SinkPackets(const TraceRecord* start, const TraceRecord* end)
{
	auto *ptr = start;
	while(ptr < end) records_.push_back(*ptr++);
	if(records_.size() >= RECORD_BUFFER_SIZE) {
		Flush();
	}
}

TextFileTraceSink::TextFileTraceSink(FILE *outfile, ArchInterface *interface) : TraceSink(), outfile_(outfile), interface_(interface)
{

}

TextFileTraceSink::~TextFileTraceSink()
{
	Flush();
}

void TextFileTraceSink::SinkPackets(const TraceRecord* start, const TraceRecord* end)
{
	while(start < end) {
		WritePacket(start++);
	}
}

void TextFileTraceSink::WritePacket(const TraceRecord* pkt)
{
	switch(pkt->GetType()) {
		case TraceRecordType::InstructionHeader:
			WriteInstructionHeader((InstructionHeaderRecord*)pkt);
			return;
		case TraceRecordType::InstructionCode:
			WriteInstructionCode((InstructionCodeRecord*)pkt);
			return;
		case TraceRecordType::RegRead:
			WriteRegRead((RegReadRecord*)pkt);
			return;
		case TraceRecordType::RegWrite:
			WriteRegWrite((RegWriteRecord*)pkt);
			return;
		case TraceRecordType::BankRegRead:
			WriteBankRegRead((BankRegReadRecord*)pkt);
			return;
		case TraceRecordType::BankRegWrite:
			WriteBankRegWrite((BankRegWriteRecord*)pkt);
			return;
		case TraceRecordType::MemReadAddr:
			WriteMemReadAddr((MemReadAddrRecord*)pkt);
			return;
		case TraceRecordType::MemReadData:
			WriteMemReadData((MemReadDataRecord*)pkt);
			return;
		case TraceRecordType::MemWriteAddr:
			WriteMemWriteAddr((MemWriteAddrRecord*)pkt);
			return;
		case TraceRecordType::MemWriteData:
			WriteMemWriteData((MemWriteDataRecord*)pkt);
			return;
	}
	Flush();
}

void TextFileTraceSink::Flush()
{
	fflush(outfile_);
}

void TextFileTraceSink::WriteInstructionHeader(const InstructionHeaderRecord* record)
{
	fprintf(outfile_, "\n[%08x] ", record->GetPC());
	isa_mode_ = record->GetIsaMode();
	pc_ = record->GetPC();
}

void TextFileTraceSink::WriteInstructionCode(const InstructionCodeRecord* record)
{
	std::string disasm = interface_->DisassembleInstruction(*record);
	fprintf(outfile_, "%08x %s\t\t", record->GetIR(), disasm.c_str());
}

void TextFileTraceSink::WriteRegRead(const RegReadRecord* record)
{
	const std::string &regname = interface_->GetRegisterSlotName(record->GetRegNum());
	uint32_t width = interface_->GetRegisterSlotWidth(record->GetRegNum());
	switch(width) {
		case 1:
			fprintf(outfile_, "(R[%s] => %02x)", regname.c_str(), record->GetData());
			break;
		case 2:
			fprintf(outfile_, "(R[%s] => %04x)", regname.c_str(), record->GetData());
			break;
		case 4:
		default:
			fprintf(outfile_, "(R[%s] => %08x)", regname.c_str(), record->GetData());
			break;
	}
}

void TextFileTraceSink::WriteRegWrite(const RegWriteRecord* record)
{
	const std::string &regname = interface_->GetRegisterSlotName(record->GetRegNum());
	uint32_t width = interface_->GetRegisterSlotWidth(record->GetRegNum());

	switch(width) {
		case 1:
			fprintf(outfile_, "(R[%s] <= %02x)", regname.c_str(), record->GetData());
			break;
		case 2:
			fprintf(outfile_, "(R[%s] <= %04x)", regname.c_str(), record->GetData());
			break;
		case 4:
		default:
			fprintf(outfile_, "(R[%s] <= %08x)", regname.c_str(), record->GetData());
			break;
	}
}

void TextFileTraceSink::WriteBankRegRead(const BankRegReadRecord* record)
{
	const std::string &regname = interface_->GetRegisterSlotName(record->GetBank());
	// TODO: handle banks of registers which are not 32 bit
	fprintf(outfile_, "(R[%s][%x] => %08x)", regname.c_str(), record->GetRegNum(), record->GetData());
}

void TextFileTraceSink::WriteBankRegWrite(const BankRegWriteRecord* record)
{
	const std::string &regname = interface_->GetRegisterSlotName(record->GetBank());
	// TODO: handle banks of registers which are not 32 bit
	fprintf(outfile_, "(R[%s][%x] <= %08x)", regname.c_str(), record->GetRegNum(), record->GetData());
}

void TextFileTraceSink::WriteMemReadAddr(const MemReadAddrRecord* record)
{
	fprintf(outfile_, "(Mem[%u][%08x] => ", record->GetWidth(), record->GetAddress());
}

void TextFileTraceSink::WriteMemReadData(const MemReadDataRecord* record)
{
	switch(record->GetWidth()) {
		case 1:
			fprintf(outfile_, "%02x)", record->GetData());
			break;
		case 2:
			fprintf(outfile_, "%04x)", record->GetData());
			break;
		case 4:
			fprintf(outfile_, "%08x)", record->GetData());
			break;
		default:
			assert(false);
	}

}

void TextFileTraceSink::WriteMemWriteAddr(const MemWriteAddrRecord* record)
{
	fprintf(outfile_, "(Mem[%u][%08x] => ", record->GetWidth(), record->GetAddress());
}

void TextFileTraceSink::WriteMemWriteData(const MemWriteDataRecord* record)
{
	switch(record->GetWidth()) {
		case 1:
			fprintf(outfile_, "%02x)", record->GetData());
			break;
		case 2:
			fprintf(outfile_, "%04x)", record->GetData());
			break;
		case 4:
			fprintf(outfile_, "%08x)", record->GetData());
			break;
		default:
			assert(false);
	}
}
