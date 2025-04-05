#ifndef PPU_H
#define PPU_H
#include <array>
#include <functional>
#include "PictureBus.h"
#include "MainBus.h"
#include "VirtualScreen.h"
#include "PaletteColors.h"

namespace sn
{
    const int FrameEndScanline = 261;
    const int ScanlineVisibleDots = 256;
    const int ScanlineCycleLength = 341;
    const int ScanlineEndCycle = 340;
    const int VisibleScanlines = 240;
    const int AttributeOffset = 0x3C0;

    class PPU
    {
    public:
        PPU(PictureBus &bus, VirtualScreen &screen);
        
        Byte getData();
        Byte getStatus();
        Byte getOAMData();
        void setData(Byte data);
        void reset();
        void step();
        void control(Byte ctrl);
        void setMask(Byte mask);
        void setScroll(Byte scroll);
        void setOAMData(Byte value);
        void setOAMAddress(Byte addr);
        void setDataAddress(Byte addr);
        void doDMA(const Byte* page_ptr);
        void setInterruptCallback(std::function<void(void)> cb);
        
    private:
        enum State
        {
            VerticalBlank,
            PreRender,
            Render,
            PostRender
        } m_pipelineState;
        
        enum CharacterPage
        {
            Low,
            High,
        } m_bgPage, m_sprPage;
        
        void writeOAM(Byte addr, Byte value);
        Byte readOAM(Byte addr);
        Byte read(Address addr);
        
        int m_cycle;
        int m_scanline;
        Byte m_dataBuffer;
        Byte m_spriteDataAddress;
        bool m_evenFrame;
        bool m_vblank;
        bool m_sprZeroHit;
        bool m_spriteOverflow;
        bool m_firstWrite;
        bool m_longSprites;
        bool m_generateInterrupt;
        bool m_greyscaleMode;
        bool m_showSprites;
        bool m_showBackground;
        bool m_hideEdgeSprites;
        bool m_hideEdgeBackground;
        
        Address m_dataAddress;
        Address m_tempAddress;
        Byte m_fineXScroll;
        Address m_dataAddrIncrement;
        
        std::function<void(void)> m_vblankCallback;
        std::vector<Byte> m_spriteMemory;
        std::vector<Byte> m_scanlineSprites;
        std::vector<std::vector<sf::Color>> m_pictureBuffer;
        
        PictureBus &m_bus;
        VirtualScreen &m_screen;
    };
}

#endif
