#include <iostream>
#include <SDL2/SDL.h>
#include <memory>
#include <chrono>

#include "Bus.hpp"
#include "CPU.hpp"
#include "PPU.hpp"
#include "Cartridge.hpp"

// Screen dimensions
constexpr int SCREEN_WIDTH = 256 * 3;  // NES resolution x3
constexpr int SCREEN_HEIGHT = 240 * 3; // NES resolution x3
constexpr int PATTERN_WIDTH = 128 * 2; // Pattern table display
constexpr int PATTERN_HEIGHT = 128;

// Color palette - simplified NES colors
constexpr uint32_t NES_PALETTE[64] = {
    0xFF545454, 0xFF001E74, 0xFF081090, 0xFF300088, 0xFF440064, 0xFF5C0030, 0xFF540400, 0xFF3C1800,
    0xFF202A00, 0xFF083A00, 0xFF004000, 0xFF003C00, 0xFF00323C, 0xFF000000, 0xFF000000, 0xFF000000,
    0xFF989698, 0xFF084CC4, 0xFF3032EC, 0xFF5C1EE4, 0xFF8814B0, 0xFFA01464, 0xFF982220, 0xFF783C00,
    0xFF545A00, 0xFF287200, 0xFF087C00, 0xFF007628, 0xFF006678, 0xFF000000, 0xFF000000, 0xFF000000,
    0xFFECEEEC, 0xFF4C9AEC, 0xFF787CEC, 0xFFB062EC, 0xFFE454EC, 0xFFEC58B4, 0xFFEC6A64, 0xFFD48820,
    0xFFA0AA00, 0xFF74C400, 0xFF4CD020, 0xFF38CC6C, 0xFF38B4CC, 0xFF3C3C3C, 0xFF000000, 0xFF000000,
    0xFFECEEEC, 0xFFA8CCEC, 0xFFBCBCEC, 0xFFD4B2EC, 0xFFECAEEC, 0xFFECAED4, 0xFFECB4B0, 0xFFE4C490,
    0xFFCCD278, 0xFFB4DE78, 0xFFA8E290, 0xFF98E2B4, 0xFFA0D6E4, 0xFFA0A2A0, 0xFF000000, 0xFF000000};

class Emulator
{
public:
    Emulator() : running(false),
                 paused(false),
                 frameByFrame(false),
                 selectedPalette(0),
                 window(nullptr),
                 renderer(nullptr),
                 texture(nullptr),
                 patternTexture(nullptr)
    {
        // Initialize SDL
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
            return;
        }

        // Create window
        window = SDL_CreateWindow("NES Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                  SCREEN_WIDTH + PATTERN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (!window)
        {
            std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return;
        }

        // Create renderer
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer)
        {
            std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return;
        }

        // Create texture for main screen
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
                                    256, 240);
        if (!texture)
        {
            std::cerr << "Texture could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return;
        }

        // Create texture for pattern tables
        patternTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
                                           256, 128);
        if (!patternTexture)
        {
            std::cerr << "Pattern texture could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return;
        }

        // Create NES hardware
        cartridge = std::make_shared<Cartridge>();
        nes.insertCartridge(cartridge);
        nes.reset();
    }

    ~Emulator()
    {
        // Clean up SDL
        if (patternTexture)
            SDL_DestroyTexture(patternTexture);
        if (texture)
            SDL_DestroyTexture(texture);
        if (renderer)
            SDL_DestroyRenderer(renderer);
        if (window)
            SDL_DestroyWindow(window);
        SDL_Quit();
    }

    bool loadROM(const std::string& path) {
        cartridge = std::make_shared<Cartridge>();
        
        if (!cartridge->load(path)) {
            std::cerr << "Failed to load ROM: " << path << std::endl;
            cartridge = nullptr;
            return false;
        }
        
        // Connect cartridge to system
        nes.connectCartridge(cartridge);
        
        // Reset the system to apply the new cartridge
        nes.reset();
        
        return true;
    }

    void run()
    {
        if (!window || !renderer || !texture)
        {
            std::cerr << "Cannot run emulator - initialization failed" << std::endl;
            return;
        }

        running = true;

        // Main emulation loop
        auto lastTime = std::chrono::high_resolution_clock::now();
        uint32_t frameCount = 0;

        while (running)
        {
            // Handle events
            SDL_Event e;
            while (SDL_PollEvent(&e))
            {
                if (e.type == SDL_QUIT)
                {
                    running = false;
                }
                else if (e.type == SDL_KEYDOWN)
                {
                    switch (e.key.keysym.sym)
                    {
                    case SDLK_ESCAPE:
                        running = false;
                        break;
                    case SDLK_SPACE:
                        paused = !paused;
                        break;
                    case SDLK_r:
                        nes.reset();
                        break;
                    case SDLK_p:
                        selectedPalette = (selectedPalette + 1) % 8;
                        break;
                    case SDLK_f:
                        frameByFrame = true;
                        paused = true;
                        break;
                    }
                }
                // Add controller input handling here
            }

            if (!paused || frameByFrame)
            {
                // Run the NES until we get a new frame
                do
                {
                    nes.clock();
                } while (!nes.ppu.frameComplete());

                frameByFrame = false;
                frameCount++;

                // Render the NES screen
                renderNESScreen();

                // Render pattern tables
                renderPatternTables();

                // Present the rendered frame
                SDL_RenderPresent(renderer);
            }

            // Calculate and print FPS every second
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastTime).count();
            if (elapsed >= 1)
            {
                std::cout << "FPS: " << frameCount / elapsed << std::endl;
                frameCount = 0;
                lastTime = currentTime;
            }
        }
    }

private:
    void renderNESScreen()
    {
        uint32_t pixels[256 * 240];
        uint8_t *screenData = nes.ppu.GetScreen();

        // Convert NES screen data to RGBA
        for (int y = 0; y < 240; y++)
        {
            for (int x = 0; x < 256; x++)
            {
                uint8_t pixelData = screenData[y * 256 + x];
                uint8_t palette = (pixelData & 0xF0) >> 4;
                uint8_t pixel = pixelData & 0x0F;
                pixels[y * 256 + x] = NES_PALETTE[nes.ppu.GetColourFromPaletteRam(palette, pixel) & 0x3F];
            }
        }

        // Update texture and render
        SDL_UpdateTexture(texture, NULL, pixels, 256 * sizeof(uint32_t));
        SDL_Rect destRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, texture, NULL, &destRect);
    }

    void renderPatternTables()
    {

        if (!patternTexture) {
            std::cerr << "Pattern texture not initialized!" << std::endl;
            return;
        }
        
        uint32_t pixels[256 * 128];
        
        // Get pattern tables for our selected palette
        uint8_t *patternTable0 = nes.ppu.GetPatternTable(0, selectedPalette);
        uint8_t *patternTable1 = nes.ppu.GetPatternTable(1, selectedPalette);
        
        if (!patternTable0 || !patternTable1) {
            std::cerr << "Pattern table pointers are null!" << std::endl;
            return;
        }

        uint32_t pixels[256 * 128];

        // Get pattern tables for our selected palette
        uint8_t *patternTable0 = nes.ppu.GetPatternTable(0, selectedPalette);
        uint8_t *patternTable1 = nes.ppu.GetPatternTable(1, selectedPalette);

        // Convert pattern table data to RGBA
        for (int y = 0; y < 128; y++)
        {
            for (int x = 0; x < 128; x++)
            {
                // Left pattern table (0)
                uint8_t pixel0 = patternTable0[y * 128 + x];
                pixels[y * 256 + x] = NES_PALETTE[nes.ppu.GetColourFromPaletteRam(selectedPalette, pixel0) & 0x3F];

                // Right pattern table (1)
                uint8_t pixel1 = patternTable1[y * 128 + x];
                pixels[y * 256 + (x + 128)] = NES_PALETTE[nes.ppu.GetColourFromPaletteRam(selectedPalette, pixel1) & 0x3F];
            }
        }

        // Update texture and render
        SDL_UpdateTexture(patternTexture, NULL, pixels, 256 * sizeof(uint32_t));
        SDL_Rect destRect = {SCREEN_WIDTH, 0, PATTERN_WIDTH, PATTERN_HEIGHT};
        SDL_RenderCopy(renderer, patternTexture, NULL, &destRect);
    }

    // Emulation state
    bool running = false;
    bool paused = false;
    bool frameByFrame = false;
    uint8_t selectedPalette = 0;

    // SDL objects
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Texture *patternTexture;

    // NES hardware
    Bus nes;
    std::shared_ptr<Cartridge> cartridge;
};

int main(int argc, char *argv[])
{
    Emulator emulator;

    // Check if a ROM file was provided
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <rom_file.nes>" << std::endl;
        return 1;
    }

    // Load the ROM
    if (!emulator.loadROM(argv[1]))
    {
        return 1;
    }

    // Start the emulator
    emulator.run();

    return 0;
}