#pragma once

#include "parser.hpp"

namespace Starlet::Serializer {

struct BmpData;

class BmpParser : public Parser {
public:
	bool parse(const std::string& path, BmpData& out);

private:
	bool parseBmpHeader(const unsigned char* p, size_t fileSize, int32_t& width, int32_t& height, uint32_t& dataOffset);

	bool validateFileSignature(const unsigned char* p, size_t fileSize) const;
	bool validateDimensions(int32_t width, int32_t height) const;

	uint32_t readUint32(const unsigned char* p, size_t offset) const;
	uint16_t readUint16(const unsigned char* p, size_t offset) const;

	bool allocatePixelBuffer(BmpData& out, int32_t width, int32_t height) const;
	bool copyPixelData(const unsigned char* p, size_t fileSize, uint32_t dataOffset, BmpData& out, int32_t width, int32_t height) const;
	void convertBgrToRgb(const unsigned char* src, unsigned char* dst, int32_t width) const;
};

}
