#ifndef MAPPER_HPP
#define MAPPER_HPP

#include <cstdint>
#include "Typedefs.hpp"

class Mapper
{
public:
    Mapper(uint8_t prgBanks, uint8_t chrBanks) : nPRGBanks(prgBanks), nCHRBanks(chrBanks) {}
    virtual ~Mapper() = default;

    virtual bool cpuMapRead(Address, uint32_t &) = 0;
    virtual bool cpuMapWrite(Address, uint32_t &) = 0;
    virtual bool ppuMapRead(Address, uint32_t &) = 0;
    virtual bool ppuMapWrite(Address, uint32_t &) = 0;

    // Optional reset capability for mappers with registers
    virtual void reset() {}

    // Optional scanline counter for mappers with IRQ capability
    virtual void scanline() {}

    uint8_t nPRGBanks = 0;
    uint8_t nCHRBanks = 0;
};



#endif