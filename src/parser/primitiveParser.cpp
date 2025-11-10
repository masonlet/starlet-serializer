#include "starlet-serializer/parser/sceneParser.hpp"
#include "starlet-serializer/data/primitiveData.hpp"

namespace Starlet::Serializer {

template<PrimitiveType T>
bool SceneParser::parsePrimitive(const unsigned char*& p, PrimitiveData& out) {
  out.type = T;
  STARLET_PARSE_STRING_OR(return false, p, out.name, 64, "primitive name");
  STARLET_PARSE_OR(return false, parseVec3f, out.transform.pos, "primitive position");
  STARLET_PARSE_OR(return false, parseVec3f, out.transform.rot, "primitive rotation");
  STARLET_PARSE_OR(return false, parseVec3f, out.transform.size, "triangle size");
  return parseColour(p, out.colour.colour);
}

bool SceneParser::parseTriangle(const unsigned char*& p, PrimitiveData& out) {
  return parsePrimitive<PrimitiveType::Triangle>(p, out);
}
bool SceneParser::parseSquare(const unsigned char*& p, PrimitiveData& out) {
  return parsePrimitive<PrimitiveType::Square>(p, out);
}
bool SceneParser::parseCube(const unsigned char*& p, PrimitiveData& out) {
  return parsePrimitive<PrimitiveType::Cube>(p, out);
}

}