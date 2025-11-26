#include "starlet-serializer/parser/mesh/obj_parser.hpp"
#include "starlet-serializer/data/mesh_data.hpp"
#include "starlet-logger/logger.hpp"

#include "starlet-math/vec3.hpp"

#include <cstring>  
#include <cfloat>
#include <vector>

namespace Starlet::Serializer {

bool ObjParser::parse(const std::string& path, MeshData& out) {
	std::vector<unsigned char> file;
	if (!loadBinaryFile(file, path)) return false;

	if (file.empty() || file[0] == '\0')
		return Logger::error("ObjParser", "parse", "File is empty");

	std::vector<Starlet::Math::Vec3<float>> positions;
	std::vector<Starlet::Math::Vec2<float>> texCoords;
	std::vector<Starlet::Math::Vec3<float>> normals; 
	std::vector<unsigned int> indices;

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

		if (strcmp(reinterpret_cast<const char*>(cmd), "v") == 0) {
			Starlet::Math::Vec3<float> pos;
			if (!parseFloat(p, pos.x) || !parseFloat(p, pos.y) || !parseFloat(p, pos.z))
				return Logger::error("ObjParser", "parse", "Failed to parse vertex position at vertex " + std::to_string(positions.size()));

			float w;
			parseFloat(p, w);

			positions.push_back(pos);
		}
		else if (strcmp(reinterpret_cast<const char*>(cmd), "vt") == 0) {
			Starlet::Math::Vec2<float> texCoord;
			if (!parseVec2f(p, texCoord))
				return Logger::error("ObjParser", "parse", "Failed to parse texture coordinate at texCoord " + std::to_string(texCoords.size()));

			float w;
			parseFloat(p, w);

			texCoords.push_back(texCoord);

		}
		else if (strcmp(reinterpret_cast<const char*>(cmd), "vn") == 0){
			Starlet::Math::Vec3<float> normal;

			if (!parseVec3f(p, normal))
				return Logger::error("ObjParser", "parse", "Failed to parse normal at normal " + std::to_string(normals.size()));

			normals.push_back(normal);
		}
		else if (strcmp((const char*)cmd, "f") == 0) {
			std::vector<unsigned int> faceIndices;

			while (true) {
				p = skipWhitespace(p);
				if (!*p || *p == '\n' || *p == '\r') break;

				int i = 0;
				bool negative = (*p == '-');
				if (negative) ++p;

				unsigned int absVal;
				if (!parseUInt(p, absVal)) break;

				i = negative ? -static_cast<int>(absVal) : static_cast<int>(absVal);

				if (i > 0) --i;
				else if (i < 0 ) i = static_cast<int>(positions.size()) + i;
				else return Logger::error("ObjParser", "parse", "Face index cannot be 0");

				if(i < 0 || i >= static_cast<int>(positions.size()))
					return Logger::error("ObjParser", "parse", "Face index out of bounds" + std::to_string(i));

				faceIndices.push_back(i);

				while (*p == '/') {
					++p;
					unsigned int dummy;
					parseUInt(p, dummy);
				}
			}

			if (faceIndices.size() < 3)
				return Logger::error("ObjParser", "parse", "Face has fewer than 3 vertices");

			for (size_t i = 2; i < faceIndices.size(); ++i) {
				indices.push_back(faceIndices[0]);
				indices.push_back(faceIndices[i - 1]);
				indices.push_back(faceIndices[i]);
			}
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

	out.numIndices = indices.size();
	out.numTriangles = out.numIndices / 3;
	out.indices = std::move(indices);

	return true;
}

}