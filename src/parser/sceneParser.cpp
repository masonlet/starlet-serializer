#include "StarletSerializer/parser/sceneParser.hpp"
#include "StarletSerializer/utils/log.hpp"

#include "StarletScene/component/model.hpp"
#include "StarletScene/component/light.hpp"
#include "StarletScene/component/camera.hpp"
#include "StarletScene/component/grid.hpp"
#include "StarletScene/component/textureData.hpp"
#include "StarletScene/component/primitive.hpp"
#include "StarletScene/component/transform.hpp"
#include "StarletScene/component/velocity.hpp"
#include "StarletScene/component/colour.hpp"

#include "StarletScene/scene.hpp"

bool SceneParser::parse(Scene& scene, const std::string& path) {
	std::string src{};
	if (!loadFile(src, path)) return error("SceneLoader", "loadScene", "Failed to load path: " + path);

	const unsigned char* p = reinterpret_cast<const unsigned char*>(src.c_str());
	while (*p) {
		const unsigned char* nextLine = skipToNextLine(p);
		const unsigned char* endLine = trimEOL(p, nextLine);

		if (endLine <= p) {
			p = nextLine;
			continue;
		}

		if (!parseSceneLine(p, scene)) {
			const std::size_t maxLen = 256;
			std::size_t len = static_cast<std::size_t>(endLine - p);
			if (len > maxLen) len = maxLen;
			std::string errorMsg;
			errorMsg.reserve(len);
			errorMsg.append(reinterpret_cast<const char*>(p), len);
			return error("SceneManager", "loadTxtScene", std::string("Failed to process scene line: \"") + errorMsg + "\"");
		}

		if (p < nextLine) p = nextLine;
	}
	return true;
}

bool SceneParser::parseSceneLine(const unsigned char*& p, Scene& scene) {
	if (!p || *p == '\0') return true;

	unsigned char token[64]{};
	parseToken(p, token, sizeof(token));

	const char* nameStr = reinterpret_cast<const char*>(token);
	if (!p || strlen(nameStr) == 0) return true;

	if (strcmp(nameStr, "comment") == 0 || nameStr[0] == '#') {
		p = skipToNextLine(p);
		return true;
	}
	else if (strcmp(nameStr, "model") == 0) {
		StarEntity entity = scene.createEntity();
		Model* model = scene.addComponent<Model>(entity);
		TransformComponent* transform = scene.addComponent<TransformComponent>(entity);
		ColourComponent* colour = scene.addComponent<ColourComponent>(entity);
		return parseModel(p, *model, *transform, *colour);
	}
	else if (strcmp(nameStr, "light") == 0) {
		StarEntity entity = scene.createEntity();
		Light* light = scene.addComponent<Light>(entity);
		TransformComponent* transform = scene.addComponent<TransformComponent>(entity);
		ColourComponent* colour = scene.addComponent<ColourComponent>(entity);
		return parseLight(p, *light, *transform, *colour);
	}
	else if (strcmp(nameStr, "camera") == 0) {
		StarEntity entity = scene.createEntity();
		Camera* camera = scene.addComponent<Camera>(entity);
		TransformComponent* transform = scene.addComponent<TransformComponent>(entity);
		return parseCamera(p, *camera, *transform);
	}
	else if (strcmp(nameStr, "texture") == 0) {
		StarEntity entity = scene.createEntity();
		TextureData* texture = scene.addComponent<TextureData>(entity);
		return parseTexture(p, *texture);
	}
	else if (strcmp(nameStr, "textureCube") == 0) {
		StarEntity entity = scene.createEntity();
		TextureData* texture = scene.addComponent<TextureData>(entity);
		return parseCubeTexture(p, *texture);
	}
	else if (strcmp(nameStr, "textureAdd") == 0) {
		struct TextureConnection {
			std::string modelName, textureName;
			unsigned int slot{ 0 };
			float mix{ 1.0f };
		} data;
		
		PARSE_STRING_OR(return false, p, data.modelName, 64, "texture connection model name");
		PARSE_OR(return false, parseUInt, data.slot, "texture connection slot");
		if (data.slot >= Model::NUM_TEXTURES)
			return error("Parser", "textureAdd", "Invalid texture slot index: " + std::to_string(data.slot) + " for model: " + data.modelName);

		PARSE_STRING_OR(return false, p, data.textureName, 128, "texture connection name");
		PARSE_OR(return false, parseFloat, data.mix, "texture connection mix");

		StarEntity e = scene.getEntityByName<Model>(data.modelName);
		if (e == -1) return error("Parser", "textureAdd", "Entity is not a model or not found: " + data.modelName);

		Model& model = scene.getComponent<Model>(e);
		model.textureNames[data.slot] = data.textureName;
		model.textureMixRatio[data.slot] = data.mix;
		model.useTextures = true;
		return true;
	}
	else if (strcmp(nameStr, "triangle") == 0) {
		StarEntity entity = scene.createEntity();
		Primitive* primitive = scene.addComponent<Primitive>(entity);
		TransformComponent* transform = scene.addComponent <TransformComponent>(entity);
		ColourComponent* colour = scene.addComponent<ColourComponent>(entity);
		return parseTriangle(p, *primitive, *transform, *colour);
	}
	else if (strcmp(nameStr, "square") == 0) {
		StarEntity entity = scene.createEntity();
		Primitive* primitive = scene.addComponent<Primitive>(entity);
		TransformComponent* transform = scene.addComponent<TransformComponent>(entity);
		ColourComponent* colour = scene.addComponent<ColourComponent>(entity);
		return parseSquare(p, *primitive, *transform, *colour);
	}
	else if (strcmp(nameStr, "cube") == 0) {
		StarEntity entity = scene.createEntity();
		Primitive* primitive = scene.addComponent<Primitive>(entity);
		TransformComponent* transform = scene.addComponent<TransformComponent>(entity);
		ColourComponent* colour = scene.addComponent<ColourComponent>(entity);
		return parseCube(p, *primitive, *transform, *colour);
	}
	else if (strcmp(nameStr, "squareGrid") == 0) {
		StarEntity entity = scene.createEntity();
		Grid* grid = scene.addComponent<Grid>(entity);
		TransformComponent* transform = scene.addComponent<TransformComponent>(entity);
		ColourComponent* colour = scene.addComponent<ColourComponent>(entity);
		return parseSquareGrid(p, *grid, *transform, *colour);
	}
	else if (strcmp(nameStr, "cubeGrid") == 0) {
		StarEntity entity = scene.createEntity();
		Grid* grid = scene.addComponent<Grid>(entity);
		TransformComponent* transform = scene.addComponent<TransformComponent>(entity);
		ColourComponent* colour = scene.addComponent<ColourComponent>(entity);
		return parseCubeGrid(p, *grid, *transform, *colour);
	}
	else if (strcmp(nameStr, "velocity") == 0) {
		unsigned char nameToken[256]{};
		parseToken(p, nameToken, sizeof(nameToken));
		const std::string entityName = reinterpret_cast<const char*>(nameToken);

		StarEntity entity = scene.getEntityByName<Model>(entityName);
		if (entity != -1) {
			VelocityComponent* velocity = scene.addComponent<VelocityComponent>(entity);
			if (velocity) return parseVelocity(p, *velocity);
			else return error("SceneLoader", "processSceneLine", "Failed to add VelocityComponent to entity " + entityName);
		}
	}
	else if (strcmp(nameStr, "ambient") == 0) {
		bool enabled{ false };
		if (!parseBool(p, enabled)) return error("SceneLoader", "processSceneLine", "Failed to parse ambient enabled");

		Vec3 colour{ 0.0f, 0.0f, 0.0f };
		if (!parseVec3f(p, colour)) return error("SceneLoader", "processSceneLine", "Failed to parse ambient colour");

		if (enabled) scene.setAmbientLight({ colour, 1.0f });
		else scene.setAmbientLight(Vec4{ 0.0f, 0.0f, 0.0f, 1.0f });
		return true;
	}

	return error("SceneManager", "processSceneLine", "Failed to handle: " + std::string(nameStr));
}