#include "starlet-serializer/parser/sceneParser.hpp"

#include "starlet-math/vec4.hpp"

namespace Starlet::Serializer {

bool SceneParser::parseColour(const unsigned char*& p, Math::Vec4<float>& colourOut) {
	const unsigned char* original = p;
	if (parseNumericColour(p, colourOut)) return true;
	p = original;
	if (parseNamedColour(p, colourOut)) return true;
	p = original;
	return false;
}

bool SceneParser::parseNumericColour(const unsigned char*& p, Math::Vec4<float>& out) {
	const unsigned char* original = p;
	if (parseVec4f(p, out)) {
		if (out.x > 1.0f) out.x /= 255.0f;
		if (out.y > 1.0f) out.y /= 255.0f;
		if (out.z > 1.0f) out.z /= 255.0f;
		if (out.w > 1.0f) out.w /= 255.0f;
		return true;
	}

	p = original;
	Math::Vec3<float> rgb;
	if (parseVec3f(p, rgb)) {
		out = { rgb.x, rgb.y, rgb.z, 1.0f };
		if (out.x > 1) out.x /= 255.f;
		if (out.y > 1) out.y /= 255.f;
		if (out.z > 1) out.z /= 255.f;
		return true;
	}

	p = original;
	return false;
}

bool SceneParser::parseNamedColour(const unsigned char*& p, Math::Vec4<float>& colour) {
	unsigned char input[64]{};
	if (!parseToken(p, input, sizeof(input)) || !p) return false;

	const char* name = reinterpret_cast<const char*>(input);
	if (strcmp(name, "Red") == 0)     colour = { 1.0f, 0.0f, 0.0f, 1.0f };
	else if (strcmp(name, "Green") == 0)   colour = { 0.0f, 1.0f, 0.0f, 1.0f };
	else if (strcmp(name, "Blue") == 0)    colour = { 0.0f, 0.0f, 1.0f, 1.0f };
	else if (strcmp(name, "Yellow") == 0)  colour = { 1.0f, 1.0f, 0.0f, 1.0f };
	else if (strcmp(name, "White") == 0)   colour = { 1.0f, 1.0f, 1.0f, 1.0f };
	else if (strcmp(name, "Gray") == 0
		|| strcmp(name, "Grey") == 0)    colour = { 0.5f, 0.5f, 0.5f, 1.0f };

	else return false;
	return true;
}

}