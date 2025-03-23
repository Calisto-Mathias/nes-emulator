#ifndef PPU_HPP
#define PPU_HPP

class PPU
{
public:
    PPU();
    ~PPU();

    void cpuWrite(Address, Byte);
	Byte cpuRead(Address, bool bReadOnly = false);

    void ppuWrite(Address, Byte);
    Byte ppuRead(Address, bool bReadOnly = false);

    void ConnectCartridge(const std::shared_ptr<Cartridge>& cartridge);
    void clock();

private:
    std::shared_ptr<Cartridge> cart;
    std::array<std::array<Byte, 1024>, 2> tblName;
    std::array<std::array<Byte, 4096>, 2> tblPattern;
    std::array<Byte, 32> tblPalette;
};

#endif