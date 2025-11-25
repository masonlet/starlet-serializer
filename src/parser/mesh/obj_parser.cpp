#include "starlet-serializer/parser/mesh/obj_parser.hpp"
#include "starlet-serializer/data/mesh_data.hpp"

#include "starlet-logger/logger.hpp"

#include <cstring>  
#include <cfloat>

namespace Starlet::Serializer {

namespace {
	inline bool strncasecmp(const char* a, const char* b, size_t n) {
		for (size_t i = 0; i < n; ++i)
			if (tolower(a[i]) != tolower(b[i])) return false;
		return true;
	}
}

bool ObjParser::parse(const std::string& path, MeshData& out) {
	return Starlet::Logger::error("ObjParser", "parse", "OBJ not yet implemented!"), false;
}

}