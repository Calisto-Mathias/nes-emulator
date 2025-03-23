#ifndef BUS_HPP
#define BUS_HPP

#include <memory>
#include <array>

#include "Typedefs.hpp"
#include "PPU.hpp"
#include "Cartridge.hpp"
#include "CPU.hpp"

class Bus
{
public:
	Bus();
	~Bus();

public:
	CPU cpu;	
    PPU ppu;
	std::shared_ptr<Cartridge> cart;
	std::array<Byte, MEMORY_SIZE> cpuRam;

public:
	void cpuWrite(Address, Byte);
	Byte cpuRead(Address, bool bReadOnly = false);

private:
	uint32_t nSystemClockCounter = 0;

public:
	void insertCartridge(const std::shared_ptr<Cartridge>& cartridge);
	void reset();
	void clock();
};

#endif