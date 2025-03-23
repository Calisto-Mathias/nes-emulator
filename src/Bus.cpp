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

Byte Bus::cpuRead(Address addr, bool bReadOnly) {
    Byte data = 0x00;
    if (cart->cpuRead(addr, data)) {
        // The cartridge "may" handle the read
    } else if (addr >= 0x0000 && addr <= 0x1FFF) {
        return cpuRam[addr & MEMORY_UNIT.second];
    } else if (addr >= 0x2000 && addr <= 0x3FFF) {
        return ppu.cpuRead(addr & 0x0007, bReadOnly);
    }

    return 0x00;
}

void Bus::insertCartridge(const std::shared_ptr<Cartridge>& cartridge) {
    cart = cartridge;
    ppu.ConnectCartridge(cart);
}

void Bus::reset() {
    cpu.Reset();
    nSystemClockCounter = 0;
}