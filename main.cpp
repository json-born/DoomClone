#include <SDL3/SDL.h>
#include <iostream>
#include <iomanip>
#include <array>
#include <string>

#include "wad.h"
#include "palette.h"

void renderFrame(SDL_Renderer* renderer, SDL_Texture* texture) {
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

SDL_Texture* createTextureFromRGB(SDL_Renderer* renderer,
    const std::vector<uint8_t>& pixels,
    int width, int height)
{
    SDL_Texture* texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGB24,  // matches 3-byte RGB data
        SDL_TEXTUREACCESS_STATIC,
        width,
        height
    );

    if (!texture) {
        throw std::runtime_error(std::string("Failed to create texture: ") + SDL_GetError());
    }

    // Update the texture with pixel data
    SDL_UpdateTexture(texture, nullptr, pixels.data(), width * 3);

    return texture;
}

int main() {
    try {
        Wad wad = loadWad("doom.wad");

		// Read PLAYPAL lump
        const std::string lumpName = "PLAYPAL";

        auto it = wad.directoryMap.find(lumpName);
        if (it == wad.directoryMap.end()) {
            std::cerr << "Lump not found: " << lumpName << "\n";
            return 1;
        }

        const int imageWidth = 512;
        const int imageHeight = 512;

        SDL_Window* window = SDL_CreateWindow("PLAYPAL Viewer",
            imageWidth, imageHeight, 0);

        SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

		// Create texture from PLAYPAL lump
        const auto& entry = it->second;
        std::vector<char> lumpData = readLump(wad, entry.position, entry.size);
        std::vector<std::array<uint8_t, 3>> colors = parsePlaypalLump(lumpData);
        std::vector<uint8_t> pixelBuffer = generate256ColorPalettePixels(colors);
        SDL_Texture* texture = createTextureFromRGB(renderer, pixelBuffer, imageWidth, imageHeight);

        bool running = true;
        SDL_Event e;
        while (running) {
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_EVENT_QUIT) {
                    running = false;
                }
            }
            renderFrame(renderer, texture);
        }

        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}