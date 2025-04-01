#include "../include/PPU.hpp"
#include "../include/Bus.hpp"
#include <cstring>

// NES system palette - maps color indices to actual RGB values
const uint32_t systemPalette[64] = {
    0x545454, 0x001E74, 0x081090, 0x300088, 0x440064, 0x5C0030, 0x540400, 0x3C1800,
    0x202A00, 0x083A00, 0x004000, 0x003C00, 0x00323C, 0x000000, 0x000000, 0x000000,
    0x989698, 0x084CC4, 0x3032EC, 0x5C1EE4, 0x8814B0, 0xA01464, 0x982220, 0x783C00,
    0x545A00, 0x287200, 0x087C00, 0x007628, 0x006678, 0x000000, 0x000000, 0x000000,
    0xECEEEC, 0x4C9AEC, 0x787CEC, 0xB062EC, 0xE454EC, 0xEC58B4, 0xEC6A64, 0xD48820,
    0xA0AA00, 0x74C400, 0x4CD020, 0x38CC6C, 0x38B4CC, 0x3C3C3C, 0x000000, 0x000000,
    0xECEEEC, 0xA8CCEC, 0xBCBCEC, 0xD4B2EC, 0xECAEEC, 0xECAED4, 0xECB4B0, 0xE4C490,
    0xCCD278, 0xB4DE78, 0xA8E290, 0x98E2B4, 0xA0D6E4, 0xA0A2A0, 0x000000, 0x000000
};

PPU::PPU()
{
    // Clear memory
    tblName.fill(0);
    tblPalette.fill(0);
    sprScreen.fill(0);
    sprNameTable.fill(0);
    sprPatternTable.fill(0);
    
    // Initialize registers
    control.reg = 0x00;
    mask.reg = 0x00;
    status.reg = 0x00;
    
    // Initialize internal state
    address_latch = 0x00;
    ppu_data_buffer = 0x00;
    ppu_address = 0x0000;
    scanline = 0;
    cycle = 0;
    bg_next_tile_id = 0x00;
    bg_next_tile_attrib = 0x00;
    bg_next_tile_lsb = 0x00;
    bg_next_tile_msb = 0x00;
    bg_shifter_pattern_lo = 0x0000;
    bg_shifter_pattern_hi = 0x0000;
    bg_shifter_attrib_lo = 0x0000;
    bg_shifter_attrib_hi = 0x0000;
    frame_complete = false;
    
    // Clear OAM
    oam_addr = 0x00;
    for (auto& entry : OAM) {
        entry.x = 0;
        entry.y = 0xFF;
        entry.id = 0;
        entry.attribute = 0;
    }
}

PPU::~PPU()
{
    // Nothing specific to clean up
}

void PPU::ConnectCartridge(const std::shared_ptr<Cartridge>& cartridge)
{
    this->cart = cartridge;
}

void PPU::reset()
{
    // Reset internal registers
    control.reg = 0x00;
    mask.reg = 0x00;
    status.reg = 0x00;
    
    // Reset PPU addressing
    address_latch = 0x00;
    ppu_data_buffer = 0x00;
    ppu_address = 0x0000;
    
    // Reset PPU timing
    scanline = 0;
    cycle = 0;
    frame_complete = false;
    nmi = false;
}

bool PPU::frameComplete()
{
    bool temp = frame_complete;
    frame_complete = false;
    return temp;
}

void PPU::cpuWrite(Address addr, Byte data)
{
    switch (addr & 0x0007)
    {
        case 0x0000: // Control
            control.reg = data;
            break;
            
        case 0x0001: // Mask
            mask.reg = data;
            break;
            
        case 0x0002: // Status
            // Status register is read-only
            break;
            
        case 0x0003: // OAM Address
            oam_addr = data;
            break;
            
        case 0x0004: // OAM Data
            // Assuming OAM is an array of ObjectAttributeEntry and data needs to be assigned to its fields
            OAM[oam_addr].id = data;
            oam_addr++;
            break;
            
        case 0x0005: // Scroll
            if (address_latch == 0) {
                // X scroll
                ppu_address = (ppu_address & 0xFFE0) | ((data & 0xF8) >> 3);
                address_latch = 1;
            }
            else {
                // Y scroll
                ppu_address = (ppu_address & 0xFC1F) | ((data & 0xF8) << 2);
                address_latch = 0;
            }
            break;
            
        case 0x0006: // PPU Address
            if (address_latch == 0) {
                // High byte
                ppu_address = (ppu_address & 0x00FF) | ((data & 0x3F) << 8);
                address_latch = 1;
            }
            else {
                // Low byte
                ppu_address = (ppu_address & 0xFF00) | data;
                address_latch = 0;
            }
            break;
            
        case 0x0007: // PPU Data
            ppuWrite(ppu_address, data);
            ppu_address += (control.increment_mode ? 32 : 1);
            break;
    }
}

Byte PPU::cpuRead(Address addr, bool bReadOnly)
{
    Byte data = 0x00;
    
    switch (addr & 0x0007)
    {
        case 0x0000: // Control - write only
            data = control.reg;
            break;
            
        case 0x0001: // Mask - write only
            data = mask.reg;
            break;
            
        case 0x0002: // Status
            data = (status.reg & 0xE0) | (ppu_data_buffer & 0x1F);
            if (!bReadOnly) {
                status.vertical_blank = 0;
                address_latch = 0;
            }
            break;
            
        case 0x0003: // OAM Address
            break;
            
        case 0x0004: // OAM Data
            data = OAM[oam_addr].id;
            break;
            
        case 0x0005: // Scroll - write only
            break;
            
        case 0x0006: // PPU Address - write only
            break;
            
        case 0x0007: // PPU Data
            data = ppu_data_buffer;
            if (!bReadOnly) {
                ppu_data_buffer = ppuRead(ppu_address, false);
                
                // Palette memory is accessed immediately
                if (ppu_address >= 0x3F00)
                    data = ppu_data_buffer;
                    
                ppu_address += (control.increment_mode ? 32 : 1);
            }
            break;
    }
    
    return data;
}

void PPU::ppuWrite(Address addr, Byte data)
{
    addr &= 0x3FFF;
    
    if (cart->ppuWrite(addr, data)) {
        // Cartridge address space
    }
    else if (addr >= 0x0000 && addr <= 0x1FFF) {
        // Pattern Memory - usually ROM on the cartridge
    }
    else if (addr >= 0x2000 && addr <= 0x3EFF) {
        // Nametable Memory
        addr &= 0x0FFF;
        tblName[addr] = data;
    }
    else if (addr >= 0x3F00 && addr <= 0x3FFF) {
        // Palette Memory
        addr &= 0x001F;
        
        // Mirror palette addresses
        if (addr == 0x0010) addr = 0x0000;
        if (addr == 0x0014) addr = 0x0004;
        if (addr == 0x0018) addr = 0x0008;
        if (addr == 0x001C) addr = 0x000C;
        
        tblPalette[addr] = data;
    }
}

Byte PPU::ppuRead(Address addr, bool bReadOnly)
{
    Byte data = 0x00;
    addr &= 0x3FFF;
    
    if (cart->ppuRead(addr, data)) {
        // Cartridge address space
    }
    else if (addr >= 0x0000 && addr <= 0x1FFF) {
        // Pattern tables
        data = cart->ppuRead(addr, data) ? data : 0x00;
    }
    else if (addr >= 0x2000 && addr <= 0x3EFF) {
        // Nametables
        addr &= 0x0FFF;
        data = tblName[addr];
    }
    else if (addr >= 0x3F00 && addr <= 0x3FFF) {
        // Palette RAM
        addr &= 0x001F;
        
        // Mirror addresses
        if (addr == 0x0010) addr = 0x0000;
        if (addr == 0x0014) addr = 0x0004;
        if (addr == 0x0018) addr = 0x0008;
        if (addr == 0x001C) addr = 0x000C;
        
        data = tblPalette[addr] & (mask.grayscale ? 0x30 : 0x3F);
    }
    
    return data;
}

void PPU::clock()
{
    // Implement pixel data fetching
    auto IncrementScrollX = [&]() {
        if (mask.render_background || mask.render_sprites) {
            // If rendering is enabled, update coarse X scroll
            if ((ppu_address & 0x001F) == 31) {
                // If at the end of a nametable, wrap around
                ppu_address &= ~0x001F;         // Clear coarse X
                ppu_address ^= 0x0400;          // Switch horizontal nametable
            } else {
                // Otherwise increment coarse X
                ppu_address += 1;
            }
        }
    };

    auto IncrementScrollY = [&]() {
        if (mask.render_background || mask.render_sprites) {
            // If rendering is enabled, update Y scroll
            // Fine Y increment
            uint16_t fine_y = (ppu_address & 0x7000) >> 12;
            if (fine_y == 7) {
                // Reset fine Y and increment coarse Y
                fine_y = 0;
                uint16_t coarse_y = (ppu_address & 0x03E0) >> 5;
                if (coarse_y == 29) {
                    // End of nametable, switch vertical nametable
                    coarse_y = 0;
                    ppu_address ^= 0x0800;
                } else if (coarse_y == 31) {
                    // Invalid state, wrap around without switching nametable
                    coarse_y = 0;
                } else {
                    // Normal increment
                    coarse_y++;
                }
                // Update coarse Y in address
                ppu_address = (ppu_address & ~0x03E0) | (coarse_y << 5);
            } else {
                // Increment fine Y
                fine_y++;
                ppu_address = (ppu_address & ~0x7000) | (fine_y << 12);
            }
        }
    };

    auto TransferAddressX = [&]() {
        if (mask.render_background || mask.render_sprites) {
            // Copy horizontal scroll bits from temporary to PPU address
            ppu_address = (ppu_address & ~0x041F) | (address_latch & 0x041F);
        }
    };

    auto TransferAddressY = [&]() {
        if (mask.render_background || mask.render_sprites) {
            // Copy vertical scroll bits from temporary to PPU address
            ppu_address = (ppu_address & ~0x7BE0) | (address_latch & 0x7BE0);
        }
    };

    auto LoadBackgroundShifters = [&]() {
        // Load the next tile data into the background shifters
        bg_shifter_pattern_lo = (bg_shifter_pattern_lo & 0xFF00) | bg_next_tile_lsb;
        bg_shifter_pattern_hi = (bg_shifter_pattern_hi & 0xFF00) | bg_next_tile_msb;
        bg_shifter_attrib_lo = (bg_shifter_attrib_lo & 0xFF00) | ((bg_next_tile_attrib & 0x01) ? 0xFF : 0x00);
        bg_shifter_attrib_hi = (bg_shifter_attrib_hi & 0xFF00) | ((bg_next_tile_attrib & 0x02) ? 0xFF : 0x00);
    };

    auto UpdateShifters = [&]() {
        if (mask.render_background) {
            // Shift background registers by 1
            bg_shifter_pattern_lo <<= 1;
            bg_shifter_pattern_hi <<= 1;
            bg_shifter_attrib_lo <<= 1;
            bg_shifter_attrib_hi <<= 1;
        }

        if (mask.render_sprites && cycle >= 1 && cycle < 258) {
            // Shift sprite shifters for active sprites
            for (int i = 0; i < sprite_count; i++) {
                if (spriteScanline[i].x > 0) {
                    spriteScanline[i].x--;
                } else {
                    sprite_shifter_pattern_lo[i] <<= 1;
                    sprite_shifter_pattern_hi[i] <<= 1;
                }
            }
        }
    };

    // Main PPU rendering state machine
    // Each scanline is 341 cycles, with multiple scanlines per frame
    if (scanline >= -1 && scanline < 240) {
        // Visible scanlines
        if (scanline == -1 && cycle == 1) {
            // Start of frame, clear vertical blank and other status flags
            status.vertical_blank = 0;
            status.sprite_zero_hit = 0;
            status.sprite_overflow = 0;

            // Clear sprite shifters
            for (int i = 0; i < 8; i++) {
                sprite_shifter_pattern_lo[i] = 0;
                sprite_shifter_pattern_hi[i] = 0;
            }
        }

        if ((cycle >= 2 && cycle < 258) || (cycle >= 321 && cycle < 338)) {
            UpdateShifters();

            // Background rendering sequence
            switch ((cycle - 1) % 8) {
                case 0:
                    LoadBackgroundShifters();
                    // Fetch next tile ID from nametable
                    {
                        uint16_t addr = 0x2000 | (ppu_address & 0x0FFF);
                        bg_next_tile_id = ppuRead(addr);
                    }
                    break;
                    
                case 2:
                    // Fetch next tile attribute from nametable
                    {
                        uint16_t addr = 0x23C0 | (ppu_address & 0x0C00) | 
                                      ((ppu_address >> 4) & 0x38) | 
                                      ((ppu_address >> 2) & 0x07);
                        uint8_t attr = ppuRead(addr);
                        
                        // Determine which quadrant of the attribute byte to use
                        int shift = ((ppu_address >> 4) & 4) | (ppu_address & 2);
                        bg_next_tile_attrib = (attr >> shift) & 0x03;
                    }
                    break;
                    
                case 4:
                    // Fetch next tile LSB from pattern table
                    {
                        uint16_t pattern_addr = (control.pattern_background << 12) + 
                                            (static_cast<uint16_t>(bg_next_tile_id) << 4) + 
                                            ((ppu_address >> 12) & 7);
                        bg_next_tile_lsb = ppuRead(pattern_addr);
                    }
                    break;
                    
                case 6:
                    // Fetch next tile MSB from pattern table
                    {
                        uint16_t pattern_addr = (control.pattern_background << 12) + 
                                            (static_cast<uint16_t>(bg_next_tile_id) << 4) + 
                                            ((ppu_address >> 12) & 7) + 8;
                        bg_next_tile_msb = ppuRead(pattern_addr);
                    }
                    break;
                    
                case 7:
                    // Increment horizontal position
                    IncrementScrollX();
                    break;
            }
        }

        // End of visible scanline
        if (cycle == 256) {
            IncrementScrollY();
        }
        
        if (cycle == 257) {
            LoadBackgroundShifters();
            TransferAddressX();
        }
        
        // Vertical blank logic
        if (scanline == -1 && cycle >= 280 && cycle < 305) {
            TransferAddressY();
        }
        
        // Sprite evaluation for next scanline
        if (cycle == 257 && scanline >= 0) {
            // Initialize sprite data for next scanline
            std::memset(spriteScanline, 0xFF, sizeof(spriteScanline));
            sprite_count = 0;
            
            // Find up to 8 sprites to render on next scanline
            uint8_t sprite_size = control.sprite_size ? 16 : 8;
            
            for (uint8_t i = 0; i < 64 && sprite_count < 8; ++i) {
                // Check if sprite is in range for next scanline
                int16_t diff = scanline - OAM[i].y;
                if (diff >= 0 && diff < sprite_size) {
                    // Sprite is visible on next scanline
                    if (sprite_count < 8) {
                        // Check for sprite zero
                        if (i == 0) {
                            // Sprite zero is visible
                        }
                        
                        // Copy to sprite scanline
                        memcpy(&spriteScanline[sprite_count], &OAM[i], sizeof(ObjectAttributeEntry));
                        sprite_count++;
                    }
                }
            }
        }
        
        if (cycle == 340) {
            // Prepare sprite shifters for the next scanline
            for (uint8_t i = 0; i < sprite_count; ++i) {
                uint8_t sprite_pattern_bits_lo, sprite_pattern_bits_hi;
                uint16_t sprite_pattern_addr;
                
                if (!control.sprite_size) {
                    // 8x8 sprites
                    sprite_pattern_addr = (control.pattern_sprite << 12) | 
                                         (spriteScanline[i].id << 4);
                    
                    // Handle vertical flipping
                    uint8_t row = scanline - spriteScanline[i].y;
                    if (spriteScanline[i].attribute & 0x80) {
                        row = 7 - row; // Vertical flip
                    }
                    
                    sprite_pattern_addr += row;
                } else {
                    // 8x16 sprites
                    sprite_pattern_addr = ((spriteScanline[i].id & 0x01) << 12);
                    
                    uint8_t row = scanline - spriteScanline[i].y;
                    if (spriteScanline[i].attribute & 0x80) {
                        row = 15 - row; // Vertical flip
                    }
                    
                    if (row > 7) {
                        // Bottom half of sprite
                        sprite_pattern_addr |= ((spriteScanline[i].id & 0xFE) + 1) << 4;
                        row -= 8;
                    } else {
                        // Top half of sprite
                        sprite_pattern_addr |= (spriteScanline[i].id & 0xFE) << 4;
                    }
                    
                    sprite_pattern_addr += row;
                }
                
                // Get pattern data
                sprite_pattern_bits_lo = ppuRead(sprite_pattern_addr);
                sprite_pattern_bits_hi = ppuRead(sprite_pattern_addr + 8);
                
                // Handle horizontal flipping
                if (spriteScanline[i].attribute & 0x40) {
                    // Horizontal flip - reverse the pattern bits
                    auto flipbyte = [](uint8_t b) -> uint8_t {
                        b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
                        b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
                        b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
                        return b;
                    };
                    
                    sprite_pattern_bits_lo = flipbyte(sprite_pattern_bits_lo);
                    sprite_pattern_bits_hi = flipbyte(sprite_pattern_bits_hi);
                }
                
                // Load into shifters
                sprite_shifter_pattern_lo[i] = sprite_pattern_bits_lo;
                sprite_shifter_pattern_hi[i] = sprite_pattern_bits_hi;
            }
        }
    }
    
    // Render the pixel based on registers
    if (scanline >= 0 && scanline < 240 && cycle >= 1 && cycle <= 256) {
        uint8_t bg_pixel = 0;
        uint8_t bg_palette = 0;
        
        if (mask.render_background) {
            // Get the current pixel from the shifters
            uint16_t bit_mux = 0x8000 >> 0; // Fine X offset here
            
            // Select pixel and palette
            uint8_t p0 = (bg_shifter_pattern_lo & bit_mux) > 0;
            uint8_t p1 = (bg_shifter_pattern_hi & bit_mux) > 0;
            bg_pixel = (p1 << 1) | p0;
            
            // Get the palette attribute bits
            uint8_t pal0 = (bg_shifter_attrib_lo & bit_mux) > 0;
            uint8_t pal1 = (bg_shifter_attrib_hi & bit_mux) > 0;
            bg_palette = (pal1 << 1) | pal0;
        }
        
        uint8_t fg_pixel = 0;
        uint8_t fg_palette = 0;
        uint8_t fg_priority = 0;
        
        if (mask.render_sprites) {
            // Check each sprite in scanline
            for (uint8_t i = 0; i < sprite_count; ++i) {
                if (spriteScanline[i].x == 0) {
                    // This sprite is active on current pixel
                    uint8_t sprite_pixel = 0;
                    uint8_t p0 = (sprite_shifter_pattern_lo[i] & 0x80) > 0;
                    uint8_t p1 = (sprite_shifter_pattern_hi[i] & 0x80) > 0;
                    sprite_pixel = (p1 << 1) | p0;
                    
                    // Only render if pixel is not transparent (0)
                    if (sprite_pixel != 0) {
                        // Get sprite palette and priority
                        fg_palette = (spriteScanline[i].attribute & 0x03) + 4;
                        fg_priority = (spriteScanline[i].attribute & 0x20) == 0;
                        fg_pixel = sprite_pixel;
                        break; // First non-transparent sprite wins
                    }
                }
            }
        }
        
        // Determine final pixel and palette
        uint8_t pixel = 0;
        uint8_t palette = 0;
        
        if (bg_pixel == 0 && fg_pixel == 0) {
            // Both transparent
            pixel = 0;
            palette = 0;
        } else if (bg_pixel == 0 && fg_pixel > 0) {
            // Only foreground
            pixel = fg_pixel;
            palette = fg_palette;
        } else if (bg_pixel > 0 && fg_pixel == 0) {
            // Only background
            pixel = bg_pixel;
            palette = bg_palette;
        } else {
            // Both background and foreground
            if (fg_priority) {
                // Foreground wins
                pixel = fg_pixel;
                palette = fg_palette;
            } else {
                // Background wins
                pixel = bg_pixel;
                palette = bg_palette;
            }
        }
        
        // Write final pixel to screen buffer
        sprScreen[(scanline * 256) + (cycle - 1)] = GetColourFromPaletteRam(palette, pixel);
    }
    
    // Advance renderer state
    cycle++;
    if (cycle >= 341) {
        cycle = 0;
        scanline++;
        
        if (scanline >= 261) {
            scanline = -1;
            frame_complete = true;
        }
    }
    
    // Check for NMI
    if (scanline == 241 && cycle == 1) {
        status.vertical_blank = 1;
        if (control.enable_nmi)
            nmi = true;
    }
}

uint8_t* PPU::GetScreen() {
    // Return pointer to the screen buffer for rendering
    return sprScreen.data();
}

uint8_t* PPU::GetNameTable(uint8_t i) {
    // Return pointer to the requested nametable
    // Ensure i is 0 or 1
    i = i & 0x01;
    return &sprNameTable[i * 128 * 128];
}

uint8_t* PPU::GetPatternTable(uint8_t i, uint8_t palette) {
    // Fill the pattern table with pixel data for the selected palette
    // i selects which pattern table (0 or 1)
    i = i & 0x01;
    
    // Loop through all 16x16 tiles in the pattern table
    for (uint16_t tileY = 0; tileY < 16; tileY++) {
        for (uint16_t tileX = 0; tileX < 16; tileX++) {
            // Calculate tile offset in pattern memory
            uint16_t offset = tileY * 256 + tileX * 16;
            
            // Process each row of the tile
            for (uint16_t row = 0; row < 8; row++) {
                // Get the bit planes for this row
                // Pattern tables are at 0x0000 (i=0) or 0x1000 (i=1)
                uint8_t tileLsb = ppuRead(i * 0x1000 + offset + row, true);
                uint8_t tileMsb = ppuRead(i * 0x1000 + offset + row + 8, true);
                
                // Process each pixel in the row
                for (uint16_t col = 0; col < 8; col++) {
                    // Determine the 2-bit pixel value (0, 1, 2, 3)
                    uint8_t pixel = ((tileMsb & 0x01) << 1) | (tileLsb & 0x01);
                    tileLsb >>= 1;
                    tileMsb >>= 1;
                    
                    // Calculate position in the output buffer
                    uint16_t x = tileX * 8 + (7 - col);  // Reverse order since we're shifting right
                    uint16_t y = tileY * 8 + row;
                    
                    // Store color value in pattern table buffer
                    // The format here is palette in the high nibble, pixel in the low nibble
                    sprPatternTable[i * 128 * 128 + y * 128 + x] = (palette << 4) | pixel;
                }
            }
        }
    }
    
    return &sprPatternTable[i * 128 * 128];
}

uint32_t PPU::GetColourFromPaletteRam(uint8_t palette, uint8_t pixel) {
    // Combine palette and pixel data to get the correct color from palette RAM
    // palette: 0-7 (0-3 for background, 4-7 for sprites)
    // pixel: 0-3 (color index within the palette)
    
    // Sanity check inputs
    palette &= 0x07;
    pixel &= 0x03;
    
    // Calculate address in palette memory
    // 0x3F00 is the base address of palette memory
    uint16_t addr = 0x3F00 + (palette << 2) + pixel;
    
    // Read the color value (0-63) from palette memory
    uint8_t colorIndex = ppuRead(addr, true) & 0x3F;  // Mask to 6 bits
    
    // Return the RGB value from the system palette
    return systemPalette[colorIndex];
}

uint8_t PPU::GetBit(uint8_t data, uint8_t bit) {
    // Helper function to extract a specific bit from a byte
    return (data >> bit) & 0x01;
}