#ifndef CARTRIDGE_HPP
#define CARTRIDGE_HPP

#include "Typedefs.hpp"
#include "Mappers/Mapper_000.hpp"
#include <vector>
#include <memory>
#include <string>

class Cartridge
{
public:
    // Default constructor needed for initial setup in Emulator class
    Cartridge() = default;
    Cartridge(const std::string& filename);
    ~Cartridge();

    bool cpuWrite(Address addr, Byte data);
    bool cpuRead(Address addr, Byte&);
    
    bool ppuWrite(Address addr, Byte data);
    bool ppuRead(Address addr, Byte&);
    
    // Add load method to handle ROM loading
    bool load(const std::string& filename);

    std::vector<Byte> vPRGMemory;
    std::vector<Byte> vCHRMemory;

    uint8_t nMapperID = 0;
    uint8_t nPRGBanks = 0;
    uint8_t nCHRBanks = 0;

    std::shared_ptr<Mapper> pMapper;
};

#endif