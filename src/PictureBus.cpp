#include "Log.h"
#include "PictureBus.h"

namespace sn
{
    void PictureBus::scanlineIRQ() {
        m_mapper->scanlineIRQ();
    }

    bool PictureBus::setMapper(Mapper *mapper)
    {
        if (!mapper) {
            LOG(Error) << "Mapper argument is nullptr" << std::endl;
            return false;
        }

        m_mapper = mapper;
        updateMirroring();
        return true;
    }
    
    PictureBus::PictureBus() 
        : m_mapper(nullptr),
          m_RAM(0x800),
          m_palette(0x20)
    {
    }
    
    Byte PictureBus::readPalette(Byte paletteAddr)
    {
        if (paletteAddr >= 0x10 && paletteAddr % 4 == 0) {
            paletteAddr = paletteAddr & 0xf;
        }
        return m_palette[paletteAddr];
    }
    
    void PictureBus::updateMirroring()
    {
        switch (m_mapper->getNameTableMirroring())
        {
            case Vertical:
                NameTable0 = NameTable2 = 0;
                NameTable1 = NameTable3 = 0x400;
                LOG(InfoVerbose) << "Vertical Name Table mirroring set. (Horizontal Scrolling)" << std::endl;
                break;
            case Horizontal:
                NameTable0 = NameTable1 = 0;
                NameTable2 = NameTable3 = 0x400;
                LOG(InfoVerbose) << "Horizontal Name Table mirroring set. (Vertical Scrolling)" << std::endl;
                break;
            case FourScreen:
                NameTable0 = m_RAM.size();
                LOG(InfoVerbose) << "FourScreen mirroring." << std::endl;
                break;
            case OneScreenLower:
                NameTable0 = NameTable1 = NameTable2 = NameTable3 = 0;
                LOG(InfoVerbose) << "Single Screen mirroring set with lower bank." << std::endl;
                break;
            case OneScreenHigher:
                NameTable0 = NameTable1 = NameTable2 = NameTable3 = 0x400;
                LOG(InfoVerbose) << "Single Screen mirroring set with higher bank." << std::endl;
                break;
            default:
                NameTable0 = NameTable1 = NameTable2 = NameTable3 = 0;
                LOG(Error) << "Unsupported Name Table mirroring : " << m_mapper->getNameTableMirroring() << std::endl;
        }
    }

    Byte PictureBus::read(Address addr)
    {
        addr = addr & 0x3fff;

        if (addr <= 0x1fff) {
            return m_mapper->readCHR(addr);
        }
        else if (addr >= 0x3f00) {
            auto paletteAddr = addr & 0x1f;
            return readPalette(paletteAddr);
        }
        else {
            const auto index = addr & 0x3ff;
            auto normalizedAddr = addr;
            
            if (addr >= 0x3000) {
                normalizedAddr -= 0x1000;
            }

            if (NameTable0 >= m_RAM.size()) {
                return m_mapper->readCHR(normalizedAddr);
            }
            else if (normalizedAddr < 0x2400) {
                return m_RAM[NameTable0 + index];
            }
            else if (normalizedAddr < 0x2800) {
                return m_RAM[NameTable1 + index];
            }
            else if (normalizedAddr < 0x2c00) {
                return m_RAM[NameTable2 + index];
            }
            else {
                return m_RAM[NameTable3 + index];
            }
        }
    }

    void PictureBus::write(Address addr, Byte value)
    {
        addr = addr & 0x3fff;

        if (addr <= 0x1fff) {
            m_mapper->writeCHR(addr, value);
        }
        else if (addr >= 0x3f00) {
            auto palette = addr & 0x1f;
            if (palette >= 0x10 && addr % 4 == 0) {
                palette = palette & 0xf;
            }
            m_palette[palette] = value;
        }
        else {
            const auto index = addr & 0x3ff;
            auto normalizedAddr = addr;
            
            if (addr >= 0x3000) {
                normalizedAddr -= 0x1000;
            }

            if (NameTable0 >= m_RAM.size()) {
                m_mapper->writeCHR(normalizedAddr, value);
            }
            else if (normalizedAddr < 0x2400) {
                m_RAM[NameTable0 + index] = value;
            }
            else if (normalizedAddr < 0x2800) {
                m_RAM[NameTable1 + index] = value;
            }
            else if (normalizedAddr < 0x2c00) {
                m_RAM[NameTable2 + index] = value;
            }
            else {
                m_RAM[NameTable3 + index] = value;
            }
        }
    }
}
