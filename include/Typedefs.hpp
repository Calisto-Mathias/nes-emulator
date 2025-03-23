#ifndef TYPEDEFS_HPP
#define TYPEDEFS_HPP

#include "CPU.hpp"

class CPU;

typedef uint8_t Byte;
typedef uint16_t Address;

typedef uint8_t Register;
typedef uint16_t LargeRegister;

typedef Byte Opcode;

namespace StatusRegisterFlags {
    enum Flags{
        C = (1 << 0), // Carry Bit
        Z = (1 << 1), // Zero
        I = (1 << 2), // Interrupt Disable
        D = (1 << 3), // Decimal Mode
        B = (1 << 4), // Break
        U = (1 << 5), // Unused
        V = (1 << 6), // Overflow
        N = (1 << 7) // Negative
    };
}

typedef bool (CPU::*AddressingMode)();
typedef bool (CPU::*OperationFunction)();

struct Instruction {
    AddressingMode addressingMode;
    OperationFunction operation;
    const uint8_t cyclesCount;
    std::string name;

    Instruction() : addressingMode(nullptr), operation(nullptr), cyclesCount(0), name("") {}
    Instruction(AddressingMode mode, OperationFunction op, uint8_t cycles, std::string nameSetter)
        : addressingMode(mode), operation(op), cyclesCount(cycles), name(nameSetter) {}
};

#endif