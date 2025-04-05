#ifndef PICTUREBUS_H
#define PICTUREBUS_H
#include <vector>
#include "Mapper.h"
#include "Cartridge.h"

namespace sn
{
    class PictureBus
    {
    private:
        Mapper* m_mapper;
        std::vector<Byte> m_RAM;
        std::vector<Byte> m_palette;
        
        // Indices for nametables in RAM vector
        std::size_t NameTable0; 
        std::size_t NameTable1;
        std::size_t NameTable2;
        std::size_t NameTable3;
        
    public:
        PictureBus();
        
        void scanlineIRQ();
        void updateMirroring();
        void write(Address addr, Byte value);
        Byte read(Address addr);
        Byte readPalette(Byte paletteAddr);
        bool setMapper(Mapper *mapper);
    };
}
#endif // PICTUREBUS_H
