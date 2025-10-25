#pragma once

#include "parser.hpp"

class BmpParser : public Parser {
public:
	bool parse(const std::string& path, TextureCPU& out);

private:
	bool parseBmpHeader(const unsigned char* p, size_t fileSize, int32_t& height, int32_t& width, uint32_t& dataOffset);
};