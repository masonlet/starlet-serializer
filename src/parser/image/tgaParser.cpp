#include "starlet-serializer/parser/image/tgaParser.hpp"
#include "starlet-serializer/data/imageData.hpp"

#include "starlet-logger/logger.hpp"

namespace Starlet::Serializer {

namespace {
	constexpr uint8_t TGA_IMAGE_TYPE_UNCOMPRESSED_TRUE_COLOR = 2;
	constexpr uint8_t TGA_COLOUR_MAP_TYPE_NONE = 0;
	constexpr uint8_t TGA_ORIGIN_TOP_BIT = 0x20; // Bit 5 of image descriptor byte
	constexpr size_t  TGA_HEADER_SIZE = 18;
}

bool TgaParser::parse(const std::string& path, ImageData& out) {
	clearImageData(out);

	std::optional<std::vector<unsigned char>> file = loadImageData(path);
	if (!file.has_value()) return false;

	const unsigned char* p = file->data();
	uint32_t width{ 0 }, height{ 0 };
	uint8_t bpp{ 0 };
	bool topDown{ false };
	uint32_t dataOffset{ 0 };

	if (!parseHeader(p, file->size(), width, height, bpp, topDown, dataOffset))
		return false;

	if (!allocatePixelBuffer(out, width, height))
		return false;

	if (!copyPixelData(p, file->size(), dataOffset, out, width, height, bpp, topDown))
		return false;

	return true;
}

bool TgaParser::parseHeader(const unsigned char* p, size_t fileSize, uint32_t& width, uint32_t& height, uint8_t& bpp, bool& topDown, uint32_t& dataOffset) {
	if (!validateFileSignature(p, fileSize)) return false;

	const uint8_t idLength      = p[0];
	const uint8_t colourMapType = p[1];
	const uint8_t imageType     = p[2];

	if (imageType != TGA_IMAGE_TYPE_UNCOMPRESSED_TRUE_COLOR)
		return Logger::error("TgaParser", "parseHeader", "Unsupported TGA image type (only uncompressed true-colour TGA supported (2)), got: " + std::to_string(imageType));

	if (colourMapType != TGA_COLOUR_MAP_TYPE_NONE)
		return Logger::error("TgaParser", "parseHeader", "Unsupported TGA colour map type (only no colour map supported (0)), got: " + std::to_string(colourMapType));

	width = static_cast<uint32_t>(readUint16(p, 12));
	height = static_cast<uint32_t>(readUint16(p, 14));
	if (!validateDimensions(width, height))
		return false;

	bpp = p[16];
	if (bpp != 24 && bpp != 32)
		return Logger::error("TgaParser", "parseHeader", "Unsupported TGA bits per pixel (only 24 and 32 supported), got: " + std::to_string(bpp));

	const uint8_t imageDescriptor = p[17];
	topDown = (imageDescriptor & TGA_ORIGIN_TOP_BIT) != 0;

	dataOffset = TGA_HEADER_SIZE + idLength;
	if (dataOffset >= fileSize)
		return Logger::error("TgaParser", "parseHeader", "Invalid data offset");

	return true;
}

bool TgaParser::validateFileSignature(const unsigned char* p, size_t fileSize) const {
	if (!p)	return Logger::error("TgaParser", "validateFileSignature", "Null buffer");

	if (fileSize < TGA_HEADER_SIZE)
		return Logger::error("TgaParser", "validateFileSignature", "File too small: " + std::to_string(fileSize) + " bytes");

	return true;
}

bool TgaParser::copyPixelData(const unsigned char* p, size_t fileSize, uint32_t dataOffset, ImageData& out, uint32_t width, uint32_t height, uint8_t bpp, bool topDown) const {
	const size_t rowStride = static_cast<size_t>(width) * ((bpp == 24) ? 3 : 4);
	const size_t needed = static_cast<size_t>(dataOffset) + rowStride * static_cast<size_t>(height);

	if (needed > fileSize)
		return Logger::error("TgaParser", "copyPixelData", "File too small for declared dimensions: " + std::to_string(fileSize) + " bytes, need " + std::to_string(needed) + " bytes");

	const unsigned char* srcPixels = p + dataOffset;

	for (uint32_t row = 0; row < height; ++row) {
		const uint32_t srcRow = topDown ? row : (height - 1 - row);
		const unsigned char* srcRowPtr = srcPixels + srcRow * rowStride;
		unsigned char* dstRowPtr = out.pixels.data() + static_cast<size_t>(row) * static_cast<size_t>(width) * out.pixelSize;

		if (bpp == 24) convertBgrToRgb(srcRowPtr, dstRowPtr, width);
		else {
			for (uint32_t col = 0; col < width; ++col) {
				const unsigned char* srcPixelPtr = srcRowPtr + col * 4;
				unsigned char* dstPixelPtr = dstRowPtr + static_cast<size_t>(col) * out.pixelSize;
				dstPixelPtr[0] = srcPixelPtr[2];
				dstPixelPtr[1] = srcPixelPtr[1];
				dstPixelPtr[2] = srcPixelPtr[0];
			}
		}
	}

	return true;
}

}