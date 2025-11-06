#pragma once

#include "StarletSerializer/parser/imageParser.hpp"

namespace Starlet::Serializer {

class BmpParser : public ImageParser {
public:
	bool parse(const std::string& path, ImageData& out) override;

private:
	bool parseBmpHeader(const unsigned char* p, size_t fileSize, uint32_t& width, int32_t& height, uint32_t& dataOffset);

	bool validateFileSignature(const unsigned char* p, size_t fileSize) const;

	bool allocatePixelBuffer(ImageData& out, uint32_t width, uint32_t height) const;
	bool copyPixelData(const unsigned char* p, size_t fileSize, uint32_t dataOffset, ImageData& out, uint32_t width, uint32_t height, bool bottomUp) const;
};

}
