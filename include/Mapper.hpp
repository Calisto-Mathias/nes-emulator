#ifndef MAPPER_HPP
#define MAPPER_HPP

#include <cstdint>
#include "Typedefs.hpp"

class Mapper {
public:
    Mapper(uint8_t, uint8_t);
    ~Mapper();

    virtual bool cpuMapRead(Address, uint32_t&) = 0;
    virtual bool cpuMapWrite(Address, uint32_t&) = 0;
    virtual bool ppuMapRead(Address, uint32_t&) = 0;
    virtual bool ppuMapWrite(Address, uint32_t&) = 0;

    uint8_t nPRGBanks = 0;
    uint8_t nCHRBanks = 0;
};

#endif