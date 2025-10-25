#pragma once

#include "StarletScene/scene.hpp"

#include <string>
#include <fstream>

class Scene;
struct Model;

struct TransformComponent;
struct ColourComponent;

class Writer {
public:
	bool writeScene(const Scene& scene);

private:
	bool writeCameras(std::ostream& file, const Scene& scene);
	bool writeModels(std::ostream& file, const Scene& scene);
	bool writeLights(std::ostream& file, const Scene& scene);

	bool writeTransform(std::ostream& file, const TransformComponent& transform, bool includeSize = true);
	bool writeColourMode(std::ostream& file, const Model& model, const ColourComponent& colour);
};