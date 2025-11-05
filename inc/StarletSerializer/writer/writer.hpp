#pragma once

#include <string>
#include <vector>
#include <fstream>

#ifndef WRITE_OR 
#define WRITE_OR(onFail, writer, errorMsg) \
do { \
  if (!(writer(file, data))) { \
      if((errorMsg) && *(errorMsg) != '\0') fprintf(stderr, "[Writer ERROR]: Failed to write %s\n", errorMsg); \
      onFail; \
	} \
} while(0)
#endif

namespace Starlet::Serializer {

struct SceneData;
struct TransformData;
struct ModelData;

class Writer {
public:
	bool writeScene(const SceneData& data, const std::string& path);

private:
	bool writeCameras(std::ostream& file, const SceneData& data);
	bool writeModels(std::ostream& file, const SceneData& data);
	bool writeLights(std::ostream& file, const SceneData& data);
	bool writePrimitives(std::ostream& file, const SceneData& data);
	bool writeTextures(std::ostream& file, const SceneData& data);
	bool writeTextureConnections(std::ostream& file, const SceneData& data);
	bool writeGrids(std::ostream& file, const SceneData& data);
	bool writeVelocities(std::ostream& file, const SceneData& data);
	bool writeAmbient(std::ostream& file, const SceneData& data);

	bool writeTransform(std::ostream& file, const TransformData& transform, bool includeSize = true);
	bool writeColourMode(std::ostream& file, const ModelData& model);
};

}