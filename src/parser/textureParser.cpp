#include "StarletSerializer/parser/sceneParser.hpp"

#include "StarletSerializer/data/textureData.hpp"

namespace Starlet::Serializer {
  bool SceneParser::parseTexture(const unsigned char*& p, TextureData& out) {
    PARSE_STRING_OR(return false, p, out.name, 128, "texture name");
    PARSE_STRING_OR(return false, p, out.faces[0], 256, "texture file");
    PARSE_OR(return false, parseFloat, out.mix, "texture mix");
    PARSE_OR(return false, parseVec2f, out.tiling, "texture tiling");
    out.isCube = false;
    return true;
  }
  bool SceneParser::parseCubeTexture(const unsigned char*& p, TextureData& out) {
    PARSE_STRING_OR(return false, p, out.name, 128, "cube map name");
    for (int i = 0; i < 6; ++i) PARSE_STRING_OR(return false, p, out.faces[i], 256, "cube map face");
    PARSE_OR(return false, parseFloat, out.mix, "cube map mix");
    PARSE_OR(return false, parseVec2f, out.tiling, "cube map tiling");
    out.isCube = true;
    return true;
  }
}