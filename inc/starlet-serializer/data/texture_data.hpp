#pragma once

#include "base_data.hpp"

#include "starlet-math/vec2.hpp"

namespace Starlet::Serializer {

struct TextureData : public NamedData {
	std::string faces[6];
	float mix{ 1.0f };
	Math::Vec2<float> tiling{ 1.0f, 1.0f };
	bool isCube{ false };
};

struct TextureConnection {
	std::string modelName, textureName;
	unsigned int slot{ 0 };
	float mix{ 1.0f };
};

}