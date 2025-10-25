#include "StarletSerializer/writer/writer.hpp"

#include "StarletScene/component/model.hpp"
#include "StarletScene/component/light.hpp"
#include "StarletScene/component/camera.hpp"
#include "StarletScene/component/grid.hpp"
#include "StarletScene/component/textureData.hpp"
#include "StarletScene/component/primitive.hpp"
#include "StarletScene/component/transform.hpp"
#include "StarletScene/component/velocity.hpp"
#include "StarletScene/component/colour.hpp"

#include <iomanip>

bool Writer::writeScene(const Scene& scene) {
  std::ofstream file(scene.getScenePath());
  if (!file.is_open()) return error("Writer", "writeScene", "Failed to open file for saving: " + scene.getScenePath());

  file << std::fixed << std::setprecision(3);

  if (!writeCameras(file, scene))
    return error("Writer", "writeScene", "Failed to write cameras");

  if(!writeModels(file, scene))
    return error("Writer", "writeScene", "Failed to write models");

  if (!writeLights(file, scene))
    return error("Writer", "writeScene", "Failed to write lights");

  return true;
}

bool Writer::writeCameras(std::ostream& file, const Scene& scene) {
  file << "comment, name, pos(xyz), rot(yaw pitch), fov, nearPlane farPlane, camSpeed\n";
  for (const auto& [entity, cam] : scene.getEntitiesOfType<Camera>()) {
    if (!scene.hasComponent<TransformComponent>(entity)) continue;
    const TransformComponent& transform = scene.getComponent<TransformComponent>(entity);

    file << "camera, " << cam->name << ", ";
    if (!writeTransform(file, transform, false)) return false;
    file << cam->fov << ", "
         << cam->nearPlane << " " << cam->farPlane << ", "
         << cam->moveSpeed << "\n";
  }

  return !file.fail();
}
bool Writer::writeModels(std::ostream& file, const Scene& scene) {
  file << "\ncomment, name, meshPath, pos(xyz), rot(xyz), scale(xyz), colour(Int, Named Colour, Random, Rainbow, PLY), specular(rgb, power)\n";
  for (const auto& [entity, model] : scene.getEntitiesOfType<Model>()) {
    if (model->name.rfind("triangle_instance", 0) == 0 || model->name.rfind("cube_instance_", 0) == 0 || model->name.rfind("square_instance_", 0) == 0)
      continue;

    file << "model, "
      << model->name << ", "
      << model->meshPath << ", ";

    if (scene.hasComponent<TransformComponent>(entity)) {
      const TransformComponent& transform = scene.getComponent<TransformComponent>(entity);
      if (!writeTransform(file, transform)) return false;
    }

    ColourComponent colour{};
    if (scene.hasComponent<ColourComponent>(entity))
      colour = scene.getComponent<ColourComponent>(entity);

    writeColourMode(file, *model, colour);
    file << ", " << colour.specular << "\n";
  }

  return !file.fail();
}
bool Writer::writeLights(std::ostream& file, const Scene& scene) {
  file << "\ncomment, name, type, pos (xyz), diffuse (rgba), attention (xyzw), direction, param1 (spotlight inner, spotlight outer), param2 (on/off)\n";
  for (const auto& [entity, light] : scene.getEntitiesOfType<Light>()) {
    if (!scene.hasComponent<TransformComponent>(entity)) continue;

    const TransformComponent& transform = scene.getComponent<TransformComponent>(entity);
    const ColourComponent& colour = scene.getComponent<ColourComponent>(entity);

    const std::string lightType =
      (light->param1.x == 0) ? "Point" :
      (light->param1.x == 1) ? "Spot" :
      /*      param1.x == 2)*/ "Directional";

    file << "light, "
      << light->name << ", "
      << lightType << ", ";
		if (!writeTransform(file, transform, false)) return false;
    file << colour.colour << ", "
      << light->attenuation << ", "
      << light->param1.y << "\n";
  }

  return !file.fail();
}

bool Writer::writeTransform(std::ostream& file, const TransformComponent& transform, bool includeSize) {
  file << transform.pos << ", ";
  file << transform.rot << ", ";
  if (includeSize) file << transform.size << ", ";
  return !file.fail();
}
bool Writer::writeColourMode(std::ostream& file, const Model& model, const ColourComponent& colour) {
  switch (model.mode) {
  case ColourMode::Solid: {
    Vec4 rgba{ colour.colour.x * 255.0f, colour.colour.y * 255.0f, colour.colour.z * 255.0f, colour.colour.w * 255.0f };
    if (rgba.r == 255 && rgba.g == 0 && rgba.b == 0 && rgba.a == 255) file << "Red";
    else if (rgba.r == 0 && rgba.g == 255 && rgba.b == 0 && rgba.a == 255) file << "Green";
    else if (rgba.r == 0 && rgba.g == 0 && rgba.b == 255 && rgba.a == 255) file << "Blue";
    else file << rgba;
    break;
  }
  case ColourMode::Random:           file << "Random"; break;
  case ColourMode::VerticalGradient: file << "Rainbow"; break;
  case ColourMode::PLYColour:        file << "PLY"; break;
  default: break;
  }

  return !file.fail();
}