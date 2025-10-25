#pragma once

#include "parser.hpp"

class Scene;

struct Camera;
struct Primitive;
struct Model;
struct Light;
struct Grid;
struct TextureData;

struct TransformComponent;
struct VelocityComponent;
struct ColourComponent;

enum class ColourMode;
enum class PrimitiveType;
enum class GridType;
enum class LightType;

class SceneParser : public Parser {
public:
	bool parse(Scene& scene, const std::string& path);

	bool parseModel(const unsigned char*& p, Model& out, TransformComponent& transform, ColourComponent& colour);
	bool parseLight(const unsigned char*& p, Light& light, TransformComponent& transform, ColourComponent& colour);
	bool parseCamera(const unsigned char*& p, Camera& out, TransformComponent& transform);
	bool parseTexture(const unsigned char*& p, TextureData& out);
	bool parseCubeTexture(const unsigned char*& p, TextureData& out);

	bool parseTriangle(const unsigned char*& p, Primitive& out, TransformComponent& transform, ColourComponent& colour);
	bool parseSquare(const unsigned char*& p, Primitive& out, TransformComponent& transform, ColourComponent& colour);
	bool parseCube(const unsigned char*& p, Primitive& out, TransformComponent& transform, ColourComponent& colour);

	bool parseSquareGrid(const unsigned char*& p, Grid& grid, TransformComponent& transform, ColourComponent& colour);
	bool parseCubeGrid(const unsigned char*& p, Grid& grid, TransformComponent& transform, ColourComponent& colour);

	bool parseColour(const unsigned char*& p, Vec4<float>& colourOut);
	bool parseVelocity(const unsigned char*& p, VelocityComponent& velocity);

private:
	bool parseSceneLine(const unsigned char*& p, Scene& scene);

	bool parseSpecialColour(const unsigned char*& p, ColourMode& mode);
	bool parseNamedColour(const unsigned char*& p, Vec4<float>& colour);
	bool parseNumericColour(const unsigned char*& p, Vec4<float>& out);

	bool parseLightType(const unsigned char*& p, LightType& type);

	template <PrimitiveType T>
	bool parsePrimitive(const unsigned char*& p, Primitive& out, TransformComponent& transform, ColourComponent& colour);

	template <GridType T>
	bool parseGrid(const unsigned char*& p, Grid& grid, TransformComponent& transform, ColourComponent& colour);
};