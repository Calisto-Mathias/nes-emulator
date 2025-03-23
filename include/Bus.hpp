#ifndef BUS_HPP
#define BUS_HPP

#include <stdint.h>
#include <array>
#include "CPU.hpp"

class Bus {
    public:
        Bus();
        ~Bus();
        // Devices on the bus
        CPU cpu;
        std::array<uint8_t, 64 * 1024> ram;
        // Read and write functions
        void write(uint16_t addr, uint8_t data);
        uint8_t read(uint16_t addr, bool bReadOnly = false);
};

#endif