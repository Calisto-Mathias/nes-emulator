#include "../../include/Mappers/Mapper_000.hpp"
#include "../../include/Typedefs.hpp"
#include "../../include/Mapper.hpp"

Mapper_000::Mapper_000(uint8_t prgBanks, uint8_t chrBanks)
    : Mapper(prgBanks, chrBanks) {}

Mapper_000::~Mapper_000() {}

bool Mapper_000::cpuMapRead(Address address, uint32_t &mappedAddress) {
    if (address >= 0x8000 && address <= 0xFFFF) {
        mappedAddress = address & (nPRGBanks > 1 ? 0x7FFF : 0x3FFF);
        return true;
    }

    return false;
}

bool Mapper_000::cpuMapWrite(Address address, uint32_t &mappedAddress) {
    if (address >= 0x8000 && address <= 0xFFFF) {
        mappedAddress = address & (nPRGBanks > 1 ? 0x7FFF : 0x3FFF);
        return true;
    }

    return false;
}

// TODO: Maybe has to change.

bool Mapper_000::ppuMapRead(Address address, uint32_t &mappedAddress) {
    if (address >= 0x0000 && address <= 0x1FFF) {
        mappedAddress = address;
        return true;
    }

    return false;
}

bool Mapper_000::ppuMapWrite(Address address, uint32_t &mappedAddress) {
    if (address >= 0x0000 && address <= 0x1FFF) {
        if (nCHRBanks == 0) {
            mappedAddress = address;
            return true;
        }
    }

    return false;
}

