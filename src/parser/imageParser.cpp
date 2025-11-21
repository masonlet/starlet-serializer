#include "starlet-serializer/parser/imageParser.hpp"

#include "starlet-serializer/parser/image/bmpParser.hpp"
#include "starlet-serializer/parser/image/tgaParser.hpp"

#include "starlet-logger/logger.hpp"

#include <memory>

namespace Starlet::Serializer {

bool ImageParser::parse(const std::string& path, ImageData& out) {
	std::unique_ptr<ImageParserBase> parser;
	switch (detectFormat(path)) {
			case ImageFormat::BMP:
				parser = std::make_unique<BmpParser>();
				break;
			case ImageFormat::TGA:
				parser = std::make_unique<TgaParser>();
				break;
			default:
				Logger::error("ImageParser", "parse", "Unsupported image format: " + path);
				return false;
	}

	return parser->parse(path, out);
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
