#include "StarletSerializer/parser/sceneParser.hpp"

#include "StarletScene/component/grid.hpp"
#include "StarletScene/component/transform.hpp"
#include "StarletScene/component/colour.hpp"

template <GridType T>
bool SceneParser::parseGrid(const unsigned char*& p, Grid& grid, TransformComponent& transform, ColourComponent& colour) {
  grid.type = T;
  PARSE_STRING_OR(return false, p, grid.name, 64, "grid name");
  PARSE_OR(return false, parseUInt, grid.count, "grid count");
  PARSE_OR(return false, parseFloat, grid.spacing, "grid spacing");
  PARSE_OR(return false, parseVec3f, transform.pos, "grid start position");
  PARSE_OR(return false, parseVec3f, transform.rot, "grid rotation");
  PARSE_OR(return false, parseVec3f, transform.size, "grid scale");
  return parseColour(p, colour.colour);
}

bool SceneParser::parseSquareGrid(const unsigned char*& p, Grid& grid, TransformComponent& transform, ColourComponent& colour) {
  return parseGrid<GridType::Square>(p, grid, transform, colour);
}
bool SceneParser::parseCubeGrid(const unsigned char*& p, Grid& grid, TransformComponent& transform, ColourComponent& colour) {
  return parseGrid<GridType::Cube>(p, grid, transform, colour);
}
