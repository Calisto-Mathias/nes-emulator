#include "MapperNROM.h"
#include "MapperSxROM.h"
#include "CPUOpcodes.h"
#include "MapperUxROM.h"
#include "MapperCNROM.h"
#include "MapperAxROM.h"
#include "MapperMMC3.h"
#include "MapperColorDreams.h"
#include "MapperGxROM.h"
#include "Mapper.h"

namespace sn {
    NameTableMirroring Mapper::getNameTableMirroring()
{
        return static_cast<NameTableMirroring>(m_cartridge.getNameTableMirroring());
    }

 std::unique_ptr<Mapper> Mapper::createMapper(Mapper::Type type, 
        sn::Cartridge& cart, 
        std::function<void()> irq_callback, 
        std::function<void(void)> mirroring_callback)
    {
        std::unique_ptr<Mapper> m = nullptr;
        
      switch (type)
        {
            case NROM: {
                m = std::make_unique<MapperNROM>(cart);
                break;}
                
case SxROM:
                  m = std::make_unique<MapperSxROM>(cart, mirroring_callback);
                break;
                
            case UxROM:
                m = std::make_unique<MapperUxROM>(cart);break;
                
            case CNROM:
                m = std::make_unique<MapperCNROM>(cart);
                break;
                
            case MMC3:
                m = std::make_unique<MapperMMC3>(cart, irq_callback, mirroring_callback); break;
                
            case AxROM:
                m = std::make_unique<MapperAxROM>(cart, mirroring_callback);
                break;
                
            case ColorDreams:
                m = std::make_unique<MapperColorDreams>(cart, mirroring_callback);
                break;
                
            case GxROM: {
                m = std::make_unique<MapperGxROM>(cart, mirroring_callback);
                } break;
                
default:
                break;
        }
        
        return m;
    }

}