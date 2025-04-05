#ifndef MEMORY_H
#define MEMORY_H
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include "Cartridge.h"
#include "Mapper.h"

namespace sn
{
    struct IORegistersHasher
    {
        std::size_t operator()(sn::IORegisters const & reg) const noexcept
        {
            return std::hash<std::uint32_t>{}(reg);
        }
    };
    
    enum IORegisters
    {
        JOY1 = 0x4016,
        JOY2 = 0x4017,
        PPUCTRL = 0x2000,
        PPUMASK,
        PPUSTATUS,
        OAMADDR,
        OAMDATA,
        PPUSCROL,
        PPUADDR,
        PPUDATA,
        OAMDMA = 0x4014,
    };

    class MainBus
    {
    private:
        Mapper* m_mapper;
        std::vector<Byte> m_RAM;
        std::vector<Byte> m_extRAM;
        
        std::unordered_map<IORegisters, std::function<void(Byte)>, IORegistersHasher> m_writeCallbacks;
        std::unordered_map<IORegisters, std::function<Byte(void)>, IORegistersHasher> m_readCallbacks;
        
    public:
        MainBus();
        
        Byte read(Address addr);
        void write(Address addr, Byte value);
        const Byte* getPagePtr(Byte page);
        
        bool setMapper(Mapper* mapper);
        bool setReadCallback(IORegisters reg, std::function<Byte(void)> callback);
        bool setWriteCallback(IORegisters reg, std::function<void(Byte)> callback);
    };
};

#endif // MEMORY_H
