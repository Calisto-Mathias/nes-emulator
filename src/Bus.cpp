#include <iostream>
#include <vector>
#include "../include/Bus.hpp"
#include "../include/Typedefs.hpp"

Bus::Bus() {
    // Connect CPU to communication bus
    cpu.ConnectBus(this);

    for (auto &i : cpuRam) {
        i = 0x00;
    }

    cart = nullptr;

    reset();
}

void Bus::reset() {
    cpu.reset();
    ppu.reset();
    nSystemClockCounter = 0;
    
    // Make sure to initialize controller states
    controller1 = 0x00;
    controller2 = 0x00;
}

Bus::~Bus() {

}

void Bus::cpuWrite(Address addr, Byte data) {
    if (cart->cpuWrite(addr, data)) {
        // The cartridge "may" handle the write
    } else if (addr >= 0x0000 && addr <= 0x1FFF) {
        cpuRam[addr & MEMORY_UNIT.second] = data;
    } else if (addr >= 0x2000 && addr <= 0x3FFF) {
        ppu.cpuWrite(addr & 0x0007, data);
    }
}

uint8_t Bus::cpuRead(uint16_t addr, bool readOnly) {
    uint8_t data = 0;
    
    if (cart && cart->cpuRead(addr, data)) {
        // Cartridge address space
    }
    else if (addr >= 0x0000 && addr <= 0x1FFF) {
        // System RAM, mirrored every 2048 bytes
        data = cpuRam[addr & 0x07FF];
    }
    else if (addr >= 0x2000 && addr <= 0x3FFF) {
        // PPU registers, mirrored every 8 bytes
        data = ppu.cpuRead(addr & 0x0007, readOnly);
    }
    else if (addr >= 0x4000 && addr <= 0x4017) {
        // APU and I/O registers
        if (addr == 0x4016) {
            // Controller 1
            data = (controller1 & 0x80) > 0;
            controller1 <<= 1;
        }
    }
    
    return data;
}

void Bus::insertCartridge(const std::shared_ptr<Cartridge>& cartridge) {
    cart = cartridge;
    ppu.ConnectCartridge(cart);
}

void Bus::reset() {
    cpu.Reset();
    nSystemClockCounter = 0;
}

void Bus::clock() {
    // PPU runs at 3x the rate of the CPU
    ppu.clock();
    
    // CPU is clocked every 3 PPU cycles
    if (nSystemClockCounter % 3 == 0) {
        cpu.Clock();
    }
    
    // If the PPU has generated an NMI, tell the CPU
    if (ppu.nmi) {
        ppu.nmi = false;
        cpu.NMI();
    }
    
    nSystemClockCounter++;
}