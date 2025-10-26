#include "StarletSerializer/parser/sceneParser.hpp"

#include "StarletScene/component/model.hpp"
#include "StarletScene/component/transform.hpp"
#include "StarletScene/component/colour.hpp"

namespace Starlet::Serializer {
  bool SceneParser::parseModel(const unsigned char*& p, Scene::Model& model, Scene::TransformComponent& transform, Scene::ColourComponent& colour) {
    PARSE_OR(return false, parseBool, model.isVisible, "model enabled");
    PARSE_OR(return false, parseBool, model.isLighted, "model lighting");
    PARSE_STRING_OR(return false, p, model.name, 64, "model name");
    PARSE_STRING_OR(return false, p, model.meshPath, 128, "model path");
    PARSE_OR(return false, parseVec3f, transform.pos, "model position");
    PARSE_OR(return false, parseVec3f, transform.rot, "model rotation");
    PARSE_OR(return false, parseVec3f, transform.size, "model scale");
    if (!parseColour(p, colour.colour) && !parseSpecialColour(p, model.mode))
      return false;
    PARSE_OR(return false, parseVec4f, colour.specular, "model specular");
    return true;
  }

  bool SceneParser::parseSpecialColour(const unsigned char*& p, Scene::ColourMode& mode) {
    unsigned char input[64]{};
    if (!parseToken(p, input, sizeof(input)) || !p) return false;

    const char* name = reinterpret_cast<const char*>(input);
    if (strcmp(name, "Random") == 0)       mode = Scene::ColourMode::Random;
    else if (strcmp(name, "Rainbow") == 0) mode = Scene::ColourMode::VerticalGradient;
    else if (strcmp(name, "PLY") == 0)     mode = Scene::ColourMode::PLYColour;
    else return false;

    return true;
  }
}