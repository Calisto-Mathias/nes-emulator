#include <cstring>
#include "Log.h"
#include "MainBus.h"

namespace sn
{
    const Byte* MainBus::getPagePtr(Byte pg)
    {
        Address address = pg << 8;
        
        if (address < 0x2000) {
            return &m_RAM[address & 0x7ff];
        }
        
        if (address >= 0x6000 && address < 0x8000) {
            if (m_mapper->hasExtendedRAM()) 
                return &m_extRAM[address - 0x6000];
        }
        
        if (address >= 0x4020 && address < 0x6000) 
        {
            LOG(Error) << "Expansion ROM access attempted, which is unsupported" << std::endl;
        }
        else if (address >= 0x2000 && address < 0x4020) 
        {
            LOG(Error) << "Register address memory pointer access attempt" << std::endl;
        }
        else {
            LOG(Error) << "Unexpected DMA request: " << std::hex << "0x" << +address << " (" << +pg << ")" << std::dec << std::endl;
        }
        
        return nullptr;
    }

    bool MainBus::setReadCallback(IORegisters reg, std::function<Byte(void)> callback)
    {
        if (!callback) { LOG(Error) << "callback argument is nullptr" << std::endl; return false; }
        
        return m_readCallbacks.emplace(reg, callback).second;
    }
    bool MainBus::setMapper(Mapper* mapper)
    {
        if (!mapper) { 
            LOG(Error) << "Mapper pointer is nullptr" << std::endl;
            return false;
        }

        m_mapper = mapper;
        
        if (mapper->hasExtendedRAM() == true) {
            m_extRAM.resize(0x2000);
        }

        return true;
    }
    
    bool MainBus::setWriteCallback(IORegisters reg, std::function<void(Byte)> callback) {
        if (!callback) { LOG(Error) << "callback argument is nullptr" << std::endl; return false; }
        auto result = m_writeCallbacks.emplace(reg, callback).second;
        return result;
    }

    MainBus::MainBus()
        : m_RAM(0x800, 0),
        m_mapper(nullptr)
    { }
    
    void MainBus::write(Address addr, Byte val)
    {
        if (addr < 0x2000) {
            m_RAM[addr & 0x7ff] = val;
            return;
        }
        
        if (addr >= 0x8000) {m_mapper->writePRG(addr, val); return;}
        
        if (addr >= 0x6000 && addr < 0x8000) 
        { 
          if (m_mapper->hasExtendedRAM()) {
                m_extRAM[addr - 0x6000] = val;
          }
            return;
        }
        
        if (addr >= 0x4020) {
            LOG(InfoVerbose) << "Expansion ROM access attempted. This is currently unsupported" << std::endl;
            return;
        }
        
        if (addr < 0x4000) {
            auto iter = m_writeCallbacks.find(static_cast<IORegisters>(addr & 0x2007));
            if (iter != m_writeCallbacks.end()) 
               (iter->second)(val);
            else
                LOG(InfoVerbose) << "No write callback registered for I/O register at: " << std::hex << +addr << std::endl;
        } else if (addr >= 0x4014 && addr < 0x4017) {
            auto it = m_writeCallbacks.find(static_cast<IORegisters>(addr));
            if (it != m_writeCallbacks.end()) (it->second)(val);
            else LOG(InfoVerbose) << "No write callback registered for I/O register at: " << std::hex << +addr << std::endl;
        } else {
            LOG(InfoVerbose) << "Write access attempt at: " << std::hex << +addr << std::endl;
        }
    }

    Byte MainBus::read(Address address)
    {
        if (address < 0x2000) return m_RAM[address & 0x7ff];
        
        if (address >= 0x8000) return m_mapper->readPRG(address);
        
        if (address >= 0x6000 && address < 0x8000) {
            if (m_mapper->hasExtendedRAM()) 
            {
                return m_extRAM[address - 0x6000];
            }
            return 0;
        }
        
        if (address >= 0x4020) 
        {
            LOG(InfoVerbose) << "Expansion ROM read attempted. This is currently unsupported" << std::endl;
            return 0;
        }
        
        if (address < 0x4000) {
            auto callback_it = m_readCallbacks.find(static_cast<IORegisters>(address & 0x2007));
            if (callback_it != m_readCallbacks.end()) 
               { return (callback_it->second)(); }
            else
                LOG(InfoVerbose) << "No read callback registered for I/O register at: " << std::hex << +address << std::endl;
        }
        else if (address >= 0x4014 && address < 0x4018) {
            auto IT = m_readCallbacks.find(static_cast<IORegisters>(address));
            if (IT != m_readCallbacks.end()) 
                    return (IT->second)();
            else
                LOG(InfoVerbose) << "No read callback registered for I/O register at: " << std::hex << +address << std::endl;
        }
        else {
            LOG(InfoVerbose) << "Read access attempt at: " << std::hex << +address << std::endl;
        }
        
        return 0xFF;
    }
}