#include "StarletSerializer/parser/sceneParser.hpp"
#include "StarletSerializer/utils/log.hpp"

#include "StarletScene/component/light.hpp"
#include "StarletScene/component/transform.hpp"
#include "StarletScene/component/colour.hpp"

namespace Starlet::Serializer {
	bool SceneParser::parseLightType(const unsigned char*& p, Scene::LightType& type) {
		p = skipWhitespace(p);
		if (!p || *p == '\0') return false;

		const unsigned char* original = p;

		unsigned int lightType;
		if (parseUInt(p, lightType)) {
			switch (lightType) {
			case 0: type = Scene::LightType::Point; break;
			case 1: type = Scene::LightType::Spot; break;
			case 2: type = Scene::LightType::Directional; break;
			default: return error("Parser", "parseLightType", "Unknown light type");
			}

			return true;
		}

		p = original;
		unsigned char typeName[64]{};
		if (!parseToken(p, typeName, sizeof(typeName)) || !p) {
			p = original;
			return false;
		}

		if (strcmp((char*)typeName, "Point") == 0) type = Scene::LightType::Point;
		else if (strcmp((char*)typeName, "Spot") == 0) type = Scene::LightType::Spot;
		else if (strcmp((char*)typeName, "Directional") == 0) type = Scene::LightType::Directional;
		else return error("Parser", "parseLightType", "Unknown light type");
		return true;
	}

	bool SceneParser::parseLight(const unsigned char*& p, Scene::Light& light, Scene::TransformComponent& transform, Scene::ColourComponent& colour) {
		PARSE_OR(return false, parseBool, light.enabled, "light enabled");
		PARSE_STRING_OR(return false, p, light.name, 64, "light name");
		PARSE_OR(return false, parseLightType, light.type, "light type");
		PARSE_OR(return false, parseVec3f, transform.pos, "light position");
		PARSE_OR(return false, parseVec3f, transform.rot, "light direction");
		PARSE_OR(return false, parseVec4f, colour.colour, "light diffuse");
		PARSE_OR(return false, parseVec4f, light.attenuation, "light attenuation");
		PARSE_OR(return false, parseVec2f, light.param1, "light param1");
		return true;
	}
}