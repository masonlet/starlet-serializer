#pragma once

#include "parser.hpp"

#include "StarletMath/vec2.hpp"
#include "StarletMath/vec3.hpp"
#include "StarletMath/vec4.hpp"

namespace Starlet::Serializer {
	enum class ColourMode {
		Solid,
		Random,
		VerticalGradient,
		PLYColour
	};
	enum class PrimitiveType {
		Triangle,
		Square,
		Cube
	};
	enum class GridType {
		Square,
		Cube,
		Model
	};
	enum class LightType {
		Point,
		Spot,
		Directional
	};

	struct TransformData {
		Math::Vec3<float> pos{ 0.0f };
		Math::Vec3<float> rot{ 0.0f };
		Math::Vec3<float> size{ 1.0f };
	};
	struct VelocityData {
		std::string modelName;
		Math::Vec3<float> velocity{ 1.0f };
	};
	struct ColourData {
		Math::Vec4<float> colour{ 1.0f };
		Math::Vec4<float> specular{ 1.0f, 1.0f, 1.0f, 32.0f };
	};

	struct NamedData {
		std::string name;
	};
	struct TransformedData : public NamedData {
		TransformData transform;
	};
	struct ColouredTransformedData : public TransformedData {
		ColourData colour;
	};

	struct CameraData : public TransformedData {
		float moveSpeed{ 25.0f }, mouseSpeed{ 0.1f };
		float fov{ 60.0f };
		float nearPlane{ 0.1f }, farPlane{ 10000.0f };
		bool enabled{ true }, paused{ false };
	};
	struct ModelData : public ColouredTransformedData {
		std::string meshPath;

		bool isVisible{ true }, isLighted{ true }, useTextures{ false };
		ColourMode mode{ ColourMode::Solid };

		static const unsigned int NUM_TEXTURES = 4;
		std::string textureNames[NUM_TEXTURES]{ "" };
		float textureMixRatio[NUM_TEXTURES]{ 1.0f, 0.0f, 0.0f, 0.0f };
		Math::Vec2<float> textureTiling{ 1.0f, 1.0f };
	};
	struct PrimitiveData : public ColouredTransformedData {
		PrimitiveType type;
	};
	struct LightData : public ColouredTransformedData {
		bool enabled{ true };
		LightType type{ LightType::Point };
		Math::Vec4<float> attenuation{ 0.0f, 0.01f, 0.01f, 1.0f };
		Math::Vec2<float> param1{ 0.0f };
	};
	struct GridData : public ColouredTransformedData {
		GridType type{ GridType::Cube };
		unsigned int count{ 1 };
		float spacing{ 1.0f };
	};
	struct TextureData : public NamedData {
		std::string faces[6];
		float mix{ 1.0f };
		Math::Vec2<float> tiling{ 1.0f, 1.0f };
		bool isCube{ false };
	};
	struct TextureConnection {
		std::string modelName, textureName;
		unsigned int slot{ 0 };
		float mix{ 1.0f };
	};

	struct SceneData {
		std::vector<ModelData> models;
		std::vector<LightData> lights;
		std::vector<CameraData> cameras;
		std::vector<PrimitiveData> primitives;
		std::vector<GridData> grids;
		std::vector<TextureData> textures;
		std::vector<TextureConnection> textureConnections;
		std::vector<VelocityData> velocities;

		bool ambientEnabled{ false };
		Math::Vec3<float> ambientLight{ 0.0f, 0.0f, 0.0f };
	};

	class SceneParser : public Parser {
	public:
		bool parse(const std::string& path, SceneData& scene);

		bool parseModel(const unsigned char*& p, ModelData& out);
		bool parseLight(const unsigned char*& p, LightData& light);
		bool parseCamera(const unsigned char*& p, CameraData& out);
		bool parseTexture(const unsigned char*& p, TextureData& out);
		bool parseCubeTexture(const unsigned char*& p, TextureData& out);

		bool parseTriangle(const unsigned char*& p, PrimitiveData& out);
		bool parseSquare(const unsigned char*& p, PrimitiveData& out);
		bool parseCube(const unsigned char*& p, PrimitiveData& out);

		bool parseSquareGrid(const unsigned char*& p, GridData& grid);
		bool parseCubeGrid(const unsigned char*& p, GridData& grid);

		bool parseColour(const unsigned char*& p, Math::Vec4<float>& colourOut);
		bool parseVelocity(const unsigned char*& p, VelocityData& velocity);

	private:
		bool parseSceneLine(const unsigned char*& p, SceneData& scene);

		bool parseSpecialColour(const unsigned char*& p, ColourMode& mode);
		bool parseNamedColour(const unsigned char*& p, Math::Vec4<float>& colour);
		bool parseNumericColour(const unsigned char*& p, Math::Vec4<float>& out);

		bool parseLightType(const unsigned char*& p, LightType& type);

		template <PrimitiveType T>
		bool parsePrimitive(const unsigned char*& p, PrimitiveData& out);

		template <GridType T>
		bool parseGrid(const unsigned char*& p, GridData& grid);
	};
}