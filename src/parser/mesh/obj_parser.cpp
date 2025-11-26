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
	std::vector<unsigned char> file;
	if (!loadBinaryFile(file, path)) return false;

	if (file.empty() || file[0] == '\0')
		return Logger::error("ObjParser", "parse", "File is empty");

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

		// TODO: Impelement vertex/face parsing

    p = skipToNextLine(p);
  }

	return true;
}

}