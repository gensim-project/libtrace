#ifndef INSTRUCTIONPRINTER_H
#define INSTRUCTIONPRINTER_H

#include <iostream>
#include "RecordTypes.h"

namespace libtrace {

	class RecordIterator;

	class InstructionPrinter
	{
	public:
		InstructionPrinter();

		std::string operator()(RecordIterator &it, RecordIterator end);

		bool PrintInstruction(std::ostream &str, RecordIterator &it, RecordIterator end); 
		
		void SetDisplayNone()
		{
			_print_reg_read = _print_reg_write = _print_bank_read = _print_bank_write = _print_mem_read = _print_mem_write = 0;
		}
		
		void SetDisplayMem()
		{
			_print_mem_read = _print_mem_write = 1;
		}

		void SetDisplayAll()
		{
			_print_reg_read = _print_reg_write = _print_bank_read = _print_bank_write = _print_mem_read = _print_mem_write = 1;
		}
		
	private:
		bool PrintRegRead(std::ostream &str, RegReadRecord *rcd);
		bool PrintRegWrite(std::ostream &str, RegWriteRecord *rcd);
		bool PrintBankRegRead(std::ostream &str, BankRegReadRecord *rcd);
		bool PrintBankRegWrite(std::ostream &str, BankRegWriteRecord *rcd);

		bool PrintMemRead(std::ostream &str, RecordIterator &it);
		bool PrintMemWrite(std::ostream &str, RecordIterator &it);

		bool _print_reg_read, _print_reg_write, _print_bank_read, _print_bank_write, _print_mem_read, _print_mem_write;
	};

}

#endif
