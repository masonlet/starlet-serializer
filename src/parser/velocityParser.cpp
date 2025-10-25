#include "StarletSerializer/parser/sceneParser.hpp"
#include "StarletSerializer/utils/log.hpp"

#include "StarletScene/component/velocity.hpp"

bool SceneParser::parseVelocity(const unsigned char*& p, VelocityComponent& velocity) {
	PARSE_OR(return false, parseVec3f, velocity.velocity, "velocity");
	return true;
}
