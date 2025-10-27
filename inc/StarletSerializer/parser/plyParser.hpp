#pragma once

#include "parser.hpp"

#include "StarletMath/vertex.hpp"

namespace Starlet::Serializer {
	struct PlyData {
		std::vector<Math::Vertex> vertices;
		std::vector<unsigned int> indices;
		unsigned int numVertices{ 0 }, numIndices{ 0 }, numTriangles{ 0 };

		bool hasNormals{ false }, hasColours{ false }, hasTexCoords{ false };
		float minY{ 0.0f }, maxY{ 0.0 };
	};

	class PlyParser : public Parser {
	public:
		bool parse(const std::string& path, PlyData& out);

	private:
		bool parseElementLine(const unsigned char*& p, unsigned int& verticesOut, unsigned int& trianglesOut);
		bool parsePropertyLine(const unsigned char*& p, bool& hasNx, bool& hasNy, bool& hasNz, bool& hasR, bool& hasG, bool& hasB, bool& hasU, bool& hasV);
		bool parseHeaderLine(const unsigned char*& p, unsigned int& numVerticesOut, unsigned int& numTrianglesOut, bool& hasNormalsOut, bool& hasColoursOut, bool& hasTexCoordsOut);

		bool parseVertices(const unsigned char*& p, PlyData& out);
		bool parseIndices(const unsigned char*& p, PlyData& out);
	};
}