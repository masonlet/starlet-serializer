#pragma once

#include "data.hpp"

namespace Starlet::Serializer {

enum class PrimitiveType {
	Triangle,
	Square,
	Cube
};

struct PrimitiveData : public ColouredTransformedData {
	PrimitiveType type;
};

}