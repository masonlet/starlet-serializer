#pragma once

#include "data.hpp"

#include "StarletMath/vec3.hpp"

namespace Starlet::Serializer {

struct VelocityData {
	std::string modelName;
	Math::Vec3<float> velocity{ 1.0f };
};

}