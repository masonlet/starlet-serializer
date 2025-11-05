#pragma once

#include "data.hpp"

#include "StarletMath/vec2.hpp"
#include "StarletMath/vec4.hpp"

namespace Starlet::Serializer {

enum class LightType {
	Point,
	Spot,
	Directional
};

struct LightData : public ColouredTransformedData {
	bool enabled{ true };
	LightType type{ LightType::Point };
	Math::Vec4<float> attenuation{ 0.0f, 0.01f, 0.01f, 1.0f };
	Math::Vec2<float> param1{ 0.0f };
};

}