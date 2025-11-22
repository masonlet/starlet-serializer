#pragma once

#include "parser.hpp"

namespace Starlet::Serializer {

struct MeshData;

class MeshParser : public Parser {
public:
	bool parse(const std::string& path, MeshData& out);

private:
	enum class MeshFormat {
		PLY,
		UNKNOWN
	};

	MeshFormat detectFormat(const std::string& path);
};

}