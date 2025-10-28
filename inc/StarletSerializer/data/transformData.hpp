#pragma once

#include "StarletMath/vec3.hpp"

namespace Starlet::Serializer {
	struct TransformData {
		Math::Vec3<float> pos{ 0.0f };
		Math::Vec3<float> rot{ 0.0f };
		Math::Vec3<float> size{ 1.0f };
	};
}