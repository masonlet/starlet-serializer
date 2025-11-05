#include "StarletSerializer/parser/bmpParser.hpp"
#include "StarletSerializer/data/bmpData.hpp"

#include "StarletLogger/logger.hpp"

namespace Starlet::Serializer {

bool BmpParser::parse(const std::string& path, BmpData& out) {
	out.pixels.clear(); 
	out.pixelSize = out.byteSize = 0;
	out.width = out.height = 0;

	std::vector<unsigned char> file;
	if (!loadBinaryFile(file, path)) 
		return Logger::error("BmpParser", "parse", std::string("Failed to load file: ") + path);

	if (file.empty())                
		return Logger::error("BmpParser", "parse", "File is empty");

	const unsigned char* p = file.data();
	int32_t  width{ 0 }, height{ 0 };
	uint32_t dataOffset{ 0 };

	if (!parseBmpHeader(p, file.size(), width, height, dataOffset)) 
		return false;

	if (!allocatePixelBuffer(out, width, height)) 
		return false;

	if (!copyPixelData(p, file.size(), dataOffset, out, width, height)) 
		return false;

	return true;
}

bool BmpParser::parseBmpHeader(const unsigned char* p, size_t fileSize, int32_t& width, int32_t& height, uint32_t& dataOffset) {
	if (!validateFileSignature(p, fileSize)) return false;

	dataOffset = readUint32(p, 10);
	if (dataOffset >= fileSize) return Logger::error("BmpParser", "parseBmpHeader", "Bad data offset");

	uint32_t dibSize = readUint32(p, 14);
	if (dibSize < 40) return Logger::error("BmpParser", "parseBmpHeader", "Unsupported DIB header size: " + std::to_string(dibSize));

	width = static_cast<int32_t>(readUint32(p, 18));
	height = static_cast<int32_t>(readUint32(p, 22));
	if (!validateDimensions(width, height)) return false;

	uint16_t planes = readUint16(p, 26);
	if (planes != 1) return Logger::error("BmpParser", "parseBmpHeader", "Planes != 1: " + std::to_string(planes));

	uint16_t bpp = readUint16(p, 28);
	if (bpp != 24) return Logger::error("BmpParser", "parseBmpHeader", "Only 24bpp supported: " + std::to_string(bpp));

	uint32_t compression = readUint32(p, 30);
	if (compression != 0) return Logger::error("BmpParser", "parseBmpHeader", "Compressed BMP not supported: " + std::to_string(compression));

	return true;
}

bool BmpParser::validateFileSignature(const unsigned char* p, size_t fileSize) const {
	if (!p)	return Logger::error("BmpParser", "validateFileSignature", "Null buffer");

	if (fileSize < 54) 
		return Logger::error("BmpParser", "validateFileSignature", "File too small: " + std::to_string(fileSize) + " bytes");

	if (p[0] != 'B' || p[1] != 'M') 
		return Logger::error("BmpParser", "validateFileSignature", "Bad signature (not BM)");

	return true;
}

bool BmpParser::validateDimensions(int32_t width, int32_t height) const {
	if (width <= 0)  return Logger::error("BmpParser", "validateDimensions", "Invalid width: "  + std::to_string(width));
	if (height == 0) return Logger::error("BmpParser", "validateDimensions", "Invalid height: " + std::to_string(height));
	return true;
}

uint32_t BmpParser::readUint32(const unsigned char* p, size_t offset) const {
	return static_cast<uint32_t>(p[offset]) 
		  | (static_cast<uint32_t>(p[offset + 1]) << 8) 
		  | (static_cast<uint32_t>(p[offset + 2]) << 16) 
		  | (static_cast<uint32_t>(p[offset + 3]) << 24);
}
uint16_t BmpParser::readUint16(const unsigned char* p, size_t offset) const {
	return static_cast<uint16_t>(p[offset]) 
		  | (static_cast<uint16_t>(p[offset + 1]) << 8);
}

bool BmpParser::allocatePixelBuffer(BmpData& out, int32_t width, int32_t height) const {
	const int32_t absHeight = (height > 0) ? height : -height;

	out.width = width;
	out.height = absHeight;
	out.pixelSize = 3;
	out.byteSize = static_cast<size_t>(width) * static_cast<size_t>(absHeight) * out.pixelSize;
	out.pixels.resize(out.byteSize);

	if (out.pixels.empty()) return Logger::error("BmpParser", "allocatePixelBuffer", "Failed to allocate memory for pixel data");
	return true;
}

bool BmpParser::copyPixelData(const unsigned char* p, size_t fileSize, uint32_t dataOffset, BmpData& out, int32_t width, int32_t height) const {
	const bool bottomUp = (height > 0);
	const int32_t absHeight = bottomUp ? height : -height;

	const size_t rowStridePadded = (static_cast<size_t>(width) * 3 + 3) & ~static_cast<size_t>(3);
	const size_t needed = static_cast<size_t>(dataOffset) + rowStridePadded * static_cast<size_t>(absHeight);

	if (needed > fileSize)
		return Logger::error("BmpParser", "copyPixelData", "File too small for declared dimensions: " + std::to_string(fileSize) + " bytes, need " + std::to_string(needed) + " bytes");

	const unsigned char* srcPixels = p + dataOffset;

	for (int row = 0; row < absHeight; ++row) {
		const int srcRow = bottomUp ? (absHeight - 1 - row) : row;
		const unsigned char* src = srcPixels + rowStridePadded * static_cast<size_t>(srcRow);
		unsigned char* dst = out.pixels.data() + static_cast<size_t>(row) * static_cast<size_t>(width) * 3;

		convertBgrToRgb(src, dst, width);
	}

	return true;
}

void BmpParser::convertBgrToRgb(const unsigned char* src, unsigned char* dst, int32_t width) const {
	for (int col = 0; col < width; ++col) {
		const size_t offset = static_cast<size_t>(col) * 3;
		dst[offset + 0] = src[offset + 2];
		dst[offset + 1] = src[offset + 1];
		dst[offset + 2] = src[offset + 0];
	}
}

}
