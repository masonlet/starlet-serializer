#pragma once

#include "parser.hpp"

#include "StarletScene/scene.hpp"

namespace Starlet {
	namespace Scene {
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
	}

	namespace Serializer {
		class SceneParser : public Parser {
		public:
			bool parse(Scene::Scene& scene, const std::string& path);

			bool parseModel(const unsigned char*& p, Scene::Model& out, Scene::TransformComponent& transform, Scene::ColourComponent& colour);
			bool parseLight(const unsigned char*& p, Scene::Light& light, Scene::TransformComponent& transform, Scene::ColourComponent& colour);
			bool parseCamera(const unsigned char*& p, Scene::Camera& out, Scene::TransformComponent& transform);
			bool parseTexture(const unsigned char*& p, Scene::TextureData& out);
			bool parseCubeTexture(const unsigned char*& p, Scene::TextureData& out);

			bool parseTriangle(const unsigned char*& p, Scene::Primitive& out, Scene::TransformComponent& transform, Scene::ColourComponent& colour);
			bool parseSquare(const unsigned char*& p, Scene::Primitive& out, Scene::TransformComponent& transform, Scene::ColourComponent& colour);
			bool parseCube(const unsigned char*& p, Scene::Primitive& out, Scene::TransformComponent& transform, Scene::ColourComponent& colour);

			bool parseSquareGrid(const unsigned char*& p, Scene::Grid& grid, Scene::TransformComponent& transform, Scene::ColourComponent& colour);
			bool parseCubeGrid(const unsigned char*& p, Scene::Grid& grid, Scene::TransformComponent& transform, Scene::ColourComponent& colour);

			bool parseColour(const unsigned char*& p, Math::Vec4<float>& colourOut);
			bool parseVelocity(const unsigned char*& p, Scene::VelocityComponent& velocity);

		private:
			bool parseSceneLine(const unsigned char*& p, Scene::Scene& scene);

			bool parseSpecialColour(const unsigned char*& p, Scene::ColourMode& mode);
			bool parseNamedColour(const unsigned char*& p, Math::Vec4<float>& colour);
			bool parseNumericColour(const unsigned char*& p, Math::Vec4<float>& out);

			bool parseLightType(const unsigned char*& p, Scene::LightType& type);

			template <Scene::PrimitiveType T>
			bool parsePrimitive(const unsigned char*& p, Scene::Primitive& out, Scene::TransformComponent& transform, Scene::ColourComponent& colour);

			template <Scene::GridType T>
			bool parseGrid(const unsigned char*& p, Scene::Grid& grid, Scene::TransformComponent& transform, Scene::ColourComponent& colour);
		};
	}
}