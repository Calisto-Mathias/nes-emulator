#include "../include/Cartridge.hpp"
#include "../include/Typedefs.hpp"

#include <fstream>

Cartridge::Cartridge(const std::string& sFileName) {
    iNESHeader header;

    std::ifstream ifs;
    ifs.open(sFileName, std::ifstream::binary);
    if (ifs.is_open()) {
        ifs.read((char*)&header, sizeof(iNESHeader));

        if (header.mapper1 & 0x04) {
            ifs.seekg(512, std::ios_base::cur);
        }

        nMapperID = ((header.mapper2 >> 4) << 4) | (header.mapper1 >> 4);
        nPRGBanks = header.prg_rom_chunks;
        nCHRBanks = header.chr_rom_chunks;

        uint32_t nFileType = 1;

        if (nFileType == 0) {
            // Load ROM data
        } else if (nFileType == 1) {
            nPRGBanks = header.prg_rom_chunks;
            vPRGMemory.resize(nPRGBanks * 16384);
            ifs.read((char*)vPRGMemory.data(), vPRGMemory.size());

            nCHRBanks = header.chr_rom_chunks;
            vCHRMemory.resize(nCHRBanks * 8192);
            ifs.read((char*)vCHRMemory.data(), vCHRMemory.size());
        } else if (nFileType == 2) {
            // Load ROM data
        }

        switch (nMapperID) {
            case 0: pMapper = std::make_shared<Mapper_000>(nPRGBanks, nCHRBanks); break;
        }

        ifs.close();
    }
}

Cartridge::~Cartridge() {

}

bool Cartridge::cpuWrite(Address addr, Byte data) {
    return false;
}

bool Cartridge::cpuRead(Address addr, Byte& data) {
    uint32_t mappedAddress = 0;
    if (pMapper->cpuMapRead(addr, mappedAddress)) {
        data = vPRGMemory[mappedAddress];
        return true;
    }
    return false;
}

bool Cartridge::ppuWrite(Address addr, Byte data) {
    uint32_t mappedAddress = 0;
    if (pMapper->ppuMapWrite(addr, mappedAddress)) {
        vCHRMemory[mappedAddress] = data;
        return true;
    }
    return false;
}

bool Cartridge::ppuRead(Address addr, Byte& data) {
    return false;
}