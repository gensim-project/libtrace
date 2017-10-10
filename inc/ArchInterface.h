#ifndef ARCHINTERFACE_H_
#define ARCHINTERFACE_H_

#include "RecordTypes.h"

#include <string>

class ArchInterface {
public:
	virtual std::string DisassembleInstruction(const InstructionCodeRecord &record) = 0;
	
	virtual std::string GetRegisterSlotName(int index) = 0;
	virtual std::string GetRegisterBankName(int index) = 0;
};

class DefaultArchInterface : public ArchInterface {
	virtual std::string DisassembleInstruction(const InstructionCodeRecord &record) override;
	
	virtual std::string GetRegisterSlotName(int index) override;
	virtual std::string GetRegisterBankName(int index) override;
};

#endif
