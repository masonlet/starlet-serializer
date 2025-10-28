#include "StarletSerializer/parser/sceneParser.hpp"
#include "StarletSerializer/utils/log.hpp"

#include "StarletSerializer/data/lightData.hpp"

namespace Starlet::Serializer {
	bool SceneParser::parseLightType(const unsigned char*& p, LightType& type) {
		p = skipWhitespace(p);
		if (!p || *p == '\0') return false;

		const unsigned char* original = p;

		unsigned int lightType;
		if (parseUInt(p, lightType)) {
			switch (lightType) {
			case 0: type = LightType::Point; break;
			case 1: type = LightType::Spot; break;
			case 2: type = LightType::Directional; break;
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

		if (strcmp((char*)typeName, "Point") == 0) type = LightType::Point;
		else if (strcmp((char*)typeName, "Spot") == 0) type = LightType::Spot;
		else if (strcmp((char*)typeName, "Directional") == 0) type = LightType::Directional;
		else return error("Parser", "parseLightType", "Unknown light type");
		return true;
	}

	bool SceneParser::parseLight(const unsigned char*& p, LightData& light) {
		PARSE_OR(return false, parseBool, light.enabled, "light enabled");
		PARSE_STRING_OR(return false, p, light.name, 64, "light name");
		PARSE_OR(return false, parseLightType, light.type, "light type");
		PARSE_OR(return false, parseVec3f, light.transform.pos, "light position");
		PARSE_OR(return false, parseVec3f, light.transform.rot, "light direction");
		PARSE_OR(return false, parseVec4f, light.colour.colour, "light diffuse");
		PARSE_OR(return false, parseVec4f, light.attenuation, "light attenuation");
		PARSE_OR(return false, parseVec2f, light.param1, "light param1");
		return true;
	}
}