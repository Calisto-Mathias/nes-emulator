#ifndef CARTRIDGE_HPP
#define CARTRIDGE_HPP

#include "Typedefs.hpp"
#include "Mappers/Mapper_000.hpp"
#include <vector>
#include <memory>

class Cartridge
{
public:
    Cartridge(const std::string&);
    ~Cartridge();

    bool cpuWrite(Address, Byte);
	bool cpuRead(Address, Byte&);

    bool ppuWrite(Address, Byte);
    bool ppuRead(Address, Byte&);

    std::vector<Byte> vPRGMemory;
    std::vector<Byte> vCHRMemory;

    uint8_t nMapperID = 0;
    uint8_t nPRGBanks = 0;
    uint8_t nCHRBanks = 0;

    std::shared_ptr<Mapper> pMapper;
};

#endif