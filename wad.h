#pragma once

#include <array>
#include <string>
#include <unordered_map>
#include <vector>

struct WadHeader {
	std::string id;
	uint32_t lumpCount = 0;
	uint32_t directoryOffset = 0;
};

struct LumpDirectoryEntry {
	std::string name;
	uint32_t position = 0;
	uint32_t size = 0;
};

using LumpDirectoryMap = std::unordered_map<std::string, LumpDirectoryEntry>;

struct Wad {
	WadHeader header;
	LumpDirectoryMap directoryMap;
	std::vector<char> data;
};

Wad loadWad(const std::string path);
std::vector<char> readLump(const Wad& wad, uint32_t position, uint32_t size);