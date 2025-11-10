#include "starlet-serializer/parser/sceneParser.hpp"

#include "starlet-serializer/data/gridData.hpp"

namespace Starlet::Serializer {

template <GridType T>
bool SceneParser::parseGrid(const unsigned char*& p, GridData& grid) {
  grid.type = T;
  STARLET_PARSE_STRING_OR(return false, p, grid.name, 64, "grid name");
  STARLET_PARSE_OR(return false, parseUInt, grid.count, "grid count");
  STARLET_PARSE_OR(return false, parseFloat, grid.spacing, "grid spacing");
  STARLET_PARSE_OR(return false, parseVec3f, grid.transform.pos, "grid start position");
  STARLET_PARSE_OR(return false, parseVec3f, grid.transform.rot, "grid rotation");
  STARLET_PARSE_OR(return false, parseVec3f, grid.transform.size, "grid scale");
  return parseColour(p, grid.colour.colour);
}

bool SceneParser::parseSquareGrid(const unsigned char*& p, GridData& grid) {
  return parseGrid<GridType::Square>(p, grid);
}
bool SceneParser::parseCubeGrid(const unsigned char*& p, GridData& grid) {
  return parseGrid<GridType::Cube>(p, grid);
}

}