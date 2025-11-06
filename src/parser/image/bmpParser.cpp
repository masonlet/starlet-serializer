#include "StarletSerializer/parser/image/bmpParser.hpp"
#include "StarletSerializer/data/imageData.hpp"

#include "StarletLogger/logger.hpp"

namespace Starlet::Serializer {

namespace {
	constexpr size_t BMP_HEADER_SIZE = 54;
	constexpr uint16_t BMP_SIGNATURE = 0x4D42; // "BM"
	constexpr uint16_t BMP_PLANES_EXPECTED = 1;
	constexpr uint16_t BMP_BPP_24 = 24;
	constexpr uint32_t BMP_COMPRESSION_NONE = 0;
	constexpr uint32_t BMP_DIB_HEADER_SIZE_MIN = 40;
}

bool BmpParser::parse(const std::string& path, ImageData& out) {
	clearImageData(out);

	std::optional<std::vector<unsigned char>> file = loadImageData(path);
	if (!file.has_value()) return false;

	const unsigned char* p = file->data();
	uint32_t width{ 0 };
	int32_t  height{ 0 };
	uint32_t dataOffset{ 0 };

	if (!parseBmpHeader(p, file->size(), width, height, dataOffset))
		return false;

	const bool bottomUp = (height > 0);
	const uint32_t absHeight = static_cast<uint32_t>(bottomUp ? height : -height);

	if (!allocatePixelBuffer(out, width, absHeight))
		return false;

	if (!copyPixelData(p, file->size(), dataOffset, out, width, absHeight, bottomUp))
		return false;

	return true;
}

bool BmpParser::parseBmpHeader(const unsigned char* p, size_t fileSize, uint32_t& width, int32_t& height, uint32_t& dataOffset) {
	if (!validateFileSignature(p, fileSize)) return false;

	dataOffset = readUint32(p, 10);
	if (dataOffset >= fileSize) return Logger::error("BmpParser", "parseBmpHeader", "Invalid data offset: " + std::to_string(dataOffset));

	uint32_t dibSize = readUint32(p, 14);
	if (dibSize < BMP_DIB_HEADER_SIZE_MIN) return Logger::error("BmpParser", "parseBmpHeader", "Unsupported DIB header size: " + std::to_string(dibSize));

	width = readUint32(p, 18);
	height = static_cast<int32_t>(readUint32(p, 22));
	const uint32_t absHeight = static_cast<uint32_t>(height > 0 ? height : -height);
	if (!validateDimensions(width, absHeight)) return false;

	uint16_t planes = readUint16(p, 26);
	if (planes != BMP_PLANES_EXPECTED) return Logger::error("BmpParser", "parseBmpHeader", "Planes != 1: " + std::to_string(planes));

	uint16_t bpp = readUint16(p, 28);
	if (bpp != BMP_BPP_24) return Logger::error("BmpParser", "parseBmpHeader", "Only 24bpp supported: " + std::to_string(bpp));

	uint32_t compression = readUint32(p, 30);
	if (compression != BMP_COMPRESSION_NONE) return Logger::error("BmpParser", "parseBmpHeader", "Compressed BMP not supported: " + std::to_string(compression));

	return true;
}

bool BmpParser::validateFileSignature(const unsigned char* p, size_t fileSize) const {
	if (!p)	return Logger::error("BmpParser", "validateFileSignature", "Null buffer");

	if (fileSize < BMP_HEADER_SIZE) 
		return Logger::error("BmpParser", "validateFileSignature", "File too small: " + std::to_string(fileSize) + " bytes");

	if (p[0] != 'B' || p[1] != 'M') 
		return Logger::error("BmpParser", "validateFileSignature", "Bad signature (not BM)");

	return true;
}

bool BmpParser::allocatePixelBuffer(ImageData& out, uint32_t width, uint32_t height) const {
	out.width = width;
	out.height = height;
	out.pixelSize = 3;
	out.byteSize = static_cast<size_t>(width) * static_cast<size_t>(height) * out.pixelSize;
	out.pixels.resize(out.byteSize);

	if (out.pixels.empty()) return Logger::error("BmpParser", "allocatePixelBuffer", "Failed to allocate memory for pixel data");
	return true;
}

bool BmpParser::copyPixelData(const unsigned char* p, size_t fileSize, uint32_t dataOffset, ImageData& out, uint32_t width, uint32_t height, bool bottomUp) const {
	const size_t rowStridePadded = (static_cast<size_t>(width) * 3 + 3) & ~static_cast<size_t>(3);
	const size_t needed = static_cast<size_t>(dataOffset) + rowStridePadded * static_cast<size_t>(height);

	if (needed > fileSize)
		return Logger::error("BmpParser", "copyPixelData", "File too small for declared dimensions: " + std::to_string(fileSize) + " bytes, need " + std::to_string(needed) + " bytes");

	const unsigned char* srcPixels = p + dataOffset;

	for (uint32_t row = 0; row < height; ++row) {
		const uint32_t srcRow = bottomUp ? (height - 1 - row) : row;
		const unsigned char* src = srcPixels + rowStridePadded * static_cast<size_t>(srcRow);
		unsigned char* dst = out.pixels.data() + static_cast<size_t>(row) * static_cast<size_t>(width) * 3;

		convertBgrToRgb(src, dst, width);
	}

	return true;
}

}
