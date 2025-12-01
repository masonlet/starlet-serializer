#pragma once

#include "starlet-serializer/parser/parser.hpp"

#include <vector>

namespace Starlet {

namespace Math {
	struct Vertex;
	template<typename T> struct Vec3;
	template<typename T> struct Vec2;
}

namespace Serializer {
	struct MeshData;

	class ObjParser : public Parser {
	public:
		bool parse(const std::string& path, MeshData& out);

	private:
		struct ObjVertex {
			int posI;
			int texI;
			int normI;
		};

		bool parsePosition(const unsigned char*& p, 
			std::vector<Starlet::Math::Vec3<float>>& positions,
			std::vector<Starlet::Math::Vec4<float>>& colours);
		bool parseTexCoord(const unsigned char*& p, std::vector<Starlet::Math::Vec2<float>>& texCoords);
		bool parseNormal(const unsigned char*& p, std::vector<Starlet::Math::Vec3<float>>& normals);

		void fillMeshData(
			MeshData& out,
			std::vector<Starlet::Math::Vertex>& vertices,
			std::vector<unsigned int>& indices,
			bool usedTexCoords, bool usedNormals, bool usedColours
		);
	};
}

}