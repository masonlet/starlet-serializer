#include "starlet-serializer/parser/scene_parser.hpp"
#include "starlet-serializer/data/camera_data.hpp"
#include "starlet-logger/logger.hpp"

namespace Starlet::Serializer {

bool SceneParser::parseCamera(const unsigned char*& p, CameraData& camera) {
  STARLET_PARSE_OR(return false, parseBool, camera.enabled, "camera enabled");
  STARLET_PARSE_STRING_OR(return false, p, camera.name, 64, "camera name");
  if (camera.name[0] >= '0' && camera.name[0] <= '9')
    return Logger::error("SceneParser", "parseCamera", "Invalid camera name: cannot start with a digit");
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