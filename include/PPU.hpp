#ifndef PPU_HPP
#define PPU_HPP

#include <cstdint>
#include <memory>
#include <array>
#include "Typedefs.hpp"
#include "Cartridge.hpp"

struct ObjectAttributeEntry
{
    union
    {
        struct
        {
            uint8_t y;         // Y position of sprite
            uint8_t id;        // Tile ID
            uint8_t attribute; // Attributes
            uint8_t x;         // X position of sprite
        };
        uint8_t byte_value[4];
    };
};

class PPU
{
public:
    PPU();
    ~PPU();

    // Communication with CPU
    void cpuWrite(Address addr, Byte data);
    Byte cpuRead(Address addr, bool bReadOnly = false);

    // Communication with PPU bus
    void ppuWrite(Address addr, Byte data);
    Byte ppuRead(Address addr, bool bReadOnly = false);

    void ConnectCartridge(const std::shared_ptr<Cartridge> &cartridge);
    void clock();

    // New functions for rendering
    void reset();
    bool frameComplete();

    // Screen output
    uint8_t *GetScreen();
    uint8_t *GetNameTable(uint8_t i);
    uint8_t *GetPatternTable(uint8_t i, uint8_t palette);

    // Color palettes
    uint32_t GetColourFromPaletteRam(uint8_t palette, uint8_t pixel);
    bool nmi = false;

private:
    // Cartridge
    std::shared_ptr<Cartridge> cart;

    // Tables
    std::array<uint8_t, 2 * 1024> tblName;
    std::array<uint8_t, 32> tblPalette;
    std::array<uint8_t, 256 * 240> sprScreen;
    std::array<uint8_t, 2 * 128 * 128> sprNameTable;
    std::array<uint8_t, 2 * 128 * 128> sprPatternTable;

    // Internal registers
    union
    {
        struct
        {
            uint8_t nametable_x : 1;
            uint8_t nametable_y : 1;
            uint8_t increment_mode : 1;
            uint8_t pattern_sprite : 1;
            uint8_t pattern_background : 1;
            uint8_t sprite_size : 1;
            uint8_t slave_mode : 1;
            uint8_t enable_nmi : 1;
        };
        uint8_t reg;
    } control;

    union
    {
        struct
        {
            uint8_t grayscale : 1;
            uint8_t render_background_left : 1;
            uint8_t render_sprites_left : 1;
            uint8_t render_background : 1;
            uint8_t render_sprites : 1;
            uint8_t enhance_red : 1;
            uint8_t enhance_green : 1;
            uint8_t enhance_blue : 1;
        };
        uint8_t reg;
    } mask;

    union
    {
        struct
        {
            uint8_t unused : 5;
            uint8_t sprite_overflow : 1;
            uint8_t sprite_zero_hit : 1;
            uint8_t vertical_blank : 1;
        };
        uint8_t reg;
    } status;

    // Addresses for PPU bus operations
    uint8_t address_latch = 0x00;
    uint8_t ppu_data_buffer = 0x00;
    uint16_t ppu_address = 0x0000;

    // Background rendering
    uint8_t bg_next_tile_id = 0x00;
    uint8_t bg_next_tile_attrib = 0x00;
    uint8_t bg_next_tile_lsb = 0x00;
    uint8_t bg_next_tile_msb = 0x00;
    uint16_t bg_shifter_pattern_lo = 0x0000;
    uint16_t bg_shifter_pattern_hi = 0x0000;
    uint16_t bg_shifter_attrib_lo = 0x0000;
    uint16_t bg_shifter_attrib_hi = 0x0000;

    // Sprites
    ObjectAttributeEntry OAM[64];
    uint8_t oam_addr = 0x00;

    // Sprite rendering
    ObjectAttributeEntry spriteScanline[8];
    uint8_t sprite_count;
    uint8_t sprite_shifter_pattern_lo[8];
    uint8_t sprite_shifter_pattern_hi[8];

    // Timing controls
    int16_t scanline = 0;
    int16_t cycle = 0;
    bool frame_complete = false;

    // System palette (NES colors)
    std::array<uint32_t, 64> systemPalette = {
        0xFF545454, 0xFF001E74, 0xFF081090, 0xFF300088, 0xFF440064, 0xFF5C0030, 0xFF540400, 0xFF3C1800,
        0xFF202A00, 0xFF083A00, 0xFF004000, 0xFF003C00, 0xFF00323C, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFF989698, 0xFF084CC4, 0xFF3032EC, 0xFF5C1EE4, 0xFF8814B0, 0xFFA01464, 0xFF982220, 0xFF783C00,
        0xFF545A00, 0xFF287200, 0xFF087C00, 0xFF007628, 0xFF006678, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFFECEEEC, 0xFF4C9AEC, 0xFF787CEC, 0xFFB062EC, 0xFFE454EC, 0xFFEC58B4, 0xFFEC6A64, 0xFFD48820,
        0xFFA0AA00, 0xFF74C400, 0xFF4CD020, 0xFF38CC6C, 0xFF38B4CC, 0xFF3C3C3C, 0xFF000000, 0xFF000000,
        0xFFECEEEC, 0xFFA8CCEC, 0xFFBCBCEC, 0xFFD4B2EC, 0xFFECAEEC, 0xFFECAED4, 0xFFECB4B0, 0xFFE4C490,
        0xFFCCD278, 0xFFB4DE78, 0xFFA8E290, 0xFF98E2B4, 0xFFA0D6E4, 0xFFA0A2A0, 0xFF000000, 0xFF000000
    };

    // Helper functions
    uint8_t GetBit(uint8_t data, uint8_t bit);
};

#endif