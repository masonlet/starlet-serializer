#pragma once

#include "StarletMath/vec4.hpp"

namespace Starlet::Serializer {
	struct ColourData {
		Math::Vec4<float> colour{ 1.0f };
		Math::Vec4<float> specular{ 1.0f, 1.0f, 1.0f, 32.0f };
	};
}