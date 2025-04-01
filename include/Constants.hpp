#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <cstdint>
#include <utility>

constexpr std::pair<uint16_t, uint16_t> MEMORY_UNIT = {0x0000, 0xFFFF};
constexpr uint32_t MEMORY_SIZE = static_cast<uint32_t>(MEMORY_UNIT.second) - 
                               static_cast<uint32_t>(MEMORY_UNIT.first) + 1;

// PPU Memory Maps
constexpr std::pair<uint16_t, uint16_t> PPU_GRAPHICS_MEMORY = {0x0000, 0x1FFF};
constexpr uint16_t PPU_GRAPHICS_SIZE = PPU_GRAPHICS_MEMORY.second - PPU_GRAPHICS_MEMORY.first + 1;

constexpr std::pair<uint16_t, uint16_t> APU_UNIT = { 0x4000, 0x4017 };
constexpr uint16_t APU_SIZE = APU_UNIT.second - APU_UNIT.first + 1;

constexpr std::pair<uint16_t, uint16_t> PPU_UNIT = { 0x2000, 0x2007 };
constexpr uint16_t PPU_SIZE = PPU_UNIT.second - PPU_UNIT.first + 1;

constexpr std::pair<uint16_t, uint16_t> CARTRIDGE_UNIT = { 0x4020, 0xFFFF };
constexpr uint16_t CARTRIDGE_SIZE = CARTRIDGE_UNIT.second - CARTRIDGE_UNIT.first + 1;

constexpr std::pair<uint16_t, uint16_t> PPU_VRAM_UNIT = { 0x2000, 0x27FF };
constexpr uint16_t PPU_VRAM_SIZE = PPU_VRAM_UNIT.second - PPU_VRAM_UNIT.first + 1;

constexpr std::pair<uint16_t, uint16_t> PPU_PALLETES_UNIT = { 0x3F00, 0x3FFF };
constexpr uint16_t PPU_PALLETES_SIZE = PPU_PALLETES_UNIT.second - PPU_PALLETES_UNIT.first + 1;

constexpr uint16_t NUMBER_OF_LEGAL_INSTRUCTIONS = 256;

#endif