#include "StarletSerializer/parser/sceneParser.hpp"
#include "StarletSerializer/data/cameraData.hpp"

namespace Starlet::Serializer {
  bool SceneParser::parseCamera(const unsigned char*& p, CameraData& camera) {
    PARSE_OR(return false, parseBool, camera.enabled, "camera enabled");
    PARSE_STRING_OR(return false, p, camera.name, 64, "camera name");
    PARSE_OR(return false, parseVec3f, camera.transform.pos, "camera position");
    PARSE_OR(return false, parseFloat, camera.transform.rot.y, "camera yaw");
    PARSE_OR(return false, parseFloat, camera.transform.rot.x, "camera pitch");
    PARSE_OR(return false, parseFloat, camera.fov, "camera fov");
    PARSE_OR(return false, parseFloat, camera.nearPlane, "camera near plane");
    PARSE_OR(return false, parseFloat, camera.farPlane, "camera far plane");
    PARSE_OR(return false, parseFloat, camera.moveSpeed, "camera speed");
    return true;
  }
}