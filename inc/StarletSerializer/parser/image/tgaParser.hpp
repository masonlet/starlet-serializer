#pragma once

#include "StarletSerializer/parser/imageParser.hpp"

namespace Starlet::Serializer {

class TgaParser : public ImageParser {
public:
	bool parse(const std::string& path, ImageData& out) override;

private:
	bool parseTgaHeader(const unsigned char* p, size_t fileSize, uint32_t& width, uint32_t& height, uint8_t& bpp, bool& topDown, uint32_t& dataOffset);

	bool validateFileSignature(const unsigned char* p, size_t fileSize) const;

	bool allocatePixelBuffer(ImageData& out, uint32_t width, uint32_t height) const;
	bool copyPixelData(const unsigned char* p, size_t fileSize, uint32_t dataOffset, ImageData& out, uint32_t width, uint32_t height, uint8_t bpp, bool topDown) const;
};

}
