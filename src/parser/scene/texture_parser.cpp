#include "starlet-serializer/parser/scene_parser.hpp"
#include "starlet-serializer/data/texture_data.hpp"

namespace Starlet::Serializer {

bool SceneParser::parseTexture(const unsigned char*& p, TextureData& out) {
  STARLET_PARSE_STRING_OR(return false, p, out.name, 128, "texture name");
  STARLET_PARSE_STRING_OR(return false, p, out.faces[0], 256, "texture file");
  STARLET_PARSE_OR(return false, parseFloat, out.mix, "texture mix");
  STARLET_PARSE_OR(return false, parseVec2f, out.tiling, "texture tiling");
  out.isCube = false;
  return true;
}

bool SceneParser::parseCubeTexture(const unsigned char*& p, TextureData& out) {
  STARLET_PARSE_STRING_OR(return false, p, out.name, 128, "cube map name");
  for (int i = 0; i < 6; ++i) 
    STARLET_PARSE_STRING_OR(return false, p, out.faces[i], 256, "cube map face");
  STARLET_PARSE_OR(return false, parseFloat, out.mix, "cube map mix");
  STARLET_PARSE_OR(return false, parseVec2f, out.tiling, "cube map tiling");
  out.isCube = true;
  return true;
}

}