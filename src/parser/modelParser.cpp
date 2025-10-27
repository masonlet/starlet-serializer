#include "StarletSerializer/parser/sceneParser.hpp"

namespace Starlet::Serializer {
  bool SceneParser::parseModel(const unsigned char*& p, ModelData& model) {
    PARSE_OR(return false, parseBool, model.isVisible, "model enabled");
    PARSE_OR(return false, parseBool, model.isLighted, "model lighting");
    PARSE_STRING_OR(return false, p, model.name, 64, "model name");
    PARSE_STRING_OR(return false, p, model.meshPath, 128, "model path");
    PARSE_OR(return false, parseVec3f, model.transform.pos, "model position");
    PARSE_OR(return false, parseVec3f, model.transform.rot, "model rotation");
    PARSE_OR(return false, parseVec3f, model.transform.size, "model scale");
    if (!parseColour(p, model.colour.colour) && !parseSpecialColour(p, model.mode))
      return false;
    PARSE_OR(return false, parseVec4f, model.colour.specular, "model specular");
    return true;
  }

  bool SceneParser::parseSpecialColour(const unsigned char*& p, ColourMode& mode) {
    unsigned char input[64]{};
    if (!parseToken(p, input, sizeof(input)) || !p) return false;

    const char* name = reinterpret_cast<const char*>(input);
    if (strcmp(name, "Random") == 0)       mode = ColourMode::Random;
    else if (strcmp(name, "Rainbow") == 0) mode = ColourMode::VerticalGradient;
    else if (strcmp(name, "PLY") == 0)     mode = ColourMode::PLYColour;
    else return false;

    return true;
  }
}