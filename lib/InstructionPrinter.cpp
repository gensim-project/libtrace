#include "libtrace/InstructionPrinter.h"
#include "libtrace/RecordIterator.h"
#include "libtrace/RecordTypes.h"

#include <cassert>
#include <iomanip>
#include <sstream>

using namespace libtrace;

InstructionPrinter::InstructionPrinter()
{
	SetDisplayAll();
}

std::string InstructionPrinter::operator()(RecordIterator &it, RecordIterator end)
{
	std::stringstream str;
	PrintInstruction(str, it, end);
	return str.str();
}

bool InstructionPrinter::PrintInstruction(std::ostream &str, RecordIterator &it, RecordIterator end)
{
	// Print both header records
	TraceRecord header_record = *it++;
	TraceRecord code_record = *it++;
	
	assert(header_record.GetType() == InstructionHeader);
	assert(code_record.GetType() == InstructionCode);
	
	InstructionHeaderRecord *hdr = (InstructionHeaderRecord*)&header_record;
	InstructionCodeRecord *code = (InstructionCodeRecord*)&code_record;
	
	str << "[" << std::hex << std::setw(8) << std::setfill('0') << hdr->GetPC() << "] " << std::hex << std::setw(8) << std::setfill('0') << code->GetIR() << " ";
	
	while(it != end && (*it).GetType() != InstructionHeader) {
		// do a thing
		TraceRecord rcd = *it;
		TraceRecord *record = &rcd;
		
		switch((*it).GetType()) {
			case RegRead: PrintRegRead(str, (RegReadRecord*)record); break;
			case RegWrite: PrintRegWrite(str, (RegWriteRecord*)record); break;
			case BankRegRead: PrintBankRegRead(str, (BankRegReadRecord*)record); break;
			case BankRegWrite: PrintBankRegWrite(str, (BankRegWriteRecord*)record); break;
			case MemReadAddr: PrintMemRead(str, it); break;
			case MemWriteAddr: PrintMemWrite(str, it); break;
			
			default:
				fprintf(stderr, "Unsupported record type %u at %lu\n", (*it).GetType(), it.index());
				assert(false && "Unsupported record type");
		}
		
		it++;
	}
	
	return true;
}

bool InstructionPrinter::PrintRegRead(std::ostream &str, RegReadRecord* rec)
{
	assert(rec->GetType() == RegRead);
	if(!_print_reg_read) return true;
	
	str << "(R[" << rec->GetRegNum() << "] -> 0x" << std::hex << std::setw(8) << std::setfill('0') << rec->GetData() << ")";
	return true;
}

bool InstructionPrinter::PrintRegWrite(std::ostream &str, RegWriteRecord* rec)
{
	assert(rec->GetType() == RegWrite);
	if(!_print_reg_write) return true;
	
	str << "(R[" << rec->GetRegNum() << "] <- 0x" << std::hex << std::setw(8) << std::setfill('0') << rec->GetData() << ")";
	return true;
}

bool InstructionPrinter::PrintBankRegRead(std::ostream &str, BankRegReadRecord *rec){
	if(!_print_bank_read) return true;
	str << "(R[" << (uint32_t)rec->GetBank() << "][" << (uint32_t)rec->GetRegNum() <<"] -> 0x" << std::hex << std::setw(8) << std::setfill('0') << rec->GetData() << ")";
	return true;
}

bool InstructionPrinter::PrintBankRegWrite(std::ostream &str, BankRegWriteRecord *rec){
	if(!_print_bank_write) return true;
	str << "(R[" << (uint32_t)rec->GetBank() << "][" << (uint32_t)rec->GetRegNum() <<"] <- 0x" << std::hex << std::setw(8) << std::setfill('0') << rec->GetData() << ")";
	return true;
}

bool InstructionPrinter::PrintMemRead(std::ostream &str, RecordIterator &it){
	
	TraceRecord raddr = *it++;
	TraceRecord rdata = *it;
	
	if(!_print_mem_read) return true;
	
	MemReadAddrRecord *addr = (MemReadAddrRecord*)&raddr;
	MemReadDataRecord *data = (MemReadDataRecord*)&rdata;
	
	str << "([" << std::hex << std::setw(8) << std::setfill('0') << addr->GetAddress() << "](" << (uint32_t)addr->GetWidth() << ") => 0x" << std::hex << std::setw(8) << std::setfill('0') << data->GetData() << ")";
	
	return true;
}
bool InstructionPrinter::PrintMemWrite(std::ostream &str, RecordIterator &it){	
	TraceRecord raddr = *it++;
	TraceRecord rdata = *it;
	
	if(!_print_mem_write) return true;
	
	MemWriteAddrRecord *addr = (MemWriteAddrRecord*)&raddr;
	MemWriteDataRecord *data = (MemWriteDataRecord*)&rdata;
	
	str << "([" << std::hex << std::setw(8) << std::setfill('0') << addr->GetAddress() << "](" << (uint32_t)addr->GetWidth() << ") <= 0x" << std::hex << std::setw(8) << std::setfill('0') << data->GetData() << ")";
	
	return true;
}
