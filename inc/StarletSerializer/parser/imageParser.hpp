#pragma once

#include "StarletSerializer/parser/parser.hpp"

#include <optional>

namespace Starlet::Serializer {

struct ImageData;

class ImageParser : public Parser {
public:
	virtual bool parse(const std::string& path, ImageData& out) = 0;

protected:
	void clearImageData(ImageData& imageData) const;
	std::optional<std::vector<unsigned char>> loadImageData(const std::string& path);

	bool validateDimensions(uint32_t width, uint32_t height) const;

	void convertBgrToRgb(const unsigned char* src, unsigned char* dst, uint32_t width) const;

	uint32_t readUint32(const unsigned char* p, size_t offset) const;
	uint16_t readUint16(const unsigned char* p, size_t offset) const;
};

}