#ifndef TRACESOURCE_H_
#define TRACESOURCE_H_

#include "RecordTypes.h"

#include <cstdint>
#include <string>
#include <vector>

#define RECORD_BUFFER_SIZE 1024*128
#define PACKET_BUFFER_SIZE 1024

namespace libtrace {
	class TraceSource
	{
	public:
		TraceSource(uint32_t BufferSize);
		virtual ~TraceSource();

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

		inline void Trace_Insn(uint32_t PC, uint32_t IR, bool JIT, uint8_t isa_mode, uint8_t irq_mode, uint8_t exec)
		{
			assert(!IsTerminated() && !IsPacketOpen());

			InstructionHeaderRecord *header = (InstructionHeaderRecord*)(getNextPacket());
			*header = InstructionHeaderRecord(isa_mode, PC);

			InstructionCodeRecord *code = (InstructionCodeRecord*)(getNextPacket());
			*code = InstructionCodeRecord(irq_mode, IR);

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

		inline void Trace_Vector_Bank_Reg_Read(bool Trace, std::string BankName, uint8_t Regnum, uint8_t Regindex, uint32_t Value)
		{
			Trace_Vector_Bank_Reg_Read(Trace, get_regbank_id(BankName), Regnum, Regindex, Value);
		}

		inline void Trace_Vector_Bank_Reg_Write(bool Trace, uint8_t Bank, uint8_t Regnum, uint8_t Regindex, uint32_t Value)
		{
			if(!IsPacketOpen()) return;
			assert(!IsTerminated() && IsPacketOpen());

			// XXX TODO
		}

		inline void Trace_Vector_Bank_Reg_Write(bool Trace, std::string BankName, uint8_t Regnum, uint8_t Regindex, uint32_t Value)
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

		inline void Trace_Bank_Reg_Read(bool Trace, std::string BankName, uint8_t Regnum, uint32_t Value)
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

		inline void Trace_Bank_Reg_Write(bool Trace, std::string BankName, uint8_t Regnum, uint32_t Value)
		{
			Trace_Bank_Reg_Write(Trace, get_regbank_id(BankName), Regnum, Value);
		}

		/*
		 * Register Operation Tracing
		 */
		inline void Trace_Reg_Read(bool Trace, uint8_t Regnum, uint32_t Value)
		{
			if(!IsPacketOpen()) return;
			assert(!IsTerminated() && IsPacketOpen());

			RegReadRecord *header = (RegReadRecord*)(getNextPacket());
			*header = RegReadRecord(Regnum, Value);
		}

		inline void Trace_Reg_Read(bool Trace, std::string RegName, uint32_t Value)
		{
			Trace_Reg_Read(Trace, get_reg_id(RegName), Value);
		}

		inline void Trace_Reg_Write(bool Trace, uint8_t Regnum, uint32_t Value)
		{
			if(!IsPacketOpen()) return;
			assert(!IsTerminated() && IsPacketOpen());

			RegWriteRecord *header = (RegWriteRecord*)(getNextPacket());
			*header = RegWriteRecord(Regnum, Value);
		}

		inline void Trace_Reg_Write(bool Trace, std::string RegName, uint32_t Value)
		{
			Trace_Reg_Write(Trace, get_reg_id(RegName), Value);
		}

		/*
		 * Memory Operation Tracing
		 */
		inline void Trace_Mem_Read(bool Trace, uint32_t Addr, uint32_t Value, uint32_t Width=4)
		{
			if(!IsPacketOpen()) return;
			assert(!IsTerminated() && IsPacketOpen());

			MemReadAddrRecord *header = (MemReadAddrRecord*)(getNextPacket());
			*header = MemReadAddrRecord(Width, Addr);

			MemReadDataRecord *data = (MemReadDataRecord*)(getNextPacket());
			*data = MemReadDataRecord(Width, Value);
		}

		inline void Trace_Mem_Write(bool Trace, uint32_t Addr, uint32_t Value, uint32_t Width=4)
		{
			if(!IsPacketOpen()) return;
			assert(!IsTerminated() && IsPacketOpen());

			MemWriteAddrRecord *header = (MemWriteAddrRecord*)(getNextPacket());
			*header = MemWriteAddrRecord(Width, Addr);

			MemWriteDataRecord *data = (MemWriteDataRecord*)(getNextPacket());
			*data = MemWriteDataRecord(Width, Value);
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

		std::vector<std::string> VectorRegBanks;
		std::vector<std::string> RegBanks;
		std::vector<std::string> Regs;

		TraceRecord *packet_buffer;
		TraceRecord *packet_buffer_pos;
		TraceRecord *packet_buffer_end;

		bool is_terminated;
		bool suppressed;
		bool aggressive_flushing_;

		TraceSource();

	protected:
		inline uint8_t get_reg_id(std::string reg_name)
		{
			std::vector<std::string>::iterator i = std::find(Regs.begin(), Regs.end(), reg_name);
			if (i == Regs.end()) {
				Regs.push_back(reg_name);
				return (uint8_t)(Regs.size() - 1);
			}
			return (uint8_t)std::distance(Regs.begin(), i);
		}

		inline std::string get_reg_name(uint32_t reg_id) const
		{
			assert(reg_id < Regs.size());
			return Regs[reg_id];
		}

		inline uint8_t get_regbank_id(const std::string reg_name)
		{
			std::vector<std::string>::iterator i = std::find(RegBanks.begin(), RegBanks.end(), reg_name);
			if (i == RegBanks.end()) {
				RegBanks.push_back(reg_name);
				return (uint8_t)(RegBanks.size() - 1);
			}
			return (uint8_t)std::distance(RegBanks.begin(), i);
		}

		inline std::string get_regbank_name(uint32_t reg_id) const
		{
			assert(reg_id < RegBanks.size());
			return RegBanks[reg_id];
		}

	};
}

#endif
