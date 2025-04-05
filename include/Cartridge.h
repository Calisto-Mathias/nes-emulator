#ifndef CARTRIDGE_H
#define CARTRIDGE_H
#include <vector>
#include <string>
#include <cstdint>

namespace sn
{
    using Address = std::uint16_t;
    using Byte = std::uint8_t;

    class Cartridge
    {
    private:
        std::vector<Byte> m_PRG_ROM;
        std::vector<Byte> m_CHR_ROM;
        Byte m_nameTableMirroring;
        Byte m_mapperNumber;
        bool m_extendedRAM;
        bool m_chrRAM;
        
    public:
        Cartridge();
        Byte getNameTableMirroring();
        bool hasExtendedRAM();
        Byte getMapper();
        const std::vector<Byte>& getROM();
        const std::vector<Byte>& getVROM();
        bool loadFromFile(std::string path);
    };
};

#endif // CARTRIDGE_H
