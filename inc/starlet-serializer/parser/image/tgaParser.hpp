#pragma once

#include "starlet-serializer/parser/image/imageParserBase.hpp"

namespace Starlet::Serializer {

class TgaParser : public ImageParserBase {
public:
	bool parse(const std::string& path, ImageData& out) override;

private:
	bool parseHeader(const unsigned char* p, size_t fileSize, uint32_t& width, uint32_t& height, uint8_t& bpp, bool& topDown, uint32_t& dataOffset);
	bool validateFileSignature(const unsigned char* p, size_t fileSize) const;
	bool copyPixelData(const unsigned char* p, size_t fileSize, uint32_t dataOffset, ImageData& out, uint32_t width, uint32_t height, uint8_t bpp, bool topDown) const;
};

}
