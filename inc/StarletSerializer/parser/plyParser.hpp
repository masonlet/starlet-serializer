#pragma once

#include "parser.hpp"


namespace Starlet {
	namespace Graphics {
		struct MeshCPU;
	}

	namespace Serializer {
		class PlyParser : public Parser {
		public:
			bool parse(const std::string& path, Graphics::MeshCPU& drawInfo);

		private:
			bool parseElementLine(const unsigned char*& p, unsigned int& verticesOut, unsigned int& trianglesOut);
			bool parsePropertyLine(const unsigned char*& p, bool& hasNx, bool& hasNy, bool& hasNz, bool& hasR, bool& hasG, bool& hasB, bool& hasU, bool& hasV);
			bool parseHeaderLine(const unsigned char*& p, unsigned int& numVerticesOut, unsigned int& numTrianglesOut, bool& hasNormalsOut, bool& hasColoursOut, bool& hasTexCoordsOut);

			bool parseVertices(const unsigned char*& p, Graphics::MeshCPU& drawInfo);
			bool parseIndices(const unsigned char*& p, Graphics::MeshCPU& drawInfo);
		};
	}
}