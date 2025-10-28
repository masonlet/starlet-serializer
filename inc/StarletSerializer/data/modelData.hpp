#pragma once

#include "data.hpp"

#include "StarletMath/vec2.hpp"

namespace Starlet::Serializer {
	enum class ColourMode {
		Solid,
		Random,
		VerticalGradient,
		PLYColour
	};

	struct ModelData : public ColouredTransformedData {
		std::string meshPath;

		bool isVisible{ true }, isLighted{ true }, useTextures{ false };
		ColourMode mode{ ColourMode::Solid };

		static const unsigned int NUM_TEXTURES = 4;
		std::string textureNames[NUM_TEXTURES]{ "" };
		float textureMixRatio[NUM_TEXTURES]{ 1.0f, 0.0f, 0.0f, 0.0f };
		Math::Vec2<float> textureTiling{ 1.0f, 1.0f };
	};
}