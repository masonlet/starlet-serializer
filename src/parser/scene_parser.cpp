#include "starlet-serializer/parser/scene_parser.hpp"
#include "starlet-serializer/data/scene_data.hpp"

#include "starlet-logger/logger.hpp"

namespace Starlet::Serializer {

bool SceneParser::parse(const std::string& path, SceneData& scene) {
	std::string src{};
	if (!loadFile(src, path)) return Logger::error("SceneParser", "parse", "Failed to load path: " + path);

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
			return Logger::error("SceneParser", "parse", std::string("Failed to process scene line: \"") + errorMsg + "\"");
		}

		if (p < nextLine) p = nextLine;
	}
	return true;
}

bool SceneParser::parseSceneLine(const unsigned char*& p, SceneData& scene) {
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
		ModelData model;
		if (!parseModel(p, model))
			return Logger::error("SceneParser", "parseSceneLine", "Failed to parse model");

		scene.models.push_back(model);
		return true;
	}
	else if (strcmp(nameStr, "light") == 0) {
		LightData light;
		if(!parseLight(p, light))
			return Logger::error("SceneParser", "parseSceneLine", "Failed to parse light");

		scene.lights.push_back(light);
		return true;
	}
	else if (strcmp(nameStr, "camera") == 0) {
		CameraData camera;
		if(!parseCamera(p, camera))
			return Logger::error("SceneParser", "parseSceneLine", "Failed to parse camera");

		scene.cameras.push_back(camera);
		return true;
	}
	else if (strcmp(nameStr, "texture") == 0) {
		TextureData texture;
		if(!parseTexture(p, texture))
			return Logger::error("SceneParser", "parseSceneLine", "Failed to parse texture");

		scene.textures.push_back(texture);
		return true;
	}
	else if (strcmp(nameStr, "textureCube") == 0) {
		TextureData texture;
		if(!parseCubeTexture(p, texture))
			return Logger::error("SceneParser", "parseSceneLine", "Failed to parse cube texture");

		scene.textures.push_back(texture);
		return true;
	}
	else if (strcmp(nameStr, "textureAdd") == 0) {
		TextureConnection data;
		STARLET_PARSE_STRING_OR(return false, p, data.modelName, 64, "texture connection model name");
		STARLET_PARSE_OR(return false, parseUInt, data.slot, "texture connection slot");
		if (data.slot >= ModelData::NUM_TEXTURES)
			return Logger::error("SceneParser", "parseSceneLine", "Invalid texture slot index: " + std::to_string(data.slot) + " for model: " + data.modelName);

		STARLET_PARSE_STRING_OR(return false, p, data.textureName, 128, "texture connection name");
		STARLET_PARSE_OR(return false, parseFloat, data.mix, "texture connection mix");

		scene.textureConnections.push_back(data);
		return true;
	}
	else if (strcmp(nameStr, "triangle") == 0) {
		PrimitiveData primitive;
		if(!parseTriangle(p, primitive))
			return Logger::error("SceneParser", "parseSceneLine", "Failed to parse triangle");

		scene.primitives.push_back(primitive);
		return true;
	}
	else if (strcmp(nameStr, "square") == 0) {
		PrimitiveData primitive;
		if(!parseSquare(p, primitive))
			return Logger::error("SceneParser", "parseSceneLine", "Failed to parse square");

		scene.primitives.push_back(primitive);
		return true;
	}
	else if (strcmp(nameStr, "cube") == 0) {
		PrimitiveData primitive;
		if(!parseCube(p, primitive))
			return Logger::error("SceneParser", "parseSceneLine", "Failed to parse cube");

		scene.primitives.push_back(primitive);
		return true;
	}
	else if (strcmp(nameStr, "squareGrid") == 0) {
		GridData grid;
		if(!parseSquareGrid(p, grid))
			return Logger::error("SceneParser", "parseSceneLine", "Failed to parse square grid");

		scene.grids.push_back(grid);
		return true;
	}
	else if (strcmp(nameStr, "cubeGrid") == 0) {
		GridData grid;
		if(!parseCubeGrid(p, grid))
			return Logger::error("SceneParser", "parseSceneLine", "Failed to parse cube grid");

		scene.grids.push_back(grid);
		return true;
	}
	else if (strcmp(nameStr, "velocity") == 0) {
		VelocityData velocity;

		unsigned char nameToken[256]{};
		parseToken(p, nameToken, sizeof(nameToken));
		velocity.modelName = reinterpret_cast<const char*>(nameToken);

		if(!parseVelocity(p, velocity))
			return Logger::error("SceneParser", "parseSceneLine", "Failed to parse velocity");

		scene.velocities.push_back(velocity);
		return true;
	}
	else if (strcmp(nameStr, "ambient") == 0) {
		unsigned char tok[64]{};
		const unsigned char* savePos = p;
		parseToken(p, tok, sizeof(tok));
		p = savePos;

		const char* s = reinterpret_cast<const char*>(tok);
		if (!(strcmp(s, "true") == 0 ||
			    strcmp(s, "false") == 0 ||
			    strcmp(s, "on") == 0 ||
			    strcmp(s, "off") == 0 ||
			    strcmp(s, "1") == 0 ||
			    strcmp(s, "0") == 0))
			return Logger::error("SceneParser", "parseSceneLine", "Ambient missing enabled boolean");
	
		bool enabled{ false };
		if (!parseBool(p, enabled)) return Logger::error("SceneParser", "parseSceneLine", "Failed to parse ambient enabled");

		Math::Vec3 colour{ 0.0f, 0.0f, 0.0f };
		if (!parseVec3f(p, colour)) return Logger::error("SceneParser", "parseSceneLine", "Failed to parse ambient colour");

		scene.ambientEnabled = enabled;
		scene.ambientLight = colour;
		return true;
	}

	return Logger::error("SceneParser", "parseSceneLine", "Failed to handle: " + std::string(nameStr));
}

}