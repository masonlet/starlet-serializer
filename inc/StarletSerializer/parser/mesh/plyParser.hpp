#pragma once

#include "meshParserBase.hpp"

namespace Starlet::Serializer {

struct MeshData;

class PlyParser : public MeshParserBase {
public:
	bool parse(const std::string& path, MeshData& out);

private:
	bool parseElementLine(const unsigned char*& p, unsigned int& verticesOut, unsigned int& trianglesOut);
	bool parsePropertyLine(const unsigned char*& p, bool& hasNx, bool& hasNy, bool& hasNz, bool& hasR, bool& hasG, bool& hasB, bool& hasU, bool& hasV);
	bool parseHeaderLine(const unsigned char*& p, unsigned int& numVerticesOut, unsigned int& numTrianglesOut, bool& hasNormalsOut, bool& hasColoursOut, bool& hasTexCoordsOut);

	bool parseVertices(const unsigned char*& p, MeshData& out);
	bool parseIndices(const unsigned char*& p, MeshData& out);
};

}