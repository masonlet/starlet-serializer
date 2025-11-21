#include "starlet-serializer/parser/sceneParser.hpp"
#include "starlet-serializer/data/lightData.hpp"

#include "starlet-logger/logger.hpp"

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
		default: return Logger::error("SceneParser", "parseLightType", "Unknown light type");
		}

		return true;
	}

	p = original;
	unsigned char typeName[64]{};
	if (!parseToken(p, typeName, sizeof(typeName)) || !p) {
		p = original;
		return false;
	}
		 
	if      (strcmp((char*)typeName, "Point") == 0) type = LightType::Point;
	else if (strcmp((char*)typeName, "Spot") == 0) type = LightType::Spot;
	else if (strcmp((char*)typeName, "Directional") == 0) type = LightType::Directional;
	else return Logger::error("Parser", "parseLightType", "Unknown light type");
	return true;
}

bool SceneParser::parseLight(const unsigned char*& p, LightData& light) {
	STARLET_PARSE_OR(return false, parseBool, light.enabled, "light enabled");
	STARLET_PARSE_STRING_OR(return false, p, light.name, 64, "light name");
	STARLET_PARSE_OR(return false, parseLightType, light.type, "light type");
	STARLET_PARSE_OR(return false, parseVec3f, light.transform.pos, "light position");
	STARLET_PARSE_OR(return false, parseVec3f, light.transform.rot, "light direction");
	STARLET_PARSE_OR(return false, parseColour, light.colour.colour, "light diffuse");
	STARLET_PARSE_OR(return false, parseVec4f, light.attenuation, "light attenuation");
	STARLET_PARSE_OR(return false, parseVec2f, light.param1, "light param1");
	return true;
}

}