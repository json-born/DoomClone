#include <SDL3/SDL.h>
#include <iostream>
#include <iomanip>
#include <array>
#include <string>

#include "wad.h"

int main() {
    try {
        Wad wad = loadWad("doom.wad");  // replace with your WAD file path

        // Print header
        std::cout << "WAD ID: " << wad.header.id << "\n";
        std::cout << "Lump count: " << wad.header.lumpCount << "\n";
        std::cout << "Directory offset: " << wad.header.directoryOffset << "\n\n";

        // Print all lumps
        std::cout << "Directory entries:\n";
        for (const auto& pair : wad.directoryMap) {
            const LumpDirectoryEntry& entry = pair.second;

            std::cout << "Name: " << entry.name
                << ", Position: " << entry.position
                << ", Size: " << entry.size
                << "\n";
        }

        // Example: read a known lump, e.g., PLAYPAL
        const std::string lumpName = "PLAYPAL";

        auto it = wad.directoryMap.find(lumpName);
        if (it == wad.directoryMap.end()) {
            std::cerr << "Lump not found: " << lumpName << "\n";
            return 1;
        }

        const auto& entry = it->second;
        std::vector<char> lumpData = readLump(wad, entry.position, entry.size);

        std::cout << "Lump: " << lumpName
            << ", Size: " << lumpData.size() << "\n";

        // Dump first 32 bytes in hex for inspection
        std::cout << "First 32 bytes:\n";
        for (size_t i = 0; i < lumpData.size() && i < 32; ++i) {
            std::cout << std::hex << std::setw(2) << std::setfill('0')
                << (static_cast<unsigned int>(static_cast<unsigned char>(lumpData[i]))) << " ";
            if ((i + 1) % 16 == 0) std::cout << "\n";
        }
        std::cout << std::dec << "\n"; // reset to decimal

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}