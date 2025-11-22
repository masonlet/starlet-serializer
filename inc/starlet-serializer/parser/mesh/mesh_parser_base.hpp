#pragma once

#include "starlet-serializer/parser/parser.hpp"

#include <optional>

namespace Starlet::Serializer {

struct MeshData;

class MeshParserBase : public Parser {
public:
	virtual ~MeshParserBase() = default;

	virtual bool parse(const std::string& path, MeshData& out) = 0;
};

}