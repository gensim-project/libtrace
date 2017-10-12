#include "libtrace/InstructionPrinter.h"
#include "libtrace/RecordIterator.h"
#include "libtrace/RecordTypes.h"

#include <cassert>
#include <iomanip>
#include <sstream>
#include <vector>

using namespace libtrace;

class DataPrinter {
public:
	DataPrinter(uint32_t data_low, const InstructionPrinter::extension_list_t &extensions) : data_low_(data_low), extensions_(extensions) {
		
	}
	
	friend std::ostream& operator<< (std::ostream& stream, const DataPrinter& matrix) {
		stream << "0x" << std::hex << std::setw(8) << std::setfill('0');
		for(auto i : matrix.extensions_) {
			stream << i.GetData();
		}
		stream << matrix.data_low_;
		return stream;
	}
	
private:
	uint32_t data_low_;
	const InstructionPrinter::extension_list_t &extensions_;
};

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
	
	std::vector<DataExtensionRecord> extensions;
	while(it != end && (*it).GetType() != InstructionHeader) {
		// do a thing
		TraceRecord rcd = *it;
		TraceRecord *record = &rcd;
		
		if(record->GetType() == DataExtension) {
			extensions.push_back(*(DataExtensionRecord*)&rcd);
			continue;
		}
		
		assert(extensions.size() == record->GetExtensionCount());
		
		switch((*it).GetType()) {
			case RegRead: PrintRegRead(str, (RegReadRecord*)record, extensions); break;
			case RegWrite: PrintRegWrite(str, (RegWriteRecord*)record, extensions); break;
			case BankRegRead: PrintBankRegRead(str, (BankRegReadRecord*)record, extensions); break;
			case BankRegWrite: PrintBankRegWrite(str, (BankRegWriteRecord*)record, extensions); break;
			case MemReadAddr: PrintMemRead(str, it, extensions); break;
			case MemWriteAddr: PrintMemWrite(str, it, extensions); break;
			default:
				fprintf(stderr, "Unsupported record type %u at %lu\n", (*it).GetType(), it.index());
				assert(false && "Unsupported record type");
		}
		
		extensions.clear();
		
		it++;
	}
	
	return true;
}

bool InstructionPrinter::PrintRegRead(std::ostream &str, RegReadRecord* rec, const extension_list_t& extensions)
{
	assert(rec->GetType() == RegRead);
	if(!_print_reg_read) return true;
	
	str << "(R[" << rec->GetRegNum() << "] -> 0x" << std::hex << std::setw(8) << std::setfill('0') << rec->GetData() << ")";
	return true;
}

bool InstructionPrinter::PrintRegWrite(std::ostream &str, RegWriteRecord* rec, const extension_list_t& extensions)
{
	assert(rec->GetType() == RegWrite);
	if(!_print_reg_write) return true;
	
	str << "(R[" << rec->GetRegNum() << "] <- 0x" << std::hex << std::setw(8) << std::setfill('0') << rec->GetData() << ")";
	return true;
}

bool InstructionPrinter::PrintBankRegRead(std::ostream &str, BankRegReadRecord *rec, const extension_list_t& extensions){
	if(!_print_bank_read) return true;
	str << "(R[" << (uint32_t)rec->GetBank() << "][" << (uint32_t)rec->GetRegNum() <<"] -> 0x" << std::hex << std::setw(8) << std::setfill('0') << rec->GetData() << ")";
	return true;
}

bool InstructionPrinter::PrintBankRegWrite(std::ostream &str, BankRegWriteRecord *rec, const extension_list_t& extensions){
	if(!_print_bank_write) return true;
	str << "(R[" << (uint32_t)rec->GetBank() << "][" << (uint32_t)rec->GetRegNum() <<"] <- 0x" << std::hex << std::setw(8) << std::setfill('0') << rec->GetData() << ")";
	return true;
}

bool InstructionPrinter::PrintMemRead(std::ostream &str, RecordIterator &it, const extension_list_t& addr_extensions){
	
	TraceRecord raddr = *it++;
	extension_list_t data_extensions;
	while((*it).GetType() == DataExtension) {
		TraceRecord tr = *it;
		data_extensions.push_back(*(DataExtensionRecord*)&tr);
		it++;
	}
	TraceRecord rdata = *it;
	
	if(!_print_mem_read) return true;
	
	MemReadAddrRecord *addr = (MemReadAddrRecord*)&raddr;
	MemReadDataRecord *data = (MemReadDataRecord*)&rdata;
	
	str << "([" << DataPrinter(addr->GetAddress(), addr_extensions) << "](" << (uint32_t)addr->GetWidth() << ") => " << DataPrinter(data->GetData(), data_extensions) << ")";
	
	return true;
}
bool InstructionPrinter::PrintMemWrite(std::ostream &str, RecordIterator &it, const extension_list_t& addr_extensions){	
	TraceRecord raddr = *it++;
	
	extension_list_t data_extensions;
	while((*it).GetType() == DataExtension) {
		TraceRecord tr = *it;
		data_extensions.push_back(*(DataExtensionRecord*)&tr);
		it++;
	}
	TraceRecord rdata = *it;
	
	if(!_print_mem_write) return true;
	
	MemWriteAddrRecord *addr = (MemWriteAddrRecord*)&raddr;
	MemWriteDataRecord *data = (MemWriteDataRecord*)&rdata;
	
	str << "([" << DataPrinter(addr->GetAddress(), addr_extensions) << "](" << (uint32_t)addr->GetWidth() << ") <= " << DataPrinter(data->GetData(), data_extensions) << ")";
	return true;
}

