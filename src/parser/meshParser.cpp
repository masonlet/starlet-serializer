#include "StarletSerializer/parser/meshParser.hpp"

#include "StarletSerializer/parser/mesh/plyParser.hpp"

#include "StarletLogger/logger.hpp"

#include <memory>

namespace Starlet::Serializer {

	bool MeshParser::parse(const std::string& path, MeshData& out) {
		MeshFormat format = detectFormat(path);

		std::unique_ptr<MeshParserBase> parser;
		switch (format) {
		case MeshFormat::PLY:
			parser = std::make_unique<PlyParser>();
			break;
			Logger::error("ImageParser", "parse", "Unsupported image format: " + path);
			return false;
		}

		return parser->parse(path, out);
	}

	MeshParser::MeshFormat MeshParser::detectFormat(const std::string& path) {
		size_t dotPos = path.find_last_of('.');
		if (dotPos == std::string::npos || dotPos == path.length() - 1) {
			return MeshFormat::UNKNOWN;
		}

		std::string extension = path.substr(dotPos + 1);
		for (char& c : extension) c = static_cast<char>(tolower(c));

		if (extension == "ply")      return MeshFormat::PLY;
		else                         return MeshFormat::UNKNOWN;
	}

}
