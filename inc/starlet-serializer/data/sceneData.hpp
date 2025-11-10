#pragma once

#include <vector>

#include "modelData.hpp"
#include "lightData.hpp"
#include "cameraData.hpp"
#include "primitiveData.hpp"
#include "gridData.hpp"
#include "textureData.hpp"
#include "velocityData.hpp"

#include "starlet-math/vec3.hpp"

namespace Starlet::Serializer {

struct SceneData {
	std::vector<ModelData> models;
	std::vector<LightData> lights;
	std::vector<CameraData> cameras;
	std::vector<PrimitiveData> primitives;
	std::vector<GridData> grids;
	std::vector<TextureData> textures;
	std::vector<TextureConnection> textureConnections;
	std::vector<VelocityData> velocities;

	bool ambientEnabled{ false };
	Math::Vec3<float> ambientLight{ 0.0f, 0.0f, 0.0f };
};

}