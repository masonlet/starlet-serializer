#pragma once

#include <vector>

#include "model_data.hpp"
#include "light_data.hpp"
#include "camera_data.hpp"
#include "primitive_data.hpp"
#include "grid_data.hpp"
#include "texture_data.hpp"
#include "velocity_data.hpp"

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