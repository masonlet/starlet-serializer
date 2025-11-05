#pragma once

#include "data.hpp"

namespace Starlet::Serializer {

struct CameraData : public TransformedData {
	float moveSpeed{ 25.0f }, mouseSpeed{ 0.1f };
	float fov{ 60.0f };
	float nearPlane{ 0.1f }, farPlane{ 10000.0f };
	bool enabled{ true }, paused{ false };
};

}