#ifndef BUS_HPP
#define BUS_HPP

#include <cstdint>
#include <array>

class CPU;
class Cartridge;
class PPU;

class Bus
{
public:
	Bus();
	~Bus();

public:
	CPU cpu;	
    PPU ppu;
	std::shared_ptr<Cartridge> cart;
	Byte cpuRam[MEMORY_SIZE];

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