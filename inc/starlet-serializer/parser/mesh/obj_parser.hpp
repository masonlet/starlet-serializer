#pragma once

#include "mesh_parser_base.hpp"

namespace Starlet::Serializer {

struct MeshData;

class ObjParser : public MeshParserBase {
public:
	bool parse(const std::string& path, MeshData& out);

private:
	
};

}