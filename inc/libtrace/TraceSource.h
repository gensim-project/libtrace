#ifndef TRACESOURCE_H_
#define TRACESOURCE_H_

#include "RecordTypes.h"

#include <cstdint>
#include <string>
#include <vector>

namespace libtrace {
	class TraceSource
	{
	public:
		static const size_t RecordBufferSize = 1024 * 128;
		static const size_t PacketBufferSize = 1024;
		
		TraceSource(uint32_t BufferSize);
		virtual ~TraceSource();

		typedef std::string reg_name_t;
		
		virtual void Terminate();
		void EmitPackets();

		inline void Suppress()
		{
			suppressed = 1;
		}
		inline void Unsuppress()
		{
			suppressed = 0;
		}
		inline bool IsSuppressed()
		{
			return suppressed;
		}

		template <typename PCT> void TraceInstructionHeader(PCT pc, uint8_t isa_mode);
		template <typename CodeT> void TraceInstructionCode(CodeT pc, uint8_t irq_mode);
		
		template<typename PCT, typename CodeT> void Trace_Insn(PCT PC, CodeT IR, bool JIT, uint8_t isa_mode, uint8_t irq_mode, uint8_t exec)
		{
			assert(!IsTerminated() && !IsPacketOpen());

			TraceInstructionHeader(PC, isa_mode);
			TraceInstructionCode(IR, irq_mode);
			
			packet_open = true;
		}

		inline void Trace_End_Insn()
		{
			if(!IsPacketOpen()) return;
			assert(!IsTerminated() && IsPacketOpen());
			Tracing_Packet_Count++;
			packet_open = false;
		}

		/*
		 * Vector Register Operation Tracing
		 */
		inline void Trace_Vector_Bank_Reg_Read(bool Trace, uint8_t Bank, uint8_t Regnum, uint8_t Regindex, uint32_t Value)
		{
			if(!IsPacketOpen()) return;
			assert(!IsTerminated() && IsPacketOpen());

			// XXX TODO
		}

		inline void Trace_Vector_Bank_Reg_Read(bool Trace, reg_name_t &BankName, uint8_t Regnum, uint8_t Regindex, uint32_t Value)
		{
			Trace_Vector_Bank_Reg_Read(Trace, get_regbank_id(BankName), Regnum, Regindex, Value);
		}

		inline void Trace_Vector_Bank_Reg_Write(bool Trace, uint8_t Bank, uint8_t Regnum, uint8_t Regindex, uint32_t Value)
		{
			if(!IsPacketOpen()) return;
			assert(!IsTerminated() && IsPacketOpen());

			// XXX TODO
		}

		inline void Trace_Vector_Bank_Reg_Write(bool Trace, reg_name_t &BankName, uint8_t Regnum, uint8_t Regindex, uint32_t Value)
		{
			Trace_Vector_Bank_Reg_Write(Trace, get_regbank_id(BankName), Regnum, Regindex, Value);
		}

		/*
		 * Banked Register Operation Tracing
		 */

		inline void Trace_Bank_Reg_Read(bool Trace, uint8_t Bank, uint8_t Regnum, uint32_t Value)
		{
			if(!IsPacketOpen()) return;
			assert(!IsTerminated() && IsPacketOpen());

			BankRegReadRecord *header = (BankRegReadRecord*)(getNextPacket());
			*header = BankRegReadRecord(Bank, Regnum, Value);
		}

		inline void Trace_Bank_Reg_Read(bool Trace, reg_name_t &BankName, uint8_t Regnum, uint32_t Value)
		{
			Trace_Bank_Reg_Read(Trace, get_regbank_id(BankName), Regnum, Value);
		}

		inline void Trace_Bank_Reg_Write(bool Trace, uint8_t Bank, uint8_t Regnum, uint32_t Value)
		{
			if(!IsPacketOpen()) return;
			assert(!IsTerminated() && IsPacketOpen());

			BankRegWriteRecord *header = (BankRegWriteRecord*)(getNextPacket());
			*header = BankRegWriteRecord(Bank, Regnum, Value);
		}

		inline void Trace_Bank_Reg_Write(bool Trace, reg_name_t &BankName, uint8_t Regnum, uint32_t Value)
		{
			Trace_Bank_Reg_Write(Trace, get_regbank_id(BankName), Regnum, Value);
		}

		/*
		 * Register Operation Tracing
		 */
		template <typename T> void Trace_Reg_Read(bool Trace, uint8_t Regnum, T Value);
		
		template <typename T> void Trace_Reg_Read(bool Trace, reg_name_t &RegName, T Value)
		{
			Trace_Reg_Read<T>(Trace, get_reg_id(RegName), Value);
		}

		template <typename T> void Trace_Reg_Write(bool Trace, uint8_t Regnum, T Value);

		template <typename T> void Trace_Reg_Write(bool Trace, reg_name_t &RegName, T Value)
		{
			Trace_Reg_Write<T>(Trace, get_reg_id(RegName), Value);
		}

		/*
		 * Memory Operation Tracing
		 */
		template<typename T> void TraceMemReadAddr(T Addr, uint32_t Width);
		template<typename T> void TraceMemReadData(T Data, uint32_t Width);
		
		template<typename AddrT, typename DataT> void Trace_Mem_Read(bool Trace, AddrT Addr, DataT Value, uint32_t Width=4)
		{
			if(!IsPacketOpen()) return;
			assert(!IsTerminated() && IsPacketOpen());

			TraceMemReadAddr(Width, Addr);
			TraceMemReadData(Width, Value);
		}
		
		template<typename T> void TraceMemWriteAddr(T Addr, uint32_t Width);
		template<typename T> void TraceMemWriteData(T Data, uint32_t Width);

		template<typename AddrT, typename DataT> void Trace_Mem_Write(bool Trace, AddrT Addr, DataT Value, uint32_t Width=4)
		{
			if(!IsPacketOpen()) return;
			assert(!IsTerminated() && IsPacketOpen());

			TraceMemWriteAddr(Width, Addr);
			TraceMemWriteData(Width, Value);
		}

		inline bool IsTerminated() const
		{
			return is_terminated;
		}

		inline bool IsPacketOpen() const
		{
			return packet_open;
		}

		void SetAggressiveFlush(bool b)
		{
			aggressive_flushing_ = b;
		}
		bool GetAggressiveFlush() const
		{
			return aggressive_flushing_;
		}

		void SetSink(TraceSink *sink)
		{
			sink_ = sink;
		}

		void Flush();

	protected:
		uint32_t IO_Packet_Count;
		uint32_t Tracing_Packet_Count;
		bool packet_open;

	private:		
		TraceRecord *getNextPacket()
		{
			if(packet_buffer_pos == packet_buffer_end || GetAggressiveFlush()) EmitPackets();
			return packet_buffer_pos++;
		}

		TraceSink *sink_;

		std::vector<reg_name_t> VectorRegBanks;
		std::vector<reg_name_t> RegBanks;
		std::vector<reg_name_t> Regs;

		TraceRecord *packet_buffer;
		TraceRecord *packet_buffer_pos;
		TraceRecord *packet_buffer_end;

		bool is_terminated;
		bool suppressed;
		bool aggressive_flushing_;

		TraceSource();

	protected:
		inline uint8_t get_reg_id(reg_name_t &reg_name)
		{
			std::vector<reg_name_t>::iterator i = std::find(Regs.begin(), Regs.end(), reg_name);
			if (i == Regs.end()) {
				Regs.push_back(reg_name);
				return (uint8_t)(Regs.size() - 1);
			}
			return (uint8_t)std::distance(Regs.begin(), i);
		}

		inline reg_name_t get_reg_name(uint32_t reg_id) const
		{
			assert(reg_id < Regs.size());
			return Regs[reg_id];
		}

		inline uint8_t get_regbank_id(reg_name_t &reg_name)
		{
			std::vector<reg_name_t>::iterator i = std::find(RegBanks.begin(), RegBanks.end(), reg_name);
			if (i == RegBanks.end()) {
				RegBanks.push_back(reg_name);
				return (uint8_t)(RegBanks.size() - 1);
			}
			return (uint8_t)std::distance(RegBanks.begin(), i);
		}

		inline reg_name_t get_regbank_name(uint32_t reg_id) const
		{
			assert(reg_id < RegBanks.size());
			return RegBanks[reg_id];
		}

	};
	
	template <> void TraceSource::TraceInstructionHeader(uint32_t pc, uint8_t isa_mode) {
		auto *header = (InstructionHeaderRecord*)getNextPacket();
		*header = InstructionHeaderRecord(pc, isa_mode);
	}
	template <> void TraceSource::TraceInstructionHeader(uint64_t pc, uint8_t isa_mode) {
		auto *header = (InstructionHeaderRecord*)getNextPacket();
		*header = InstructionHeaderRecord(pc, isa_mode);
		
		auto *extension = (DataExtensionRecord*)getNextPacket();
		*extension = DataExtensionRecord(InstructionHeader, pc >> 32);
	}
	template <> void TraceSource::TraceInstructionCode(uint32_t ir, uint8_t irq_mode) {
		auto *header = (InstructionCodeRecord*)getNextPacket();
		*header = InstructionCodeRecord(ir, irq_mode);
	}
	template <> void TraceSource::TraceInstructionCode(uint64_t ir, uint8_t irq_mode) {
		auto *header = (InstructionCodeRecord*)getNextPacket();
		*header = InstructionCodeRecord(ir, irq_mode);
		
		auto *extension = (DataExtensionRecord*)getNextPacket();
		*extension = DataExtensionRecord(InstructionCode, ir >> 32);
	}
	
	template <> void TraceSource::Trace_Reg_Read(bool Trace, uint8_t Regnum, uint32_t Value)
	{
		if(!IsPacketOpen()) return;
		assert(!IsTerminated() && IsPacketOpen());

		RegReadRecord *header = (RegReadRecord*)(getNextPacket());
		*header = RegReadRecord(Regnum, Value);
	}
	template <> void TraceSource::Trace_Reg_Read(bool Trace, uint8_t Regnum, uint64_t Value)
	{
		if(!IsPacketOpen()) return;
		assert(!IsTerminated() && IsPacketOpen());

		RegReadRecord *record = (RegReadRecord*)(getNextPacket());
		*record = RegReadRecord(Regnum, Value);
		
		auto extension = (DataExtensionRecord*)(getNextPacket());
		*extension = DataExtensionRecord(RegRead, Value >> 32);
	}
	
	
	template <> void TraceSource::Trace_Reg_Write(bool Trace, uint8_t Regnum, uint32_t Value)
	{
		if(!IsPacketOpen()) return;
		assert(!IsTerminated() && IsPacketOpen());

		RegWriteRecord *record = (RegWriteRecord*)(getNextPacket());
		*record = RegWriteRecord(Regnum, Value);
	}
	template <> void TraceSource::Trace_Reg_Write(bool Trace, uint8_t Regnum, uint64_t Value)
	{
		if(!IsPacketOpen()) return;
		assert(!IsTerminated() && IsPacketOpen());

		RegWriteRecord *record = (RegWriteRecord*)(getNextPacket());
		*record = RegWriteRecord(Regnum, Value);
		
		auto extension = (DataExtensionRecord*)(getNextPacket());
		*extension = DataExtensionRecord(RegWrite, Value >> 32);
	}
	
	template<> void TraceSource::TraceMemReadAddr(uint32_t Addr, uint32_t Width) {
		auto *record = (MemReadAddrRecord*)getNextPacket();
		*record = MemReadAddrRecord(Addr, Width);
	}
	template<> void TraceSource::TraceMemReadAddr(uint64_t Addr, uint32_t Width) {
		auto *record = (MemReadAddrRecord*)getNextPacket();
		*record = MemReadAddrRecord(Addr, Width);
		
		auto *extension = (DataExtensionRecord*)getNextPacket();
		*extension = DataExtensionRecord(MemReadAddr, Addr >> 32);
	}
	
	template<> void TraceSource::TraceMemWriteAddr(uint32_t Addr, uint32_t Width) {
		auto *record = (MemWriteAddrRecord*)getNextPacket();
		*record = MemWriteAddrRecord(Addr, Width);
	}
	template<> void TraceSource::TraceMemWriteAddr(uint64_t Addr, uint32_t Width) {
		auto *record = (MemWriteAddrRecord*)getNextPacket();
		*record = MemWriteAddrRecord(Addr, Width);
		
		auto *extension = (DataExtensionRecord*)getNextPacket();
		*extension = DataExtensionRecord(MemReadAddr, Addr >> 32);
	}
	
	
}

#endif
