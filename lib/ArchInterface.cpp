#include "ArchInterface.h"

using namespace libtrace;

std::string ArchInterface::DisassembleInstruction(const InstructionCodeRecord &record) {
	return "???";
}

std::string ArchInterface::GetRegisterSlotName(int idx) {
	return "???";
}
std::string ArchInterface::GetRegisterBankName(int idx) {
	return "???";
}

uint32_t ArchInterface::GetRegisterSlotWidth(int idx) {
	return 0;
}
uint32_t ArchInterface::GetRegisterBankWidth(int idx) {
	return 0;
}
