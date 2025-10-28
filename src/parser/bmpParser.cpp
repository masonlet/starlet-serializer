#include "StarletSerializer/parser/bmpParser.hpp"
#include "StarletSerializer/utils/log.hpp"

#include "StarletSerializer/data/bmpData.hpp"

#include <cstdio>

namespace Starlet::Serializer {
	bool BmpParser::parse(const std::string& path, BmpData& out) {
		out.pixels.clear(); 
		out.pixelSize = out.byteSize = 0;
		out.width = out.height = 0;

		std::vector<unsigned char> file;
		if (!loadBinaryFile(file, path))
			return error("BMPParser", "parseBMP", std::string("Failed to load file: ") + path);

		if (file.empty()) return error("BmpParser", "parseBMP", "Input pointer is null\n");

		const unsigned char* p = file.data();
		int32_t  height{ 0 }, width{ 0 };
		uint32_t dataOffset{ 0 };

		if (!parseBmpHeader(p, file.size(), height, width, dataOffset)) return false;

		const bool bottomUp = (height > 0);
		height = bottomUp ? height : -height;

		size_t rowStridePadded = (static_cast<size_t>(width) * 3 + 3) & ~static_cast<size_t>(3);
		size_t needed = static_cast<size_t>(dataOffset) + rowStridePadded * static_cast<size_t>(height);
		if (needed > file.size())
			return error("BMPParser", "parseBMP", "File too small for declared dimensions: " + std::to_string(file.size()) + " bytes, need " + std::to_string(needed) + " bytes");

		out.width = width;
		out.height = height;
		out.pixelSize = 3;
		out.byteSize = static_cast<size_t>(width * height * out.pixelSize);
		out.pixels.resize(out.byteSize);
		if (out.pixels.empty())
			return error("BMPParser", "parseBMP", "Failed to allocate memory for pixel data");

		const unsigned char* srcPixels = p + dataOffset;
		for (int row = 0; row < height; ++row) {
			int srcRow = bottomUp ? (height - 1 - row) : row;
			const unsigned char* src = srcPixels + rowStridePadded * static_cast<size_t>(srcRow);
			unsigned char* dst = out.pixels.data() + static_cast<size_t>(row) * static_cast<size_t>(width) * 3;

			for (int col = 0; col < width; ++col) {
				const size_t s = static_cast<size_t>(col) * 3;
				if (s + 2 < out.byteSize) {
					dst[s + 0] = src[s + 2];
					dst[s + 1] = src[s + 1];
					dst[s + 2] = src[s + 0];
				}
				else return error("BMPParser", "parseBMP", "Buffer overrun detected while writing to 'dst'");
			}
		}

		return true;
	}

	bool BmpParser::parseBmpHeader(const unsigned char* p, size_t fileSize, int32_t& height, int32_t& width, uint32_t& dataOffset) {
		if (!p) return error("BMPParser", "readHeader", "Null buffer");
		if (fileSize < 54) return error("BMPParser", "parseBMP", "File too small: " + std::to_string(fileSize) + " bytes");
		if (p[0] != 'B' || p[1] != 'M') return error("BMPParser", "readHeader", "Bad signature (not BM)");

		dataOffset = (uint32_t)p[10] | ((uint32_t)p[11] << 8) | ((uint32_t)p[12] << 16) | ((uint32_t)p[13] << 24);
		if (dataOffset >= fileSize) return error("BMPParser", "readHeader", "Bad data offset");

		uint32_t dibSize = static_cast<uint32_t>(p[14]) |
			(static_cast<uint32_t>(p[15]) << 8) |
			(static_cast<uint32_t>(p[16]) << 16) |
			(static_cast<uint32_t>(p[17]) << 24);
		if (dibSize < 40) return error("BMPParser", "readHeader", "Unsupported DIB header size: " + std::to_string(dibSize));

		width = static_cast<int32_t>(
			static_cast<uint32_t>(p[18]) |
			(static_cast<uint32_t>(p[19]) << 8) |
			(static_cast<uint32_t>(p[20]) << 16) |
			(static_cast<uint32_t>(p[21]) << 24)
			);
		if (width <= 0) return error("BMPParser", "readHeader", "Invalid width: " + std::to_string(width));

		height = static_cast<int32_t>(
			static_cast<uint32_t>(p[22]) |
			(static_cast<uint32_t>(p[23]) << 8) |
			(static_cast<uint32_t>(p[24]) << 16) |
			(static_cast<uint32_t>(p[25]) << 24)
			);
		if (height == 0) return error("BMPParser", "readHeader", "Invalid height: " + std::to_string(height));

		uint16_t planes = static_cast<uint16_t>(static_cast<uint32_t>(p[26]) | (static_cast<uint32_t>(p[27]) << 8));
		if (planes != 1) return error("BMPParser", "readHeader", "Planes != 1: " + std::to_string(planes));

		uint16_t bpp = static_cast<uint16_t>(static_cast<uint32_t>(p[28]) | (static_cast<uint32_t>(p[29]) << 8));
		if (bpp != 24) return error("BMPParser", "readHeader", "Only 24bpp supported: " + std::to_string(bpp));

		uint32_t compression = static_cast<uint32_t>(p[30]) |
			(static_cast<uint32_t>(p[31]) << 8) |
			(static_cast<uint32_t>(p[32]) << 16) |
			(static_cast<uint32_t>(p[33]) << 24);
		if (compression != 0) return error("BMPParser", "readHeader", "Compressed BMP not supported: " + std::to_string(compression));
		return true;
	}
}
