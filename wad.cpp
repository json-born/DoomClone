#include "wad.h"

#include <fstream>
#include <string>
#include <array>
#include <vector>
#include <span>
#include <iostream>
#include <unordered_map>

uint32_t readLE32(std::span<const char> buffer, size_t offset) {
	return static_cast<uint32_t>(
		static_cast<uint8_t>(buffer[offset]) |
		(static_cast<uint8_t>(buffer[offset + 1]) << 8) |
		(static_cast<uint8_t>(buffer[offset + 2]) << 16) |
		(static_cast<uint8_t>(buffer[offset + 3]) << 24)
	);
}

WadHeader readHeader(std::ifstream& fileStream) {
	std::array<char, 12> buffer;

	fileStream.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));

	WadHeader header;
	header.id = std::string(buffer.data(), 4);
	header.lumpCount = readLE32(buffer, 4);
	header.directoryOffset = readLE32(buffer, 8);

	return header;
}

std::vector<char> readData(std::ifstream& fileStream) {
	fileStream.seekg(0, std::ios::end);

	// this is perfectly safe as WAD files are way smaller than 4GB
	uint32_t size = static_cast<uint32_t>(fileStream.tellg());
	std::vector<char> buffer(size);

	fileStream.seekg(0, std::ios::beg);
	fileStream.read(buffer.data(), size);
	fileStream.seekg(0, std::ios::beg);
	
	return buffer;
}

LumpDirectoryMap readDirectory(std::ifstream& fileStream, uint32_t offset, uint32_t size) {
	std::vector<char> buffer(size);
	LumpDirectoryMap lumpMap;

	lumpMap.reserve(size / 16);

	fileStream.seekg(offset, std::ios::beg);
	fileStream.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));

	for (uint32_t i = 0; i + 16 <= size; i += 16) {
		uint32_t lumpPosition = readLE32(buffer, i);
		uint32_t lumpSize     = readLE32(buffer, i + 4);

		std::string lumpName(buffer.data() + i + 8, 8);

		// Strip trailing nulls (keep everything before the first null)
		size_t endPos = 7;
		while (endPos > 0 && lumpName[endPos] == '\0') {
			--endPos;
		}
		lumpName.resize(endPos + 1);

		lumpMap[lumpName] = { lumpName, lumpPosition, lumpSize };
	}
	
	return lumpMap;
}

std::vector<char> readLump(const Wad& wad, uint32_t position, uint32_t size) {
	if (position + size > wad.data.size()) {
		throw std::out_of_range("Lump position/size exceeds WAD data size");
	}

	return std::vector<char>(
		wad.data.begin() + position,
		wad.data.begin() + position + size
	);
}

Wad loadWad(std::string path) {
	std::ifstream fileStream(path, std::ios::binary);
	Wad wad;
	
	if (!fileStream.is_open()) {
		throw std::runtime_error("Failed to open WAD file: " + path);
	}

	wad.data = readData(fileStream);
	wad.header = readHeader(fileStream);
	
	uint32_t directorySize = wad.header.lumpCount * 16;
	wad.directoryMap = readDirectory(fileStream, wad.header.directoryOffset, directorySize);
	
	return wad;
}