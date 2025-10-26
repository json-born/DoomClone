#pragma once
#include <vector>
#include <array>

std::vector<uint8_t> generate256ColorPalettePixels(const std::vector<std::array<uint8_t, 3>>& colors);

std::vector<std::array<uint8_t, 3>> parsePlaypalLump(const std::vector<char>& lumpData);
