#include "starlet-serializer/parser/sceneParser.hpp"
#include "starlet-serializer/data/velocityData.hpp"

namespace Starlet::Serializer {

bool SceneParser::parseVelocity(const unsigned char*& p, VelocityData& velocity) {
	STARLET_PARSE_OR(return false, parseVec3f, velocity.velocity, "velocity");
	return true;
}

}