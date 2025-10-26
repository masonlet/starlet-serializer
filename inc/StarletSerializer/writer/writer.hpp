#pragma once

#include "StarletScene/scene.hpp"

#include <string>
#include <fstream>

namespace Starlet {
	namespace Scene {
		struct Model;

		struct TransformComponent;
		struct ColourComponent;
	}

	namespace Serializer {
		class Writer {
		public:
			bool writeScene(const Scene::Scene& scene);

		private:
			bool writeCameras(std::ostream& file, const Scene::Scene& scene);
			bool writeModels(std::ostream& file, const Scene::Scene& scene);
			bool writeLights(std::ostream& file, const Scene::Scene& scene);

			bool writeTransform(std::ostream& file, const Scene::TransformComponent& transform, bool includeSize = true);
			bool writeColourMode(std::ostream& file, const Scene::Model& model, const Scene::ColourComponent& colour);
		};
	}
}