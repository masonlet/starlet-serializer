#include "StarletSerializer/parser/sceneParser.hpp"

namespace Starlet::Serializer {
  template <GridType T>
  bool SceneParser::parseGrid(const unsigned char*& p, GridData& grid) {
    grid.type = T;
    PARSE_STRING_OR(return false, p, grid.name, 64, "grid name");
    PARSE_OR(return false, parseUInt, grid.count, "grid count");
    PARSE_OR(return false, parseFloat, grid.spacing, "grid spacing");
    PARSE_OR(return false, parseVec3f, grid.transform.pos, "grid start position");
    PARSE_OR(return false, parseVec3f, grid.transform.rot, "grid rotation");
    PARSE_OR(return false, parseVec3f, grid.transform.size, "grid scale");
    return parseColour(p, grid.colour.colour);
  }

  bool SceneParser::parseSquareGrid(const unsigned char*& p, GridData& grid) {
    return parseGrid<GridType::Square>(p, grid);
  }
  bool SceneParser::parseCubeGrid(const unsigned char*& p, GridData& grid) {
    return parseGrid<GridType::Cube>(p, grid);
  }
}