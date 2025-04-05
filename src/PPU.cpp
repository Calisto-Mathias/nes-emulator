#include "Log.h"
#include "PPU.h"

#define GET_MSB(x) ((x & 0x80) >> 7)
#define GET_BIT(x, bit) ((x >> bit) & 1)

namespace sn
{
    void PPU::writeOAM(Byte oam_addr, Byte val) 
    { 
        m_spriteMemory[oam_addr] = val;
    }
    
    Byte PPU::readOAM(Byte addr) {
        return m_spriteMemory[addr];
    }
    
    PPU::PPU(PictureBus& bus, VirtualScreen& screen) 
        : m_bus(bus), m_screen(screen),
          m_spriteMemory(64 * 4),
          m_pictureBuffer(ScanlineVisibleDots, std::vector<sf::Color>(VisibleScanlines, sf::Color::Magenta))
    {
    }
    
    void PPU::setInterruptCallback(std::function<void(void)> cb) {
        m_vblankCallback = cb;
    }
    
    Byte PPU::getStatus() {
        Byte result = 0;
        
        if (m_vblank)
            result |= 0x80;
            
        if (m_sprZeroHit)
            result |= 0x40;
            
        if (m_spriteOverflow)
            result |= 0x20;
            
        m_vblank = false;
        
        m_firstWrite = true;
        
        return result;
    }
    
    void PPU::control(Byte ctrl)
    {
        m_generateInterrupt = (ctrl & 0x80) ? true : false;
        
        m_longSprites = (ctrl & 0x20) ? true : false;
        
        m_bgPage = static_cast<CharacterPage>((ctrl & 0x10) != 0);
        m_sprPage = static_cast<CharacterPage>((ctrl & 0x8) != 0);
        
        m_dataAddrIncrement = (ctrl & 0x4) ? 32 : 1;
        
        m_tempAddress &= ~0xc00;
        m_tempAddress |= (ctrl & 3) << 10;
    }
    
    Byte PPU::getData()
    {
        auto buffer_data = m_dataBuffer;
        m_dataBuffer = read(m_dataAddress);
        
        if (m_dataAddress >= 0x3f00) buffer_data = m_dataBuffer;
        
        m_dataAddress += m_dataAddrIncrement;
        return buffer_data;
    }
    
    void PPU::setData(Byte data)
    {
        write(m_dataAddress, data);
        m_dataAddress += m_dataAddrIncrement;
    }
    
    Byte PPU::getOAMData()
    {
        return m_spriteMemory[m_spriteDataAddress];
    }
    
    void PPU::setOAMAddress(Byte addr)
    {
        m_spriteDataAddress = addr;
    }
    
    void PPU::setOAMData(Byte val)
    {
        m_spriteMemory[m_spriteDataAddress++] = val;
    }
    
    void PPU::doDMA(const Byte* page_ptr)
    {
        std::memcpy(&m_spriteMemory[m_spriteDataAddress], page_ptr, 256 - m_spriteDataAddress);
        
        if (m_spriteDataAddress) 
            std::memcpy(m_spriteMemory.data(), page_ptr + (256 - m_spriteDataAddress), m_spriteDataAddress);
    }
    
    void PPU::setMask(Byte mask)
    {
        m_greyscaleMode = mask & 0x1;
        
        m_hideEdgeBackground = !(mask & 0x2);
        
        m_hideEdgeSprites = !(mask & 0x4);
        
        m_showBackground = mask & 0x8;
        
        m_showSprites = mask & 0x10;
    }
    
    void PPU::setScroll(Byte scrl)
    {
        if (m_firstWrite) 
        {
            m_tempAddress &= ~0x1f;
            m_tempAddress |= (scrl >> 3) & 0x1f;
            m_fineXScroll = scrl & 0x7;
            m_firstWrite = false;
        } 
        else 
        {
            m_tempAddress &= ~0x73e0;
            m_tempAddress |= ((scrl & 0x7) << 12);
            m_tempAddress |= ((scrl & 0xf8) << 2);
            m_firstWrite = true;
        }
    }
    
    void PPU::setDataAddress(Byte addr)
    {
        if (m_firstWrite)
        {
            m_tempAddress &= 0xff;
            m_tempAddress |= (addr & 0x3f) << 8;
            m_firstWrite = false;
        }
        else 
        {
            m_tempAddress &= 0xff00;
            m_tempAddress |= addr;
            m_dataAddress = m_tempAddress;
            m_firstWrite = true;
        }
    }
    
    Byte PPU::read(Address addr) {
        return m_bus.read(addr); 
    }
    
    void PPU::write(Address addr, Byte value)
    {
        m_bus.write(addr, value);
    }
    
    void PPU::reset() 
    { 
        m_longSprites = false; m_vblank = false;
        m_generateInterrupt = false; m_sprZeroHit = false;
        m_greyscaleMode = false; m_spriteOverflow = false;
        
        m_showBackground = true; 
        m_showSprites = true;
        m_evenFrame = true; 
        m_firstWrite = true;
        
        m_bgPage = m_sprPage = CharacterPage::Low;
        
        m_scanline = 0; m_cycle = 0; 
        m_dataAddress = 0; m_spriteDataAddress = 0;
        m_fineXScroll = 0; m_tempAddress = 0;
        
        m_dataAddrIncrement = 1;
        m_pipelineState = PreRender;
        
        m_scanlineSprites.reserve(8);
        m_scanlineSprites.resize(0);
    }
    
    void PPU::step()
    {
        switch (m_pipelineState)
        {
            case PreRender:
                if (m_cycle == 1)
                    m_vblank = m_sprZeroHit = false;
                else if (m_cycle == ScanlineVisibleDots + 2 && m_showBackground && m_showSprites)
                {
                    m_dataAddress &= ~0x41f;
                    m_dataAddress |= m_tempAddress & 0x41f;
                }
                else if (m_cycle > 280 && m_cycle <= 304 && m_showBackground && m_showSprites)
                {
                    m_dataAddress &= ~0x7be0;
                    m_dataAddress |= m_tempAddress & 0x7be0;
                }
               if (m_cycle >= ScanlineEndCycle - (!m_evenFrame && m_showBackground && m_showSprites))
                {
                    m_pipelineState = Render;
                    m_cycle = m_scanline = 0;
                }

                if(m_cycle==260 && m_showBackground && m_showSprites){
                    m_bus.scanlineIRQ();
                }
                break;
            case Render:
                if (m_cycle > 0 && m_cycle <= ScanlineVisibleDots)
                {
                    Byte bgColor = 0, sprColor = 0;
                    bool bgOpaque = false, sprOpaque = true;
                    bool spriteForeground = false;

                    int x = m_cycle - 1;
                    int y = m_scanline;

                    if (m_showBackground)
                    {
                        auto x_fine = (m_fineXScroll + x) % 8;
                        if (!m_hideEdgeBackground || x >= 8)
                        {
                            auto addr = 0x2000 | (m_dataAddress & 0x0FFF);
                            Byte tile = read(addr);

                            addr = (tile * 16) + ((m_dataAddress >> 12) & 0x7);
                            addr |= m_bgPage << 12;
                            bgColor = (read(addr) >> (7 ^ x_fine)) & 1;
                            bgColor |= ((read(addr + 8) >> (7 ^ x_fine)) & 1) << 1;

                            bgOpaque = bgColor;

                            addr = 0x23C0 | (m_dataAddress & 0x0C00) | ((m_dataAddress >> 4) & 0x38)
                                        | ((m_dataAddress >> 2) & 0x07);
                            auto attribute = read(addr);
                            int shift = ((m_dataAddress >> 4) & 4) | (m_dataAddress & 2);
                            bgColor |= ((attribute >> shift) & 0x3) << 2;
                        }
                        if (x_fine == 7)
                        {
                            if ((m_dataAddress & 0x001F) == 31)
                            {
                                m_dataAddress &= ~0x001F;
                                m_dataAddress ^= 0x0400;
                            }
                            else
                            {
                                m_dataAddress += 1;
                            }
                        }
                    }

                    if (m_showSprites && (!m_hideEdgeSprites || x >= 8))
                    {
                        for (auto i : m_scanlineSprites)
                        {
                            Byte spr_x =     m_spriteMemory[i * 4 + 3];

                            if (0 > x - spr_x || x - spr_x >= 8)
                                continue;

                            Byte spr_y     = m_spriteMemory[i * 4 + 0] + 1,
                                 tile      = m_spriteMemory[i * 4 + 1],
                                 attribute = m_spriteMemory[i * 4 + 2];

                            int length = (m_longSprites) ? 16 : 8;

                            int x_shift = (x - spr_x) % 8, y_offset = (y - spr_y) % length;

                            if ((attribute & 0x40) == 0)
                                x_shift ^= 7;
                            if ((attribute & 0x80) != 0)
                                y_offset ^= (length - 1);

                            Address addr = 0;

                            if (!m_longSprites)
                            {
                                addr = tile * 16 + y_offset;
                                if (m_sprPage == High) addr += 0x1000;
                            }
                            else
                            {
                                y_offset = (y_offset & 7) | ((y_offset & 8) << 1);
                                addr = (tile >> 1) * 32 + y_offset;
                                addr |= (tile & 1) << 12;
                            }

                            sprColor |= (read(addr) >> (x_shift)) & 1;
                            sprColor |= ((read(addr + 8) >> (x_shift)) & 1) << 1;

                            if (!(sprOpaque = sprColor))
                            {
                                sprColor = 0;
                                continue;
                            }

                            sprColor |= 0x10;
                            sprColor |= (attribute & 0x3) << 2;

                            spriteForeground = !(attribute & 0x20);

                            if (!m_sprZeroHit && m_showBackground && i == 0 && sprOpaque && bgOpaque)
                            {
                                m_sprZeroHit = true;
                            }

                            break;
                        }
                    }

                    Byte paletteAddr = bgColor;

                    if ( (!bgOpaque && sprOpaque) ||
                         (bgOpaque && sprOpaque && spriteForeground) )
                        paletteAddr = sprColor;
                    else if (!bgOpaque && !sprOpaque)
                        paletteAddr = 0;

                    m_pictureBuffer[x][y] = sf::Color(colors[m_bus.readPalette(paletteAddr)]);
                }
                else if (m_cycle == ScanlineVisibleDots + 1 && m_showBackground)
                {
                    if ((m_dataAddress & 0x7000) != 0x7000)
                        m_dataAddress += 0x1000;
                    else
                    {
                        m_dataAddress &= ~0x7000;
                        int y = (m_dataAddress & 0x03E0) >> 5;
                        if (y == 29)
                        {
                            y = 0;
                            m_dataAddress ^= 0x0800;
                        }
                        else if (y == 31)
                            y = 0;
                        else
                            y += 1;
                        m_dataAddress = (m_dataAddress & ~0x03E0) | (y << 5);
                    }
                }
                else if (m_cycle == ScanlineVisibleDots + 2 && m_showBackground && m_showSprites)
                {
                    m_dataAddress &= ~0x41f;
                    m_dataAddress |= m_tempAddress & 0x41f;
                }

                if(m_cycle==260 && m_showBackground && m_showSprites){
                    m_bus.scanlineIRQ();
                }

                if (m_cycle >= ScanlineEndCycle)
                {
                    m_scanlineSprites.resize(0);

                    int range = 8;
                    if (m_longSprites)
                    {
                        range = 16;
                    }

                    std::size_t j = 0;
                    for (std::size_t i = m_spriteDataAddress / 4; i < 64; ++i)
                    {
                        auto diff = (m_scanline - m_spriteMemory[i * 4]);
                        if (0 <= diff && diff < range)
                        {
                            if (j >= 8)
                            {
                                m_spriteOverflow = true;
                                break;
                            }
                            m_scanlineSprites.push_back(i);
                            ++j;
                        }
                    }

                    ++m_scanline;
                    m_cycle = 0;
                }

                if (m_scanline >= VisibleScanlines)
                    m_pipelineState = PostRender;

                break;
            case PostRender:
                if (m_cycle >= ScanlineEndCycle)
                {
                    ++m_scanline;
                    m_cycle = 0;
                    m_pipelineState = VerticalBlank;

                    for (std::size_t x = 0; x < m_pictureBuffer.size(); ++x)
                    {
                        for (std::size_t y = 0; y < m_pictureBuffer[0].size(); ++y)
                        {
                            m_screen.setPixel(x, y, m_pictureBuffer[x][y]);
                        }
                    }

                }

                break;
            case VerticalBlank:
                if (m_cycle == 1 && m_scanline == VisibleScanlines + 1)
                {
                    m_vblank = true;
                    if (m_generateInterrupt) m_vblankCallback();
                }

                if (m_cycle >= ScanlineEndCycle)
                {
                    ++m_scanline;
                    m_cycle = 0;
                }

                if (m_scanline >= FrameEndScanline)
                {
                    m_pipelineState = PreRender;
                    m_scanline = 0;
                    m_evenFrame = !m_evenFrame;
                }

                break;
            default:
                LOG(Error) << "Well, this shouldn't have happened." << std::endl;
        }

        ++m_cycle;
    }
}