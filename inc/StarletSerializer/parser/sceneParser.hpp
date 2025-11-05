#pragma once

#include "parser.hpp"

namespace Starlet::Serializer {

struct SceneData;
struct ModelData;
struct LightData;
struct CameraData;
struct TextureData;
struct PrimitiveData;
struct GridData;
struct VelocityData;
enum class ColourMode;
enum class LightType;
enum class PrimitiveType;
enum class GridType;

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