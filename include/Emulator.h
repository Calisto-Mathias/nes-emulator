#ifndef EMULATOR_H
#define EMULATOR_H
#include <chrono>
#include <SFML/Graphics.hpp>

#include "CPU.h"
#include "PPU.h"
#include "MainBus.h"
#include "PictureBus.h"
#include "Controller.h"

namespace sn
{
    const int NESVideoWidth = ScanlineVisibleDots;
    const int NESVideoHeight = VisibleScanlines;
    
    using TimePoint = std::chrono::high_resolution_clock::time_point;

    class Emulator
    {
    public:
        Emulator();
        
        void setKeys(std::vector<sf::Keyboard::Key>& p1, std::vector<sf::Keyboard::Key>& p2);
        void setVideoWidth(int width);
        void setVideoHeight(int height);
        void setVideoScale(float scale);
        void run(std::string rom_path);
        
    private:
        void DMA(Byte page);
    
        std::chrono::high_resolution_clock::duration m_elapsedTime;
        std::chrono::nanoseconds m_cpuCycleDuration;
        TimePoint m_cycleTimer;
        float m_screenScale;
        
        VirtualScreen m_emulatorScreen;
        sf::RenderWindow m_window;
        Controller m_controller1, m_controller2;
        
        std::unique_ptr<Mapper> m_mapper;
        Cartridge m_cartridge;
        PPU m_ppu;
        CPU m_cpu;
        PictureBus m_pictureBus;
        MainBus m_bus;
    };
}
#endif // EMULATOR_H
