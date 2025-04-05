#ifndef MAPPER_H
#define MAPPER_H
#include <memory>
#include <functional>
#include "CPUOpcodes.h"
#include "Cartridge.h"

namespace sn
{
    enum NameTableMirroring
    {
        FourScreen  = 8,
        Vertical    = 1,
        Horizontal  = 0,
        OneScreenLower,
        OneScreenHigher,
    };

    class Mapper
    {
    protected:
        Cartridge& m_cartridge;
        Type m_type;
        
    public:
        enum Type
        {
            GxROM = 66,
            ColorDreams = 11,
            AxROM = 7,
            MMC3  = 4,
            CNROM = 3,
            UxROM = 2,
            SxROM = 1,
            NROM  = 0,
        };
        
        Mapper(Cartridge& cart, Type t) : m_cartridge(cart), m_type(t) {};
        virtual ~Mapper() = default;
        
        virtual void scanlineIRQ(){}
        virtual NameTableMirroring getNameTableMirroring();
        
        virtual Byte readPRG(Address addr) = 0;
        virtual void writePRG(Address addr, Byte value) = 0;
        virtual Byte readCHR(Address addr) = 0;
        virtual void writeCHR(Address addr, Byte value) = 0;
        
        bool inline hasExtendedRAM() {
            return m_cartridge.hasExtendedRAM();
        }
        
        static std::unique_ptr<Mapper> createMapper(
            Type mapper_t, 
            Cartridge& cart, 
            std::function<void()> interrupt_cb, 
            std::function<void(void)> mirroring_cb
        );
    };
}

#endif //MAPPER_H
