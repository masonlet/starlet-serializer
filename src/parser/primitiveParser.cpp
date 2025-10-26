#include "StarletSerializer/parser/sceneParser.hpp"

#include "StarletScene/component/primitive.hpp"
#include "StarletScene/component/transform.hpp"
#include "StarletScene/component/colour.hpp"

namespace Starlet::Serializer {
  template<Scene::PrimitiveType T>
  bool SceneParser::parsePrimitive(const unsigned char*& p, Scene::Primitive& out, Scene::TransformComponent& transform, Scene::ColourComponent& colour) {
    out.type = T;
    PARSE_STRING_OR(return false, p, out.name, 64, "primitive name");
    PARSE_OR(return false, parseVec3f, transform.pos, "primitive position");
    PARSE_OR(return false, parseVec3f, transform.rot, "primitive rotation");
    PARSE_OR(return false, parseVec3f, transform.size, "triangle size");
    return parseColour(p, colour.colour);
  }

  bool SceneParser::parseTriangle(const unsigned char*& p, Scene::Primitive& out, Scene::TransformComponent& transform, Scene::ColourComponent& colour) {
    return parsePrimitive<Scene::PrimitiveType::Triangle>(p, out, transform, colour);
  }
  bool SceneParser::parseSquare(const unsigned char*& p, Scene::Primitive& out, Scene::TransformComponent& transform, Scene::ColourComponent& colour) {
    return parsePrimitive<Scene::PrimitiveType::Square>(p, out, transform, colour);
  }
  bool SceneParser::parseCube(const unsigned char*& p, Scene::Primitive& out, Scene::TransformComponent& transform, Scene::ColourComponent& colour) {
    return parsePrimitive<Scene::PrimitiveType::Cube>(p, out, transform, colour);
  }
}