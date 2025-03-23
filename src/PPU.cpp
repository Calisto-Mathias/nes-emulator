#include "../include/PPU.hpp"
#include "../include/Typedefs.hpp"
#include "../include/Constants.hpp"
#include "../include/Bus.hpp"

PPU::PPU() {
    // Initialize the PPU
}

PPU::~PPU() {
    // Deinitialize the PPU
}

void PPU::cpuWrite(Address addr, Byte data) {
    switch (addr)
	{
	case 0x0000: // Control
		break;
	case 0x0001: // Mask
		break;
	case 0x0002: // Status
		break;
	case 0x0003: // OAM Address
		break;
	case 0x0004: // OAM Data
		break;
	case 0x0005: // Scroll
		break;
	case 0x0006: // PPU Address
		break;
	case 0x0007: // PPU Data
		break;
	}
}

Byte PPU::cpuRead(Address addr, bool bReadOnly) {
    Byte data = 0x00;
	addr &= 0x3FFF;

	return data;
}

void PPU::ppuWrite(Address addr, Byte data) {
    addr &= 0x3FFF;
    if (cart->ppuWrite(addr, data)) {
        // The cartridge "may" handle the write
    } else if (addr >= 0x0000 && addr <= 0x1FFF) {
        if (cart->nCHRBanks == 0) {
            tblPattern[0][addr] = data;
        }
    } else if (addr >= 0x2000 && addr <= 0x3EFF) {
        addr &= 0x0FFF;
        if (cart->nCHRBanks == 0) {
            tblPattern[0][addr] = data;
        }
    } else if (addr >= 0x3F00 && addr <= 0x3FFF) {
        addr &= 0x001F;
        if (addr == 0x0010) addr = 0x0000;
        if (addr == 0x0014) addr = 0x0004;
        if (addr == 0x0018) addr = 0x0008;
        if (addr == 0x001C) addr = 0x000C;
        tblPalette[addr] = data;
    }
}

Byte PPU::ppuRead(Address addr, bool bReadOnly) {
    // Read data from the PPU
    Byte data = 0x00;
    addr &= 0x3FFF;

    if (cart->ppuRead(addr, data)) {
        // The cartridge "may" handle the read
    } else if (addr >= 0x0000 && addr <= 0x1FFF) {
        return cart->ppuRead(addr, data);
    } else if (addr >= 0x2000 && addr <= 0x3EFF) {
        addr &= 0x0FFF;
        if (cart->nCHRBanks == 0) {
            data = tblPattern[0][addr];
        }
    } else if (addr >= 0x3F00 && addr <= 0x3FFF) {
        addr &= 0x001F;
        if (addr == 0x0010) addr = 0x0000;
        if (addr == 0x0014) addr = 0x0004;
        if (addr == 0x0018) addr = 0x0008;
        if (addr == 0x001C) addr = 0x000C;
        return tblPalette[addr] & 0x3F;
    }

    return 0x00;
}

void PPU::ConnectCartridge(const std::shared_ptr<Cartridge>& cartridge) {
    cart = cartridge;
}