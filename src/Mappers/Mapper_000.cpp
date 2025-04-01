#include "../../include/Mappers/Mapper_000.hpp"
#include "../../include/Typedefs.hpp"
#include "../../include/Mapper.hpp"

Mapper_000::Mapper_000(uint8_t nPRGBanks, uint8_t nCHRBanks) : Mapper(nPRGBanks, nCHRBanks) {
    
}

Mapper_000::~Mapper_000() {
    
}

bool Mapper_000::cpuMapRead(uint16_t addr, uint32_t &mapped_addr) {
    // NROM has fixed PRG banks at 0x8000
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        // For NROM-128, mirror the single 16KB PRG bank
        if (nPRGBanks == 1) {
            mapped_addr = addr & 0x3FFF;
        } else { // NROM-256
            mapped_addr = addr & 0x7FFF; 
        }
        return true;
    }
    return false;
}

bool Mapper_000::cpuMapWrite(uint16_t addr, uint32_t &mapped_addr) {
    // Same as read
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        if (nPRGBanks == 1) {
            mapped_addr = addr & 0x3FFF;
        } else {
            mapped_addr = addr & 0x7FFF;
        }
        return true;
    }
    return false;
}

bool Mapper_000::ppuMapRead(uint16_t addr, uint32_t &mapped_addr) {
    // CHR ROM/RAM (pattern memory)
    if (addr >= 0x0000 && addr <= 0x1FFF) {
        mapped_addr = addr;
        return true;
    }
    return false;
}

bool Mapper_000::ppuMapWrite(uint16_t addr, uint32_t &mapped_addr) {
    // CHR ROM can't be written to unless it's actually CHR RAM
    if (addr >= 0x0000 && addr <= 0x1FFF) {
        if (nCHRBanks == 0) { // Indicates CHR RAM
            mapped_addr = addr;
            return true;
        }
    }
    return false;
}