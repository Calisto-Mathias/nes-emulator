#ifndef CPU_HPP
#define CPU_HPP

#include <cstdint>
#include <array>
#include <string>
#include <map>
#include "Typedefs.hpp"
#include "Constants.hpp"

class CPU;
class Bus;

class CPU
{
    public:
        CPU();
        ~CPU();

        // Input Signals into the CPU are Public
        void Clock();
        void Reset();
        void IRQ();
        void NMI();

        void write(uint16_t addr, uint8_t data);
        uint8_t read(uint16_t addr);

        void ConnectBus(Bus *b) {
            bus = b;
        }


    private:

        Bus *bus = nullptr;
        Byte FetchByteFromMemory(const Address);
        Byte FetchDataForOperation();
        void WriteByteToMemory(const Address, const Byte);

        // TODO: Change the return types to use std::optional<Byte> rather than Byte!
        // Addressing Modes
        bool IMP();
        bool IMM();
        bool ZP0();
        bool ZPX();
        bool ZPY();
        bool ABS();
        bool ABX();
        bool ABY();
        bool IND();
        bool IZX();
        bool IZY();
        bool REL();

        // Opcode Functions
        bool ADC(); // Add with Carry
        bool AND(); // And with Accumulator
        bool ASL(); // Arithmetic Shift Left
        bool BCC(); // Branch on Carry Clear
        bool BCS(); // Branch on Carry Set
        bool BEQ(); // Branch on Equal
        bool BIT(); // Bit Test
        bool BMI(); // Branch on Minus
        bool BNE(); // Branch on Not Equal
        bool BPL(); // Branch on Plus
        bool BRK(); // Force Break
        bool BVC(); // Branch on Overflow Clear
        bool BVS(); // Branch on Overflow Set
        bool CLC(); // Clear Carry Flag
        bool CLD(); // Clear Decimal Mode
        bool CLI(); // Clear Interrupt Disable
        bool CLV(); // Clear Overflow Flag
        bool CMP(); // Compare Accumulator
        bool CPX(); // Compare X Register
        bool CPY(); // Compare Y Register
        bool DEC(); // Decrement Memory
        bool DEX(); // Decrement X Register
        bool DEY(); // Decrement Y Register
        bool EOR(); // Exclusive Or with Accumulator
        bool INC(); // Increment Memory
        bool INX(); // Increment X Register
        bool INY(); // Increment Y Register
        bool JMP(); // Jump to Address
        bool JSR(); // Jump to Subroutine
        bool LDA(); // Load Accumulator
        bool LDX(); // Load X Register
        bool LDY(); // Load Y Register
        bool LSR(); // Logical Shift Right
        bool NOP(); // No Operation
        bool ORA(); // Or with Accumulator
        bool PHA(); // Push Accumulator
        bool PHP(); // Push Processor Status
        bool PLA(); // Pull Accumulator
        bool PLP(); // Pull Processor Status
        bool ROL(); // Rotate Left
        bool ROR(); // Rotate Right
        bool RTI(); // Return from Interrupt
        bool RTS(); // Return from Subroutine
        bool SBC(); // Subtract with Carry
        bool SEC(); // Set Carry Flag
        bool SED(); // Set Decimal Mode
        bool SEI(); // Set Interrupt Disable
        bool STA(); // Store Accumulator
        bool STX(); // Store X Register
        bool STY(); // Store Y Register
        bool TAX(); // Transfer Accumulator to X
        bool TAY(); // Transfer Accumulator to Y
        bool TSX(); // Transfer Stack Pointer to X
        bool TXA(); // Transfer X to Accumulator
        bool TXS(); // Transfer X to Stack Pointer
        bool TYA(); // Transfer Y to Accumulator
        bool XXX(); // Catches all illegal Instructions!

        // Utility Functions
        inline uint8_t GetNumberOfBaseClockCyclesLeftForOperation(const Opcode);
        inline bool GetFlagFromStatusRegister(const StatusRegisterFlags::Flags);
        inline void SetFlagInStatusRegister(const StatusRegisterFlags::Flags, const bool);
        std::map<Address, std::string> Disassemble(Address, Address);

    private:
        Register Accumulator;
        Register X;
        Register Y;
        Register StackPointer;
        Register StatusRegister;
        LargeRegister ProgramCounter;
        AddressingMode AddressingModeFunc;
        OperationFunction OperationFunc;
    
        Byte FetchedData;
        Address AbsoluteAddress;
        Address RelativeAddress;
        Opcode CurrentOpcode;
        uint8_t CyclesLeft;

        uint16_t TemporaryStorage;

        std::array<Instruction, NUMBER_OF_LEGAL_INSTRUCTIONS> OpcodeTable;
};

#endif