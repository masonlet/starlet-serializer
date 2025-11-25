#include "starlet-serializer/parser/scene_parser.hpp"
#include "starlet-serializer/data/primitive_data.hpp"
#include "starlet-logger/logger.hpp"

namespace Starlet::Serializer {

template<PrimitiveType T>
bool SceneParser::parsePrimitive(const unsigned char*& p, PrimitiveData& out) {
  out.type = T;
  STARLET_PARSE_STRING_OR(return false, p, out.name, 64, "primitive name");
  if (out.name[0] >= '0' && out.name[0] <= '9')
    return Logger::error("SceneParser", "parsePrimitive", "Invalid primitive name: cannot start with a digit");
  STARLET_PARSE_OR(return false, parseVec3f, out.transform.pos, "primitive position");
  STARLET_PARSE_OR(return false, parseVec3f, out.transform.rot, "primitive rotation");
  STARLET_PARSE_OR(return false, parseVec3f, out.transform.size, "primitive size");
  STARLET_PARSE_OR(return false, parseColour, out.colour.colour, "primitive colour");
  return true;
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