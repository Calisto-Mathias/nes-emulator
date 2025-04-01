#ifndef TYPEDEFS_HPP
#define TYPEDEFS_HPP

#include <cstdint>
#include <string>

class CPU;

// typedef uint8_t Byte;
// typedef uint16_t Address;

// typedef uint8_t Register;
// typedef uint16_t LargeRegister;

// typedef Byte Opcode;

using Byte = uint8_t;
using Address = uint16_t;
using Register = uint8_t;
using LargeRegister = uint16_t;
using Opcode = uint8_t;

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

using AddressingMode = bool (CPU::*)();
using OperationFunction = bool (CPU::*)();

struct Instruction {
    AddressingMode addressingMode;
    OperationFunction operation;
    const uint8_t cyclesCount;
    std::string name;

    Instruction() : addressingMode(nullptr), operation(nullptr), cyclesCount(0), name("") {}
    Instruction(AddressingMode mode, OperationFunction op, uint8_t cycles, std::string nameSetter)
        : addressingMode(mode), operation(op), cyclesCount(cycles), name(nameSetter) {}
};

typedef struct
{
    char name[4];
    uint8_t prg_rom_chunks;
    uint8_t chr_rom_chunks;
    uint8_t mapper1;
    uint8_t mapper2;
    uint8_t prg_ram_size;
    uint8_t tv_system1;
    uint8_t tv_system2;
    char unused[5];
} iNESHeader;

#endif