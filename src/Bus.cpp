#include <iostream>
#include <vector>
#include "../include/Bus.hpp"
#include "../include/Typedefs.hpp"

Bus::Bus() {
    // Connect CPU to communication bus
    cpu.ConnectBus(this);

    for (auto &i : ram) {
        i = 0x00;
    }
}

Bus::~Bus() {

}

void Bus::write(Address addr, Byte data) {
    if (addr >= 0x0000 && addr <= 0xFFFF) {
        ram[addr] = data;
    }
}

Byte Bus::read(Address addr, bool bReadOnly) {
    if (addr >= 0x0000 && addr <= 0xFFFF) {
        return ram[addr];
    }

    return 0x00;
}


