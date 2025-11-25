#include "starlet-serializer/parser/scene_parser.hpp"
#include "starlet-serializer/data/texture_data.hpp"
#include "starlet-logger/logger.hpp"

namespace Starlet::Serializer {

bool SceneParser::parseTexture(const unsigned char*& p, TextureData& out) {
  STARLET_PARSE_STRING_OR(return false, p, out.name, 128, "texture name");
  if (out.name[0] >= '0' && out.name[0] <= '9') 
    return Logger::error("SceneParser", "parseTexture", "Invalid texture name: cannot start with a digit");
  if (strstr(out.name.c_str(), ".")) 
    return Logger::error("SceneParser", "parseTexture", "Invalid texture name: appears to be a file path");
  
  STARLET_PARSE_STRING_OR(return false, p, out.faces[0], 256, "texture file");
  STARLET_PARSE_OR(return false, parseFloat, out.mix, "texture mix");
  STARLET_PARSE_OR(return false, parseVec2f, out.tiling, "texture tiling");
  out.isCube = false;
  return true;
}

bool SceneParser::parseCubeTexture(const unsigned char*& p, TextureData& out) {
  STARLET_PARSE_STRING_OR(return false, p, out.name, 128, "cube map name");
  if (out.name[0] >= '0' && out.name[0] <= '9') 
    return Logger::error("SceneParser", "parseCubeTexture", "Invalid texture name: cannot start with a digit");
    
  if (strstr(out.name.c_str(), ".")) 
    return Logger::error("SceneParser", "parseCubeTexture", "Invalid texture name: appears to be a file path");
 
  for (int i = 0; i < 6; ++i) {
    STARLET_PARSE_STRING_OR(return false, p, out.faces[i], 256, "cube map face");
    if (!strstr(out.faces[i].c_str(), "."))
      return Logger::error("SceneParser", "parseCubeTexture", "Invalid cube map face: missing file extension");
  }
  STARLET_PARSE_OR(return false, parseFloat, out.mix, "cube map mix");
  STARLET_PARSE_OR(return false, parseVec2f, out.tiling, "cube map tiling");
  out.isCube = true;
  return true;
}

}