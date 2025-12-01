#include "starlet-serializer/parser/mesh_parser.hpp"

#include "starlet-serializer/parser/mesh/ply_parser.hpp"
#include "starlet-serializer/parser/mesh/obj_parser.hpp"

#include "starlet-logger/logger.hpp"

namespace Starlet::Serializer {

bool MeshParser::parse(const std::string& path, MeshData& out) {
	switch (detectFormat(path)) {
	case MeshFormat::PLY: {
		PlyParser parser;
		return parser.parse(path, out);
	}
	case MeshFormat::OBJ: {
		ObjParser parser;
		return parser.parse(path, out);
	}
	default:
		Logger::error("MeshParser", "parse", "Unsupported mesh format: " + path);
		return false;
	}
}

MeshParser::MeshFormat MeshParser::detectFormat(const std::string& path) {
	size_t dotPos = path.find_last_of('.');
	if (dotPos == std::string::npos || dotPos == path.length() - 1) 
		return MeshFormat::UNKNOWN;

	std::string extension = path.substr(dotPos + 1);
	for (char& c : extension) c = static_cast<char>(tolower(c));

	if (extension == "ply") return MeshFormat::PLY;
	if (extension == "obj") return MeshFormat::OBJ;
	else                    return MeshFormat::UNKNOWN;
}

}
