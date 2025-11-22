#include "starlet-serializer/writer/writer.hpp"
#include "starlet-logger/logger.hpp"

#include "starlet-serializer/data/scene_data.hpp"
#include "starlet-serializer/data/model_data.hpp"
#include "starlet-serializer/data/light_data.hpp"
#include "starlet-serializer/data/camera_data.hpp"
#include "starlet-serializer/data/grid_data.hpp"
#include "starlet-serializer/data/texture_data.hpp"
#include "starlet-serializer/data/primitive_data.hpp"
#include "starlet-serializer/data/transform_data.hpp"
#include "starlet-serializer/data/velocity_data.hpp"
#include "starlet-serializer/data/colour_data.hpp"

#include <iomanip>

namespace Starlet::Serializer {

bool Writer::writeScene(const SceneData& data, const std::string& path) {
  std::ofstream file(path);
  if (!file.is_open())
    return Logger::error("Writer", "writeScene", "Failed to open file for saving: " + path);

  file << std::fixed << std::setprecision(3);

  STARLET_WRITE_OR(return false, writeCameras, "cameras");
  STARLET_WRITE_OR(return false, writeAmbient, "ambient");
  STARLET_WRITE_OR(return false, writeTextures, "textures");
  STARLET_WRITE_OR(return false, writeModels, "models");
  STARLET_WRITE_OR(return false, writeTextureConnections, "texture connections");
  STARLET_WRITE_OR(return false, writeLights, "lights");
  STARLET_WRITE_OR(return false, writePrimitives, "primitives");
  STARLET_WRITE_OR(return false, writeGrids, "grids");
  STARLET_WRITE_OR(return false, writeVelocities, "velocity");

  return true;
}

bool Writer::writeCameras(std::ostream& file, const SceneData& data) {
  if (data.cameras.empty()) return true;

  file << "comment, enabled(on/off), name, pos(xyz), rot(yaw pitch), fov, nearPlane farPlane, camSpeed\n";
  for (const CameraData& cam : data.cameras) {
    file << "camera, "
      << (cam.enabled ? "on" : "off") << ", "
      << cam.name << ", ";
    if (!writeTransform(file, cam.transform, false)) return false;
    file << cam.fov << ", "
      << cam.nearPlane << " " << cam.farPlane << ", "
      << cam.moveSpeed << "\n";
  }

  return !file.fail();
}
bool Writer::writeModels(std::ostream& file, const SceneData& data) {
  if (data.models.empty()) return true;

  file << "\ncomment, enabled(on/off), lighting(on/off), name, meshPath, pos(xyz), rot(xyz), scale(xyz), colour(Int, Named Colour, Random, Rainbow, PLY), specular(rgb, power)\n";
  for (const ModelData& model : data.models) {
    file << "model, "
      << (model.isVisible ? "on" : "off") << ", "
      << (model.isLighted ? "on" : "off") << ", "
      << model.name << ", "
      << model.meshPath << ", ";

    if (!writeTransform(file, model.transform)) return false;
    if (!writeColourMode(file, model)) return false;
    file << ", " << model.colour.specular << "\n";
  }

  return !file.fail();
}
bool Writer::writeLights(std::ostream& file, const SceneData& data) {
  if (data.lights.empty()) return true;
    
  file << "\ncomment, name, type, pos (xyz), diffuse (rgba), attention (xyzw), direction, param1 (spotlight inner, spotlight outer), param2 (on/off)\n";
  for (const LightData& light : data.lights) {
    std::string type;
    switch (light.type) {
    case LightType::Point: type = "Point"; break;
    case LightType::Spot: type = "Spot"; break;
    case LightType::Directional: type = "Directional"; break;
    }

    file << "light, "
      << (light.enabled ? "on" : "off") << ", "
      << light.name << ", "
      << type << ", ";
    if (!writeTransform(file, light.transform, false)) return false;
    file << light.colour.colour << ", "
      << light.attenuation.x << ' ' << light.attenuation.y << ' ' << light.attenuation.z << ", "
      << light.param1.x << ' ' << light.param1.y << "\n";
  }

  return !file.fail();
}

bool Writer::writePrimitives(std::ostream& file, const SceneData& data) {
  if (data.primitives.empty()) return true;

  file << "\ncomment, name, pos(xyz), rot(xyz), scale(xyz), colour, specular\n";
  for (const PrimitiveData& prim : data.primitives) {
    std::string type;
    switch (prim.type) {
    case PrimitiveType::Triangle: type = "triangle"; break;
    case PrimitiveType::Square: type = "square"; break;
    case PrimitiveType::Cube: type = "cube"; break;
    }

    file << type << ", " << prim.name << ", ";
    if (!writeTransform(file, prim.transform)) return false;
    file << prim.colour.colour << ", " << prim.colour.specular << "\n";
  }
  return !file.fail();
}

bool Writer::writeTextures(std::ostream& file, const SceneData& data) {
  if (data.textures.empty()) return true;

  file << "\ncomment, name, bmpPath(s), mix, tiling\n";
  for (const TextureData& tex : data.textures) {
    if (tex.isCube) {
      file << "textureCube, " << tex.name << ", ";
      for (int i = 0; i < 6; ++i) {
        file << tex.faces[i];
        if (i < 5) file << " ";
      }
      file << ", " << tex.mix << ", " 
        << tex.tiling.x << ' ' << tex.tiling.y << "\n";
    }
    else {
      file << "texture, " << tex.name << ", " << tex.faces[0]
        << ", " << tex.mix << ", " 
        << tex.tiling.x << ' ' << tex.tiling.y << "\n";
    }
  }
  return !file.fail();
}

bool Writer::writeTextureConnections(std::ostream& file, const SceneData& data) {
  if (data.textureConnections.empty()) return true;

  file << "\ncomment, modelName, slot, textureName, mix\n";
  for (const TextureConnection& conn : data.textureConnections) {
    file << "textureAdd, "
      << conn.modelName << ", "
      << conn.slot << ", "
      << conn.textureName << ", "
      << conn.mix << "\n";
  }
  return !file.fail();
}

bool Writer::writeGrids(std::ostream& file, const SceneData& data) {
  if (data.grids.empty()) return true;

  file << "\ncomment, name, count, spacing, pos(xyz), rot(xyz), scale(xyz), colour, specular\n";
  for (const GridData& grid : data.grids) {
    std::string type;
    switch (grid.type) {
    case GridType::Square: type = "squareGrid"; break;
    case GridType::Cube: type = "cubeGrid"; break;
    case GridType::Model: type = "modelGrid"; break;
    }

    file << type << ", " << grid.name << ", "
      << grid.count << ", " << grid.spacing << ", ";
    if (!writeTransform(file, grid.transform)) return false;
    file << grid.colour.colour << ", " << grid.colour.specular << "\n";
  }
  return !file.fail();
}

bool Writer::writeVelocities(std::ostream& file, const SceneData& data) {
  if (data.velocities.empty()) return true;

  file << "\ncomment, modelName, velocity(xyz)\n";
  for (const VelocityData& vel : data.velocities) 
    file << "velocity, " << vel.modelName << ", " << vel.velocity << "\n";
    
  return !file.fail();
}

bool Writer::writeAmbient(std::ostream& file, const SceneData& data) {
  file << "\ncomment, enabled(on/off), colour(rgb)\n";
  file << "ambient, "
    << (data.ambientEnabled ? "on" : "off") << ", "
    << data.ambientLight << "\n";
  return !file.fail();
}


bool Writer::writeTransform(std::ostream& file, const TransformData& transform, bool includeSize) {
  file << transform.pos << ", ";
  file << transform.rot << ", ";
  if (includeSize) file << transform.size << ", ";
  return !file.fail();
}
bool Writer::writeColourMode(std::ostream& file, const ModelData& model) {
  switch (model.mode) {
  case ColourMode::Solid: {
    Math::Vec4 rgba{ model.colour.colour.x * 255.0f, model.colour.colour.y * 255.0f, model.colour.colour.z * 255.0f, model.colour.colour.w * 255.0f };
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

}