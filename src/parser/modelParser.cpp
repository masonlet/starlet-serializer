#include "StarletSerializer/parser/sceneParser.hpp"
#include "StarletSerializer/data/modelData.hpp"

namespace Starlet::Serializer {

bool SceneParser::parseModel(const unsigned char*& p, ModelData& model) {
  STARLET_PARSE_OR(return false, parseBool, model.isVisible, "model enabled");
  STARLET_PARSE_OR(return false, parseBool, model.isLighted, "model lighting");
  STARLET_PARSE_STRING_OR(return false, p, model.name, 64, "model name");
  STARLET_PARSE_STRING_OR(return false, p, model.meshPath, 128, "model path");
  STARLET_PARSE_OR(return false, parseVec3f, model.transform.pos, "model position");
  STARLET_PARSE_OR(return false, parseVec3f, model.transform.rot, "model rotation");
  STARLET_PARSE_OR(return false, parseVec3f, model.transform.size, "model scale");
  if (!parseColour(p, model.colour.colour) && !parseSpecialColour(p, model.mode))
    return false;
  STARLET_PARSE_OR(return false, parseVec4f, model.colour.specular, "model specular");
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