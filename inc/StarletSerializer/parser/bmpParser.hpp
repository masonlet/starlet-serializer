#pragma once

#include "parser.hpp"

namespace Starlet::Serializer {
	struct BmpData {
		int32_t width{ 0 }, height{ 0 };
		std::vector<uint8_t> pixels;
		uint8_t  pixelSize{ 0 };
		size_t   byteSize{ 0 };
	};

	class BmpParser : public Parser {
	public:
		bool parse(const std::string& path, BmpData& out);

	private:
		bool parseBmpHeader(const unsigned char* p, size_t fileSize, int32_t& height, int32_t& width, uint32_t& dataOffset);
	};
}
