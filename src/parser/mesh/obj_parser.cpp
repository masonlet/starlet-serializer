#include "starlet-serializer/parser/mesh/obj_parser.hpp"
#include "starlet-serializer/data/mesh_data.hpp"
#include "starlet-logger/logger.hpp"

#include "starlet-math/vertex.hpp"
#include "starlet-math/vec3.hpp"
#include "starlet-math/vec2.hpp"

#include <cstring>  
#include <cfloat>
#include <vector>
#include <map>
#include <tuple>

namespace Starlet::Serializer {

bool ObjParser::parse(const std::string& path, MeshData& out) {
	std::vector<unsigned char> file;
	if (!loadBinaryFile(file, path)) return false;

	if (file.empty() || file[0] == '\0')
		return Logger::error("ObjParser", "parse", "File is empty");

	std::vector<Starlet::Math::Vec3<float>> positions;
	std::vector<Starlet::Math::Vec4<float>> colours;

	std::vector<Starlet::Math::Vec2<float>> texCoords;
	std::vector<Starlet::Math::Vec3<float>> normals; 

	bool usedTexCoords = false;
	bool usedNormals = false;

	std::map<std::tuple<int, int, int>, unsigned int> vertexMap;
	std::vector<Math::Vertex> vertices;
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
			if (!parsePosition(p, positions, colours))
				return Logger::error("ObjParser", "parse", "Failed to parse vertex position at vertex " + std::to_string(positions.size()));
		}
		else if (strcmp(reinterpret_cast<const char*>(cmd), "vt") == 0) {
			if (!parseTexCoord(p, texCoords))
				return Logger::error("ObjParser", "parse", "Failed to parse texture coordinate at texCoord " + std::to_string(texCoords.size()));
		}
		else if (strcmp(reinterpret_cast<const char*>(cmd), "vn") == 0) {
			if (!parseNormal(p, normals))
				return Logger::error("ObjParser", "parse", "Failed to parse normal at normal " + std::to_string(normals.size()));
		}
		else if (strcmp((const char*)cmd, "f") == 0) {
			std::vector<ObjVertex> faceVertices;

			while (true) {
				p = skipWhitespace(p);
				if (!*p || *p == '\n' || *p == '\r') break;

				ObjVertex fv{ -1, -1, -1 };

				int posI = 0;
				bool negative = (*p == '-');
				if (negative) ++p;

				unsigned int absVal;
				if (!parseUInt(p, absVal)) break;
				posI = negative ? -static_cast<int>(absVal) : static_cast<int>(absVal);

				if (posI > 0) --posI;
				else if (posI < 0) posI = static_cast<int>(positions.size()) + posI;
				else return Logger::error("ObjParser", "parse", "Face index cannot be 0");

				if (posI < 0 || posI >= static_cast<int>(positions.size()))
					return Logger::error("ObjParser", "parse", "Face index out of bounds" + std::to_string(posI));

				fv.posI = posI;

				while (*p == '/') {
					++p;

					if (*p == '/') {
						++p;

						if (*p != ' ' && *p != '\n' && *p != '\r') {
							int normI{ 0 };
							negative = (*p == '-');
							if (negative) ++p;

							if (parseUInt(p, absVal)) {
								normI = negative ? -static_cast<int>(absVal) : static_cast<int>(absVal);

								if (normI > 0) normI--;
								else if (normI < 0) normI = static_cast<int>(normals.size()) + normI;
								else return Logger::error("ObjParser", "parse", "Normal index cannot be 0");

								if (normI < 0 || normI >= static_cast<int>(normals.size()))
									return Logger::error("ObjParser", "parse", "Normal index out of bounds: " + std::to_string(normI));

								fv.normI = normI;
							}
						}
						break;
					}

					if (*p == ' ' || *p == '\n' || *p == '\r')
						return Logger::error("ObjParser", "parse", "Expected texture coordinate index after '/'");

					int texCoordI{ 0 };
					negative = (*p == '-');
					if (negative) ++p;

					if (!parseUInt(p, absVal))
						return Logger::error("ObjParser", "parse", "Expected texture coordinate index after '/'");

					texCoordI = negative ? -static_cast<int>(absVal) : static_cast<int>(absVal);

					if (texCoordI > 0) texCoordI--;
					else if (texCoordI < 0) texCoordI = static_cast<int>(texCoords.size()) + texCoordI;
					else return Logger::error("ObjParser", "parse", "TexCoord index cannot be 0");

					if (texCoordI < 0 || texCoordI >= static_cast<int>(texCoords.size()))
						return Logger::error("ObjParser", "parse", "TexCoord index out of bounds: " + std::to_string(texCoordI));

					fv.texI = texCoordI;

					if (*p == '/') {
						++p;

						if (*p != ' ' && *p != '\n' && *p != '\r') {
							int normI{ 0 };
							negative = (*p == '-');
							if (negative) ++p;

							if (parseUInt(p, absVal)) {
								normI = negative ? -static_cast<int>(absVal) : static_cast<int>(absVal);

								if (normI > 0) normI--;
								else if (normI < 0) normI = static_cast<int>(normals.size()) + normI;
								else return Logger::error("ObjParser", "parse", "Normal index cannot be 0");

								if (normI < 0 || normI >= static_cast<int>(normals.size()))
									return Logger::error("ObjParser", "parse", "Normal index out of bounds: " + std::to_string(normI));

								fv.normI = normI;
							}
						}
					}
					break;
				}

				faceVertices.push_back(fv);
			}

			if (faceVertices.size() < 3)
				return Logger::error("ObjParser", "parse", "Face has fewer than 3 vertices");

			std::vector<unsigned int> faceIndices;
			for (const ObjVertex& fv : faceVertices) {
				std::tuple<int, int, int> key = std::make_tuple(fv.posI, fv.texI, fv.normI);

				auto it = vertexMap.find(key);
				if (it != vertexMap.end())
					faceIndices.push_back(it->second);
				else {
					Math::Vertex v{};
					v.pos = positions[fv.posI];
					v.col = colours[fv.posI];
					if (fv.texI >= 0) {
						v.texCoord = texCoords[fv.texI];
						usedTexCoords = true;
					}
					if (fv.normI >= 0) {
						v.norm = normals[fv.normI];
						usedNormals = true;
					}

					unsigned int i = static_cast<unsigned int>(vertices.size());
					vertices.push_back(v);
					vertexMap[key] = i;
					faceIndices.push_back(i);
				}
			}

			for (size_t i = 2; i < faceIndices.size(); ++i) {
				indices.push_back(faceIndices[0]);
				indices.push_back(faceIndices[i - 1]);
				indices.push_back(faceIndices[i]);
			}
		}
		else p = skipToNextLine(p);
  }

	if (vertices.empty() && !positions.empty()) {
		for (size_t i = 0; i < positions.size(); ++i) {
			Math::Vertex v{};

			v.pos = positions[i];
			v.col = colours[i];

			vertices.push_back(v);
		}
	}

	fillMeshData(out, vertices, indices, usedTexCoords, usedNormals);
	return true;
}

bool ObjParser::parsePosition(const unsigned char*& p, 
	std::vector<Starlet::Math::Vec3<float>>& positions,
	std::vector<Starlet::Math::Vec4<float>>& colours) {

	Starlet::Math::Vec3<float> pos;
	if (!parseVec3f(p, pos))
		return false;

	Starlet::Math::Vec4<float> col{ 1.0f, 1.0f, 1.0f, 1.0f };
	float w{ 1.0f };

	const unsigned char* start = p;
	std::vector<float> extra;
	while (true) {
		float val;
		const unsigned char* save = p;
		if (parseFloat(p, val))
			extra.push_back(val);
		else {
			p = save;
			break;
		}
	}

	if (extra.size() == 1) {
		w = extra[0];
	}
	else if (extra.size() == 3) {
		col.x = extra[0];
		col.y = extra[1];
		col.z = extra[2];
	}
	else if (extra.size() == 4) {
		col.x = extra[0];
		col.y = extra[1];
		col.z = extra[2];
		col.w = extra[3];
	}

	colours.push_back(col);
	positions.push_back(pos);
	return true;
}

bool ObjParser::parseTexCoord(const unsigned char*& p, std::vector<Starlet::Math::Vec2<float>>& texCoords) {
	Starlet::Math::Vec2<float> tex;
	if (!parseVec2f(p, tex))
		return false;

	float w;
	parseFloat(p, w);

	texCoords.push_back(tex);
	return true;
}

bool ObjParser::parseNormal(const unsigned char*& p, std::vector<Starlet::Math::Vec3<float>>& normals) {
	Starlet::Math::Vec3<float> norm;
	if (!parseVec3f(p, norm))
		return false;

	normals.push_back(norm);
	return true;
}

void ObjParser::fillMeshData(
	MeshData& out,
	std::vector<Starlet::Math::Vertex>& vertices,
	std::vector<unsigned int>& indices,
	bool usedTexCoords,
	bool usedNormals
) {
	out.vertices = std::move(vertices);
	out.numVertices = out.vertices.size();

	out.numIndices = indices.size();
	out.numTriangles = out.numIndices / 3;
	out.indices = std::move(indices);

	out.hasTexCoords = usedTexCoords;
	out.hasNormals = usedNormals;
}

}