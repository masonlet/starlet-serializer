#include "starlet-serializer/parser/mesh/ply_parser.hpp"
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

bool PlyParser::parse(const std::string& path, MeshData& out) {
	std::vector<unsigned char> file;
	if (!loadBinaryFile(file, path)) return false;

	if (file.empty()) return Logger::error("PlyParser", "parse", "File is empty");

	const unsigned char* p = file.data();
	std::string errorMsg;
	while (true) {
		if (!parseHeaderLine(p, out.numVertices, out.numTriangles, out.hasNormals, out.hasColours, out.hasTexCoords)) {
			errorMsg = "header, 'end_header' not found";
			break;
		}

		if (out.numVertices == 0 || out.numTriangles == 0) {
			errorMsg = "header, no vertices/triangles declared";
			break;
		}

		out.vertices.assign(out.numVertices, Math::Vertex{});
		if (!parseVertices(p, out)) {
			errorMsg = "vertex data";
			break;
		}

		out.numIndices = out.numTriangles * 3;
		out.indices.assign(out.numIndices, 0u);
		if (!parseIndices(p, out)) {
			errorMsg = "face data";
			break;
		}

		return true;
	}

	out.indices.clear();
	out.vertices.clear();
	out.numVertices = out.numIndices = out.numTriangles = 0;
	return Logger::error("PlyParser", "parse", ("Failed to parse " + errorMsg).c_str());
}

bool PlyParser::parseHeaderLine(const unsigned char*& p, unsigned int& numVerticesOut, unsigned int& numTrianglesOut, bool& hasNormalsOut, bool& hasColoursOut, bool& hasTexCoordsOut) {
	if (!p) return Logger::error("PlyParser", "parseHeaderLine", "Input pointer is null");
	p = skipWhitespace(p);

	bool hasNx = false, hasNy = false, hasNz = false;
	bool hasRed = false, hasGreen = false, hasBlue = false;
	bool hasU = false, hasV = false;

	while (*p) {
		const unsigned char* nextLine = skipToNextLine(p);
		const unsigned char* lineEnd = trimEOL(p, nextLine);

		if (lineEnd == p) {
			p = nextLine;
			continue;
		}

		if (strncmp((const char*)p, "element", 7) == 0 && (p[7] == ' ' || p[7] == '\t')) {
			if (!parseElementLine(p, numVerticesOut, numTrianglesOut))
				return false;
		}
		else if (strncmp((const char*)p, "property", 8) == 0) {
			if (!parsePropertyLine(p, hasNx, hasNy, hasNz, hasRed, hasGreen, hasBlue, hasU, hasV))
				return false;
		}
		else if (strncmp((const char*)p, "end_header", 10) == 0) {
			hasNormalsOut = hasNx && hasNy && hasNz;
			hasColoursOut = hasRed && hasGreen && hasBlue;
			hasTexCoordsOut = hasU && hasV;
			p = nextLine;
			return true;
		}
		else if (!(strncasecmp((const char*)p, "ply", 3) == 0)
			&& !(strncasecmp((const char*)p, "format", 6) == 0)
			&& !(strncasecmp((const char*)p, "comment", 7) == 0))
			Logger::debug("plyParser", "parseHeaderLine", ("Unknown line in PLY header: %.*s\n" + std::string((const char*)p, static_cast<size_t>(lineEnd - p))).c_str());

		p = nextLine;
	}

	return false;
}

bool PlyParser::parseElementLine(const unsigned char*& p, unsigned int& verticesOut, unsigned int& trianglesOut) {
	if (!p) return Logger::error("PlyParser", "parseElementLine", "Input pointer is null");

	p = skipWhitespace(p += 7);
	if (strncmp((const char*)p, "vertex", 6) == 0 && (p[6] == ' ' || p[6] == '\t')) {
		p = skipWhitespace(p += 6);
		return parseUInt(p, verticesOut);
	}
	else if (strncmp((const char*)p, "face", 4) == 0 && (p[4] == ' ' || p[4] == '\t')) {
		p = skipWhitespace(p += 4);
		return parseUInt(p, trianglesOut);
	}
	return false;
}
bool PlyParser::parsePropertyLine(const unsigned char*& p, bool& hasNx, bool& hasNy, bool& hasNz, bool& hasR, bool& hasG, bool& hasB, bool& hasU, bool& hasV) {
	if (!p) return Logger::error("PlyParser", "parsePropertyLine", "Input pointer is null");
	p = skipWhitespace(p += 8);

	char type[32]{};
	if (!parseToken(p, (unsigned char*)type, sizeof(type)))
		return Logger::error("PlyParser", "parsePropertyLine", "Failed to parse property type: " + std::string(type));

	if (strcmp(type, "list") == 0) {
		char countType[32]{}, valueType[32]{}, propertyName[32]{};
		if (!parseToken(p, reinterpret_cast<unsigned char*>(countType), sizeof(countType)))
			return Logger::error("PlyParser", "parsePropertyLine", "Failed to parse property list: count type");
		if (!parseToken(p, reinterpret_cast<unsigned char*>(valueType), sizeof(valueType)))
			return Logger::error("PlyParser", "parsePropertyLine", "Failed to parse property list: value type");
		if (!parseToken(p, reinterpret_cast<unsigned char*>(propertyName), sizeof(propertyName)))
			return Logger::error("PlyParser", "parsePropertyLine", "Failed to parse property list: property name");
		return true;
	}

	char propertyName[32]{};
	if (!parseToken(p, (unsigned char*)propertyName, sizeof(propertyName)))
		return Logger::error("PlyParser", "parsePropertyLine", "Failed to parse property name: " + std::string(propertyName));

	if      (strcmp(propertyName, "nx") == 0 || strcmp(propertyName, "normal_x") == 0) hasNx = true;
	else if (strcmp(propertyName, "ny") == 0 || strcmp(propertyName, "normal_y") == 0) hasNy = true;
	else if (strcmp(propertyName, "nz") == 0 || strcmp(propertyName, "normal_z") == 0) hasNz = true;
	else if (strcmp(propertyName, "red") == 0)   hasR = true;
	else if (strcmp(propertyName, "green") == 0) hasG = true;
	else if (strcmp(propertyName, "blue") == 0)  hasB = true;
	else if (strcmp(propertyName, "u") == 0 || strcmp(propertyName, "texture_u") == 0) hasU = true;
	else if (strcmp(propertyName, "v") == 0 || strcmp(propertyName, "texture_v") == 0) hasV = true;
	return true;
}

bool PlyParser::parseVertices(const unsigned char*& p, MeshData& out) {
	if (!p) return Logger::error("PlyParser", "parseVertices", "Input pointer is null");
	if (!out.numVertices) return Logger::error("PlyParser", "parseVertices", "No vertices declared in header");

	float minY = FLT_MAX, maxY = -FLT_MAX;	
	unsigned int i = 0;
	while (i < out.numVertices && *p) {
		Math::Vertex& v = out.vertices[i];
		const unsigned char* nextLine = skipToNextLine(p);
		const unsigned char* lineEnd = trimEOL(p, nextLine);

		if (lineEnd == p) {
			p = nextLine;
			continue;
		}

		if (*p == '\0') return Logger::error("PlyParser", "parseVertices", "Unexpected end of data");

		if (!parseFloat(p, v.pos.x))
			return Logger::error("PlyParser", "parseVertices", "Failed to parse position X at vertex " + std::to_string(i));
		if (!parseFloat(p, v.pos.y))
			return Logger::error("PlyParser", "parseVertices", "Failed to parse position Y at vertex " + std::to_string(i));
		if (!parseFloat(p, v.pos.z))
			return Logger::error("PlyParser", "parseVertices", "Failed to parse position Z at vertex " + std::to_string(i));

		if (out.hasNormals) {
			if (!parseFloat(p, v.norm.x))
				return Logger::error("PlyParser", "parseVertices", "Failed to parse normal X at vertex " + std::to_string(i));
			if (!parseFloat(p, v.norm.y))
				return Logger::error("PlyParser", "parseVertices", "Failed to parse normal Y at vertex " + std::to_string(i));
			if (!parseFloat(p, v.norm.z))
				return Logger::error("PlyParser", "parseVertices", "Failed to parse normal Z at vertex " + std::to_string(i));
		}

		if (out.hasColours) {
			const unsigned char* original = p;
			Math::Vec3 colour = { 1.0f, 1.0f, 1.0f };

			if (parseFloat(p, colour.r) && 
					parseFloat(p, colour.g) && 
					parseFloat(p, colour.b) &&
					colour.r <= 1.0f && colour.g <= 1.0f && colour.b <= 1.0f) {
				v.col = Math::Vec4{ colour.r, colour.g, colour.b, 1.0f };
			} else {
				p = original;
				unsigned int ri = 0, gi = 0, bi = 0, ai = 256;

				if (!parseUInt(p, ri) || !parseUInt(p, gi) || !parseUInt(p, bi))
					return Logger::error("PlyParser", "parseVertices", "Failed to parse colour at vertex " + std::to_string(i));
				if (ri > 255 || gi > 255 || bi > 255)
					return Logger::error("PlyParser", "parseVertices", "Colour out of range at vertex " + std::to_string(i));

				parseUInt(p, ai);
				if (ai > 255) ai = 255;

				v.col = Math::Vec4{ ri / 255.0f, gi / 255.0f, bi / 255.0f, ai / 255.0f };
			}
		}

		if (out.hasTexCoords) {
			if (!parseFloat(p, v.texCoord.x))
				return Logger::error("PlyParser", "parseVertices", "Failed to parse texCoord X at vertex " + std::to_string(i));
			if (!parseFloat(p, v.texCoord.y))
				return Logger::error("PlyParser", "parseVertices", "Failed to parse texCoord Y at vertex " + std::to_string(i));
		}

		if (v.pos.y < minY) minY = v.pos.y;
		if (v.pos.y > maxY) maxY = v.pos.y;

		++i;
		p = nextLine;
	}

	if (i != out.numVertices) 
		return Logger::error("PlyParser", "parseVertices", "Vertex count declared: " + std::to_string(out.numVertices) + " but parsed: " + std::to_string(i));

	out.minY = minY;
	out.maxY = maxY;
	return true;
}
bool PlyParser::parseIndices(const unsigned char*& p, MeshData& out) {
	if (!p) return Logger::error("PlyParser", "parseIndices", "Input pointer is null");
	if (out.indices.empty() || out.numIndices == 0) 
		return Logger::error("PlyParser", "parseIndices", "Index buffer not allocated");

	unsigned int i = 0;
	while (i < out.numTriangles && *p) {
		const unsigned char* nextLine = skipToNextLine(p);
		const unsigned char* lineEnd = trimEOL(p, nextLine);

		if (lineEnd == p) {
			p = nextLine;
			continue;
		}

		unsigned int count = 0;
		if (!parseUInt(p, count))
			return Logger::error("PlyParser", "parseIndices", "Failed to parse face vertex count at triangle " + std::to_string(i));

		if (count != 3)
			return Logger::error("PlyParser", "parseIndices", "Non-triangle face detected (vertex count: " + std::to_string(count) + ") at triangle " + std::to_string(i));

		unsigned int i0{ 0 }, i1{ 0 }, i2{ 0 };
		if (!parseUInt(p, i0) || !parseUInt(p, i1) || !parseUInt(p, i2))
			return Logger::error("PlyParser", "parseIndices", "Failed to parse face indices at triangle " + std::to_string(i));

		if (i0 >= out.numVertices || i1 >= out.numVertices || i2 >= out.numVertices)
			return Logger::error("PlyParser", "parseIndices", "Index out of bounds at triangle " + std::to_string(i));

		size_t base = static_cast<size_t>(i) * 3;
		out.indices[base + 0] = i0;
		out.indices[base + 1] = i1;
		out.indices[base + 2] = i2;
		++i;

		p = nextLine;
	}

	if (i != out.numTriangles) 
		return Logger::error("PlyParser", "parseIndices", "Face count declared: " + std::to_string(out.numTriangles) +	" but parsed: " + std::to_string(i));

	return true;
}
}