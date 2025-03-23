#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <cstdint>
#include <utility>

constexpr std::pair<uint8_t, uint8_t> MEMORY_UNIT = { 0x0000, 0x07FF };
constexpr uint8_t MEMORY_SIZE = MEMORY_UNIT.second - MEMORY_UNIT.first + 1;

constexpr std::pair<uint8_t, uint8_t> APU_UNIT = { 0x4000, 0x4017 };
constexpr uint8_t APU_SIZE = APU_UNIT.second - APU_UNIT.first + 1;

constexpr std::pair<uint8_t, uint8_t> PPU_UNIT = { 0x2000, 0x2007 };
constexpr uint8_t PPU_SIZE = PPU_UNIT.second - PPU_UNIT.first + 1;

constexpr std::pair<uint8_t, uint8_t> CARTRIDGE_UNIT = { 0x4020, 0xFFFF };
constexpr uint8_t CARTRIDGE_SIZE = CARTRIDGE_UNIT.second - CARTRIDGE_UNIT.first + 1;

constexpr std::pair<uint8_t, uint8_t> PPU_GRAPHICS_MEMORY = { 0x0000, 0x0FFF };
constexpr uint8_t PPU_GRAPHICS_SIZE = PPU_GRAPHICS_MEMORY.second - PPU_GRAPHICS_MEMORY.first + 1;

constexpr std::pair<uint8_t, uint8_t> PPU_VRAM_UNIT = { 0x2000, 0x27FF };
constexpr uint8_t PPU_VRAM_SIZE = PPU_VRAM_UNIT.second - PPU_VRAM_UNIT.first + 1;

constexpr std::pair<uint8_t, uint8_t> PPU_PALLETES_UNIT = { 0x3F00, 0x3FFF };
constexpr uint8_t PPU_PALLETES_SIZE = PPU_PALLETES_UNIT.second - PPU_PALLETES_UNIT.first + 1;

constexpr uint8_t NUMBER_OF_LEGAL_INSTRUCTIONS = 56;

#endif