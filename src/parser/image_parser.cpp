#include "starlet-serializer/parser/image_parser.hpp"

#include "starlet-serializer/parser/image/bmp_parser.hpp"
#include "starlet-serializer/parser/image/tga_parser.hpp"

#include "starlet-logger/logger.hpp"

namespace Starlet::Serializer {

bool ImageParser::parse(const std::string& path, ImageData& out) {
	switch (detectFormat(path)) {
	case ImageFormat::BMP: {
		BmpParser parser;
		return parser.parse(path, out);
	}
	case ImageFormat::TGA: {
		TgaParser parser;
		return parser.parse(path, out);
	}
	default:
		Logger::error("ImageParser", "parse", "Unsupported image format: " + path);
		return false;
	}
}

ImageParser::ImageFormat ImageParser::detectFormat(const std::string& path) {
	size_t dotPos = path.find_last_of('.');
	if (dotPos == std::string::npos || dotPos == path.length() - 1) 
		return ImageFormat::UNKNOWN;

	std::string extension = path.substr(dotPos + 1);
	for (char& c : extension) c = static_cast<char>(tolower(c));
	
	if      (extension == "bmp") return ImageFormat::BMP;
	else if (extension == "tga") return ImageFormat::TGA;
	else                         return ImageFormat::UNKNOWN;
}

}
