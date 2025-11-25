#include "starlet-serializer/parser/scene_parser.hpp"
#include "starlet-serializer/data/velocity_data.hpp"
#include "starlet-logger/logger.hpp"

namespace Starlet::Serializer {

bool SceneParser::parseVelocity(const unsigned char*& p, VelocityData& velocity) {
	STARLET_PARSE_STRING_OR(return false, p, velocity.modelName, 128, "velocity model name");
	if (velocity.modelName[0] >= '0' && velocity.modelName[0] <= '9')
		return Logger::error("SceneParser", "parseVelocity", "Invalid model name: cannot start with a digit");
	STARLET_PARSE_OR(return false, parseVec3f, velocity.velocity, "velocity vec3");
	return true;
}

}