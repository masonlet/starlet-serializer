#pragma once

#include <vector>

#include "StarletMath/vertex.hpp"

namespace Starlet::Serializer {

struct PlyData {
	std::vector<Math::Vertex> vertices;
	std::vector<unsigned int> indices;
	unsigned int numVertices{ 0 }, numIndices{ 0 }, numTriangles{ 0 };

	bool hasNormals{ false }, hasColours{ false }, hasTexCoords{ false };
	float minY{ 0.0f }, maxY{ 0.0 };
};

}