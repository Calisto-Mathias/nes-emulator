#include "../include/Cartridge.hpp"
#include "../include/Typedefs.hpp"

#include <fstream>
#include <iostream>

Cartridge::Cartridge(const std::string& sFileName) {
    load(sFileName);
}

Cartridge::~Cartridge() {
    // Cleanup
}

bool Cartridge::load(const std::string& sFileName) {
    iNESHeader header;

    std::ifstream ifs;
    ifs.open(sFileName, std::ifstream::binary);
    if (!ifs.is_open()) {
        std::cerr << "Could not open ROM file: " << sFileName << std::endl;
        return false;
    }

    // Read the header
    ifs.read(reinterpret_cast<char*>(&header), sizeof(iNESHeader));

    // Check for NES header signature ("NES" followed by MS-DOS EOF)
    if (header.name[0] != 'N' || header.name[1] != 'E' || header.name[2] != 'S' || header.name[3] != 0x1A) {
        std::cerr << "Not a valid NES ROM file" << std::endl;
        return false;
    }

    // Handle iNES trainer if present (512 bytes)
    if (header.mapper1 & 0x04) {
        ifs.seekg(512, std::ios_base::cur);
    }

    // Determine mapper ID
    nMapperID = ((header.mapper2 >> 4) << 4) | (header.mapper1 >> 4);
    
    // Determine mirroring mode (not used yet but will be important for PPU)
    uint8_t mirrorMode = (header.mapper1 & 0x01) ? 1 : 0;
    mirrorMode |= (header.mapper1 & 0x08) ? 2 : 0;

    // Load PRG ROM (16KB banks)
    nPRGBanks = header.prg_rom_chunks;
    vPRGMemory.resize(nPRGBanks * 16384);
    ifs.read(reinterpret_cast<char*>(vPRGMemory.data()), vPRGMemory.size());

    // Load CHR ROM (8KB banks)
    nCHRBanks = header.chr_rom_chunks;
    if (nCHRBanks == 0) {
        // Create CHR RAM if there's no CHR ROM
        vCHRMemory.resize(8192);
    } else {
        vCHRMemory.resize(nCHRBanks * 8192);
        ifs.read(reinterpret_cast<char*>(vCHRMemory.data()), vCHRMemory.size());
    }

    // Create the appropriate mapper based on ID
    switch (nMapperID) {
        case 0: pMapper = std::make_shared<Mapper_000>(nPRGBanks, nCHRBanks); break;
        // Add more mappers as needed
        default:
            std::cerr << "Unsupported mapper: " << nMapperID << std::endl;
            return false;
    }

    ifs.close();
    std::cout << "ROM loaded successfully: " << sFileName << std::endl;
    std::cout << "Mapper ID: " << (int)nMapperID << ", PRG banks: " << (int)nPRGBanks << ", CHR banks: " << (int)nCHRBanks << std::endl;
    return true;
}

bool Cartridge::cpuWrite(Address addr, Byte data) {
    uint32_t mappedAddr = 0;
    if (pMapper->cpuMapWrite(addr, mappedAddr)) {
        vPRGMemory[mappedAddr] = data;
        return true;
    }
    return false;
}

bool Cartridge::cpuRead(Address addr, Byte& data) {
    uint32_t mappedAddr = 0;
    if (pMapper->cpuMapRead(addr, mappedAddr)) {
        data = vPRGMemory[mappedAddr];
        return true;
    }
    return false;
}

bool Cartridge::ppuWrite(Address addr, Byte data) {
    uint32_t mappedAddr = 0;
    if (pMapper->ppuMapWrite(addr, mappedAddr)) {
        vCHRMemory[mappedAddr] = data;
        return true;
    }
    return false;
}

bool Cartridge::ppuRead(Address addr, Byte& data) {
    uint32_t mappedAddr = 0;
    if (pMapper->ppuMapRead(addr, mappedAddr)) {
        data = vCHRMemory[mappedAddr];
        return true;
    }
    return false;
}