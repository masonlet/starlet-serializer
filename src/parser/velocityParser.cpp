#include "StarletSerializer/parser/sceneParser.hpp"
#include "StarletSerializer/utils/log.hpp"

namespace Starlet::Serializer {
	bool SceneParser::parseVelocity(const unsigned char*& p, VelocityData& velocity) {
		PARSE_OR(return false, parseVec3f, velocity.velocity, "velocity");
		return true;
	}
}