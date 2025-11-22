#pragma once

#include "base_data.hpp"

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