#include "starlet-serializer/parser/mesh/obj_parser.hpp"
#include "starlet-serializer/data/mesh_data.hpp"
#include "starlet-logger/logger.hpp"

#include "starlet-math/vec3.hpp"

#include <cstring>  
#include <cfloat>
#include <vector>

namespace Starlet::Serializer {

namespace {
	inline bool strncasecmp(const char* a, const char* b, size_t n) {
		for (size_t i = 0; i < n; ++i)
			if (tolower(a[i]) != tolower(b[i])) return false;
		return true;
	}
}

bool ObjParser::parse(const std::string& path, MeshData& out) {
	std::vector<unsigned char> file;
	if (!loadBinaryFile(file, path)) return false;

	if (file.empty() || file[0] == '\0')
		return Logger::error("ObjParser", "parse", "File is empty");

	std::vector<Starlet::Math::Vec3<float>> positions;

  const unsigned char* p = file.data();
  while (*p) {
    p = skipWhitespace(p);
    if (*p == '\0') break;

    if (*p == '#') {
      p = skipToNextLine(p);
      continue;
    }

    unsigned char cmd[32]{};
    if (!parseToken(p, cmd, sizeof(cmd))) {
      p = skipToNextLine(p);
      continue;
    }

		if (strcmp((const char*)cmd, "v") == 0) {
			Starlet::Math::Vec3<float> pos;
			if (!parseFloat(p, pos.x) || !parseFloat(p, pos.y) || !parseFloat(p, pos.z))
				return Logger::error("ObjParser", "parse", "Failed to parse vertex position at vertex " + std::to_string(positions.size()));

			float w;
			parseFloat(p, w); 

			positions.push_back(pos);
		}
		else {
			p = skipToNextLine(p);
		}
  }

	out.numVertices = positions.size();
	out.vertices.resize(out.numVertices);
	for (size_t i = 0; i < out.numVertices; ++i) {
		out.vertices[i].pos = positions[i];
	}

	return true;
}

}