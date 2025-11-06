#include "StarletSerializer/parser/imageParser.hpp"
#include "StarletSerializer/data/imageData.hpp"

#include "StarletLogger/logger.hpp"

namespace Starlet::Serializer {

void ImageParser::clearImageData(ImageData& data) const {
	data.pixels.clear();
	data.pixelSize = data.byteSize = 0;
	data.width = data.height = 0;
}

std::optional<std::vector<unsigned char>> ImageParser::loadImageData(const std::string& path) {
	std::vector<unsigned char> file;
	if (!loadBinaryFile(file, path)) {
		Logger::error("ImageParser", "loadImageData", std::string("Failed to load file: ") + path);
		return std::nullopt;
	}
		
	if (file.empty()) {
		Logger::error("ImageParser", "loadImageData", "File is empty");
		return std::nullopt;
	}

	return file;
}

bool ImageParser::validateDimensions(uint32_t width, uint32_t height) const {
	if (width == 0)  return Logger::error("ImageParser", "validateDimensions", "Invalid width: " + std::to_string(width));
	if (height == 0) return Logger::error("ImageParser", "validateDimensions", "Invalid height: " + std::to_string(height));
	constexpr size_t MAX_DIMENSION = 65536;
	if (width > MAX_DIMENSION)  return Logger::error("ImageParser", "validateDimensions", "Width exceeds maximum: " + std::to_string(width));
	if (height > MAX_DIMENSION) return Logger::error("ImageParser", "validateDimensions", "Height exceeds maximum: " + std::to_string(height));
	return true;
}

uint32_t ImageParser::readUint32(const unsigned char* p, size_t offset) const {
	return static_cast<uint32_t>(p[offset])
		  | (static_cast<uint32_t>(p[offset + 1]) << 8)
		  | (static_cast<uint32_t>(p[offset + 2]) << 16)
		  | (static_cast<uint32_t>(p[offset + 3]) << 24);
}
uint16_t ImageParser::readUint16(const unsigned char* p, size_t offset) const {
	return static_cast<uint16_t>(p[offset])
		  | (static_cast<uint16_t>(p[offset + 1]) << 8);
}

void ImageParser::convertBgrToRgb(const unsigned char* src, unsigned char* dst, uint32_t width) const {
	for (uint32_t col = 0; col < width; ++col) {
		const size_t offset = static_cast<size_t>(col) * 3;
		dst[offset + 0] = src[offset + 2];
		dst[offset + 1] = src[offset + 1];
		dst[offset + 2] = src[offset + 0];
	}
}

}