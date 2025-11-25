#include "starlet-serializer/parser/scene_parser.hpp"
#include "starlet-serializer/data/grid_data.hpp"
#include "starlet-logger/logger.hpp"

namespace Starlet::Serializer {

template <GridType T>
bool SceneParser::parseGrid(const unsigned char*& p, GridData& grid) {
  grid.type = T;
  STARLET_PARSE_STRING_OR(return false, p, grid.name, 64, "grid name");
  if (grid.name[0] >= '0' && grid.name[0] <= '9')
    return Logger::error("SceneParser", "parseGrid", "Invalid grid name: cannot start with a digit");
  STARLET_PARSE_OR(return false, parseUInt, grid.count, "grid count");
  STARLET_PARSE_OR(return false, parseFloat, grid.spacing, "grid spacing");
  STARLET_PARSE_OR(return false, parseVec3f, grid.transform.pos, "grid start position");
  STARLET_PARSE_OR(return false, parseVec3f, grid.transform.rot, "grid rotation");
  STARLET_PARSE_OR(return false, parseVec3f, grid.transform.size, "grid scale");
  STARLET_PARSE_OR(return false, parseColour, grid.colour.colour, "grid colour");
  return true;
}

bool SceneParser::parseSquareGrid(const unsigned char*& p, GridData& grid) {
  return parseGrid<GridType::Square>(p, grid);
}
bool SceneParser::parseCubeGrid(const unsigned char*& p, GridData& grid) {
  return parseGrid<GridType::Cube>(p, grid);
}

}