#include "palette.h"

#include <vector>
#include <array>
#include <iostream>

std::vector<uint8_t> generate256ColorPalettePixels(
    const std::vector<std::array<uint8_t, 3>>& colors
) {
    if (colors.size() != 256) {
        throw std::runtime_error("Palette must contain exactly 256 colors");
    }

    constexpr std::size_t gridWidth = 16;
    constexpr std::size_t gridHeight = 16;
    constexpr std::size_t cellSize = 32;

    const std::size_t imageWidth = gridWidth * cellSize;
    const std::size_t imageHeight = gridHeight * cellSize;

    std::vector<uint8_t> pixelBuffer(imageWidth * imageHeight * 3);

    for (std::size_t i = 0; i < 256; ++i) {
        const auto& color = colors[i];

        const std::size_t row = i / gridWidth;
        const std::size_t col = i % gridWidth;

        const std::size_t startX = col * cellSize;
        const std::size_t startY = row * cellSize;

        for (std::size_t y = 0; y < cellSize; ++y) {
            for (std::size_t x = 0; x < cellSize; ++x) {
                const std::size_t pixelIndex =
                    ((startY + y) * imageWidth + (startX + x)) * 3;

                pixelBuffer[pixelIndex + 0] = color[0];
                pixelBuffer[pixelIndex + 1] = color[1];
                pixelBuffer[pixelIndex + 2] = color[2];
            }
        }
    }

    return pixelBuffer;
}


std::vector<std::array<uint8_t, 3>> parsePlaypalLump(
    const std::vector<char>& lumpData
) {
	// Each palette is 256 colors, each color is 3 bytes (RGB)
    if ((lumpData.size() % 768) != 0) {
        throw std::runtime_error("PLAYPAL lump data is of incorrect size");
	}

    // For now, we only want the first palette
    std::vector<std::array<uint8_t, 3>> colors;
    colors.reserve(256);


    for (size_t i = 0; i < 256; ++i) {
        size_t offset = i * 3;
        std::array<uint8_t, 3> color = {
            static_cast<uint8_t>(lumpData[offset]),
            static_cast<uint8_t>(lumpData[offset + 1]),
            static_cast<uint8_t>(lumpData[offset + 2])
        };
        colors.push_back(color);
    }

	return colors;
}