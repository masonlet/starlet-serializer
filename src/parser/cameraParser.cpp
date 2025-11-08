#include "StarletSerializer/parser/sceneParser.hpp"
#include "StarletSerializer/data/cameraData.hpp"

namespace Starlet::Serializer {

bool SceneParser::parseCamera(const unsigned char*& p, CameraData& camera) {
  STARLET_PARSE_OR(return false, parseBool, camera.enabled, "camera enabled");
  STARLET_PARSE_STRING_OR(return false, p, camera.name, 64, "camera name");
  STARLET_PARSE_OR(return false, parseVec3f, camera.transform.pos, "camera position");
  STARLET_PARSE_OR(return false, parseFloat, camera.transform.rot.y, "camera yaw");
  STARLET_PARSE_OR(return false, parseFloat, camera.transform.rot.x, "camera pitch");
  STARLET_PARSE_OR(return false, parseFloat, camera.fov, "camera fov");
  STARLET_PARSE_OR(return false, parseFloat, camera.nearPlane, "camera near plane");
  STARLET_PARSE_OR(return false, parseFloat, camera.farPlane, "camera far plane");
  STARLET_PARSE_OR(return false, parseFloat, camera.moveSpeed, "camera speed");
  return true;
}

}