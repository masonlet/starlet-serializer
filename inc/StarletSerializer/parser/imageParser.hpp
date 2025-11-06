#pragma once

#include "parser.hpp"

namespace Starlet::Serializer {

struct ImageData;

class ImageParser : public Parser {
public:
	bool parse(const std::string& path, ImageData& out);

private:
	enum class ImageFormat {
		BMP,
		TGA,
		UNKNOWN
	};

	ImageFormat detectFormat(const std::string& path);
};

}