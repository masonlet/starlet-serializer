#pragma once

#include "data.hpp"

namespace Starlet::Serializer {

enum class GridType {
	Square,
	Cube,
	Model
};

struct GridData : public ColouredTransformedData {
	GridType type{ GridType::Cube };
	unsigned int count{ 1 };
	float spacing{ 1.0f };
};

}