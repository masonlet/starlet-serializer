#pragma once

#include "colourData.hpp"
#include "transformData.hpp"

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