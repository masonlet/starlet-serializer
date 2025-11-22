#pragma once

#include "colour_data.hpp"
#include "transform_data.hpp"

namespace Starlet::Serializer {

struct NamedData {
	std::string name;
};
struct TransformedData : public NamedData {
	TransformData transform;
};
struct ColouredTransformedData : public TransformedData {
	ColourData colour;
};

}