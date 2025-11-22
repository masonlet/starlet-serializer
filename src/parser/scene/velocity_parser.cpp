#include "starlet-serializer/parser/scene_parser.hpp"
#include "starlet-serializer/data/velocity_data.hpp"

namespace Starlet::Serializer {

bool SceneParser::parseVelocity(const unsigned char*& p, VelocityData& velocity) {
	STARLET_PARSE_OR(return false, parseVec3f, velocity.velocity, "velocity");
	return true;
}

}