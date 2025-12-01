#include "gtest/gtest.h"

#include "starlet-serializer/parser/scene_parser.hpp"
#include "starlet-serializer/data/scene_data.hpp"
#include "test_helpers.hpp"

#include "starlet-math/vec4.hpp"

using namespace Starlet::Serializer;

class SceneParserTest : public ::testing::Test {
protected:
  void expectValidParse(const std::string& filename) {
    EXPECT_TRUE(parser.parse(filename, out));
  }

  void expectInvalidParse(const std::string& filename) {
    EXPECT_FALSE(parser.parse(filename, out));
  }

  SSerializer::SceneParser parser;
  SSerializer::SceneData out;
};


TEST_F(SceneParserTest, MinimalScene) {
  createTestFile("test_data/minimal.txt", "camera true TestCam 0 0 -5 0 0 90 0.1 100 5\n");
  expectValidParse("test_data/minimal.txt");
  EXPECT_EQ(out.cameras.size(), 1);
}

TEST_F(SceneParserTest, MultipleEntityTypes) {
  createTestFile("test_data/multiple.txt",
    "model true true TestModel mesh.ply 0 0 0 0 0 0 1 1 1 Red 1 1 1 1\n"
    "light true TestLight Point 0 5 0 0 -1 0 1 1 1 1 1 0 0 1 0 0\n"
    "camera true TestCam 0 0 -5 0 0 90 0.1 100 5\n"
    "texture TestTex image.png 1.0 1 1\n");
  expectValidParse("test_data/multiple.txt");
  EXPECT_EQ(out.models.size(), 1);
  EXPECT_EQ(out.lights.size(), 1);
  EXPECT_EQ(out.cameras.size(), 1);
  EXPECT_EQ(out.textures.size(), 1);
}

TEST_F(SceneParserTest, NegativeValues) {
  createTestFile("test_data/negative.txt", "light true L Point -10 -20 -30 0 0 0 Red -1.0 0 0 1 0 0\n");
  expectValidParse("test_data/negative.txt");
  EXPECT_FLOAT_EQ(out.lights[0].transform.pos.x, -10.0f);
  EXPECT_FLOAT_EQ(out.lights[0].attenuation.x, -1.0f);
}

TEST_F(SceneParserTest, MaxFloatValues) {
  createTestFile("test_data/max_float.txt", "camera true Cam 1e10 1e10 1e10 0 0 90 0.1 1e10 5\n");
  expectValidParse("test_data/max_float.txt");
  EXPECT_FLOAT_EQ(out.cameras[0].transform.pos.x, 1e10f);
}

TEST_F(SceneParserTest, EmptyLineHandling) {
  createTestFile("test_data/empty_line.txt", "\n\n  \n\ncamera true C 0 0 0 0 0 90 0.1 100 5\n\n\n");
  expectValidParse("test_data/empty_line.txt");
  EXPECT_EQ(out.cameras.size(), 1);
}

TEST_F(SceneParserTest, BooleanCaseInsensitiveTrue) {
  createTestFile("test_data/bool_true_insensitive.txt", "camera TrUe TestCam 0 0 -5 0 0 90 0.1 100 5\n");
  expectValidParse("test_data/bool_true_insensitive.txt");
  ASSERT_EQ(out.cameras.size(), 1);
  EXPECT_TRUE(out.cameras[0].enabled);
}
TEST_F(SceneParserTest, BooleanCaseInsensitiveFalse) {
  createTestFile("test_data/bool_false_insensitive.txt", "light FaLsE TestLight Point 0 5 0 0 -1 0 Red 1 0 0 1 0 0\n");
  expectValidParse("test_data/bool_false_insensitive.txt");
  ASSERT_EQ(out.lights.size(), 1);
  EXPECT_FALSE(out.lights[0].enabled);
}

TEST_F(SceneParserTest, LightTypeVariations) {
  createTestFile("test_data/light_types.txt",
    "light true S Spot 0 0 0 0 -1 0 Red 1 0 0 1 0 0\n"
    "light true D Directional 0 0 0 0 -1 0 Red 1 0 0 1 0 0\n"
    "light true P 0 0 0 0 0 -1 0 Red 1 0 0 1 0 0\n"
    "light true SpotNum 1 0 0 0 0 -1 0 Red 1 0 0 1 0 0\n");
  expectValidParse("test_data/light_types.txt");
  EXPECT_EQ(out.lights[0].type, LightType::Spot);
  EXPECT_EQ(out.lights[1].type, LightType::Directional);
  EXPECT_EQ(out.lights[2].type, LightType::Point);
  EXPECT_EQ(out.lights[3].type, LightType::Spot);
}
TEST_F(SceneParserTest, LightTypePointNumericZero) {
  createTestFile("test_data/light_numeric.txt", "light true TestLight 0 0 0 0 0 -1 0 Red 1 0 0 1 0 0\n");
  expectValidParse("test_data/light_numeric.txt");
  ASSERT_EQ(out.lights.size(), 1);
  EXPECT_EQ(out.lights[0].type, LightType::Point);
}

TEST_F(SceneParserTest, EmptyFile) {
  createTestFile("test_data/empty.txt", "");
  expectValidParse("test_data/empty.txt");
  EXPECT_EQ(out.models.size(), 0);
  EXPECT_EQ(out.lights.size(), 0);
  EXPECT_EQ(out.cameras.size(), 0);
}
TEST_F(SceneParserTest, OnlyCommentsFile) {
  createTestFile("test_data/comments.txt",
    "# Comment line 1\n"
    "comment Comment line 2\n"
    "# Comment line 3\n");
  expectValidParse("test_data/comments.txt");
  EXPECT_EQ(out.cameras.size(), 0);
}
TEST_F(SceneParserTest, CommentAndBlankLines) {
  createTestFile("test_data/comments_blanks.txt",
    "# Full line comment\n"
    "\n"
    "camera true TestCam 0 0 -5 0 0 90 0.1 100 5\n"
    "comment This is also ignored\n"
    "light true TestLight Point 0 5 0 0 -1 0 1 1 1 1 1 0 0 1 0 0\n");
  expectValidParse("test_data/comments_blanks.txt");
  EXPECT_EQ(out.cameras.size(), 1);
  EXPECT_EQ(out.lights.size(), 1);
}

TEST_F(SceneParserTest, ColourNamed) {
  createTestFile("test_data/colour_named.txt", "light, on, whitePoint, Point, 20.000 20.000 0.000, 0.000 0.000 0.000, Red, 0.000 0.010 0.005 0.000, 0.000 0.000\n");
  expectValidParse("test_data/colour_named.txt");
  ASSERT_EQ(out.lights.size(), 1);
  const Starlet::Math::Vec4<float>& c = out.lights[0].colour.colour;
  EXPECT_FLOAT_EQ(c.x, 1.0f);
  EXPECT_FLOAT_EQ(c.y, 0.0f);
  EXPECT_FLOAT_EQ(c.z, 0.0f);
}
TEST_F(SceneParserTest, ColoursNamed) {
  createTestFile("test_data/colours_named.txt",
    "light true L1 Point 0 0 0 0 0 0 Red 1 0 0 1 0 0\n"
    "light true L2 Point 0 0 0 0 0 0 Green 1 0 0 1 0 0\n"
    "light true L3 Point 0 0 0 0 0 0 Blue 1 0 0 1 0 0\n"
    "light true L4 Point 0 0 0 0 0 0 Yellow 1 0 0 1 0 0\n"
    "light true L5 Point 0 0 0 0 0 0 White 1 0 0 1 0 0\n"
    "light true L6 Point 0 0 0 0 0 0 Gray 1 0 0 1 0 0\n"
    "light true L7 Point 0 0 0 0 0 0 Grey 1 0 0 1 0 0\n");
  expectValidParse("test_data/colours_named.txt");
  EXPECT_EQ(out.lights.size(), 7);
  EXPECT_FLOAT_EQ(out.lights[0].colour.colour.x, 1.0f);  // Red
  EXPECT_FLOAT_EQ(out.lights[1].colour.colour.y, 1.0f);  // Green
  EXPECT_FLOAT_EQ(out.lights[2].colour.colour.z, 1.0f);  // Blue
  EXPECT_FLOAT_EQ(out.lights[5].colour.colour.x, 0.5f);  // Gray
}
TEST_F(SceneParserTest, ColourRGB) {
  createTestFile("test_data/colour_rgb.txt", "light, true, TestLight, Point, 0 5 0, 0 -1 0, 0.5 0.25 1.0 1.0, 1 0 0 1, 0 0\n");
  expectValidParse("test_data/colour_rgb.txt");
  ASSERT_EQ(out.lights.size(), 1);
  const Starlet::Math::Vec4<float>& c = out.lights[0].colour.colour;
  EXPECT_FLOAT_EQ(c.x, 0.5f);
  EXPECT_FLOAT_EQ(c.y, 0.25f);
  EXPECT_FLOAT_EQ(c.z, 1.0f);
  EXPECT_FLOAT_EQ(c.a, 1.0f);
}
TEST_F(SceneParserTest, Colour255Scale) {
  createTestFile("test_data/colour_255.txt", "light, true, L Point 0 0 0 0 0 0 255 128 64 1.0 1 0 0 1 0 0\n");
  expectValidParse("test_data/colour_255.txt");
  EXPECT_FLOAT_EQ(out.lights[0].colour.colour.x, 1.0f);
  EXPECT_FLOAT_EQ(out.lights[0].colour.colour.y, 128.0f / 255.0f);
  EXPECT_FLOAT_EQ(out.lights[0].colour.colour.z, 64.0f / 255.0f);
}
TEST_F(SceneParserTest, ColourMixedScaleComponents) {
  createTestFile("test_data/colour_mixed.txt", "light true L Point 0 0 0 0 0 0 1.0 255 0.5 1.0 1 0 0 1 0 0\n");
  expectValidParse("test_data/colour_mixed.txt");
  EXPECT_FLOAT_EQ(out.lights[0].colour.colour.x, 1.0f);
  EXPECT_FLOAT_EQ(out.lights[0].colour.colour.y, 1.0f);
  EXPECT_FLOAT_EQ(out.lights[0].colour.colour.z, 0.5f);
}
TEST_F(SceneParserTest, ColourNamedCaseInsensitive) {
  createTestFile("test_data/colour_named_insensitive.txt", "light, true, L, Point, 0 0 0, 0 0 0, rEd, 1 1 1 1, 0 0\n");
  expectValidParse("test_data/colour_named_insensitive.txt");
  EXPECT_FLOAT_EQ(out.lights[0].colour.colour.x, 1.0f);
}
TEST_F(SceneParserTest, ColourSpecialModes) {
  createTestFile("test_data/colours_special.txt",
    "model true true M mesh.ply 0 0 0 0 0 0 1 1 1 Random 1 1 1 1\n"
    "model true true M2 mesh.ply 0 0 0 0 0 0 1 1 1 Rainbow 1 1 1 1\n"
    "model true true M3 mesh.ply 0 0 0 0 0 0 1 1 1 VertexColour 1 1 1 1\n");
  expectValidParse("test_data/colours_special.txt");
  EXPECT_EQ(out.models[0].mode, ColourMode::Random);
  EXPECT_EQ(out.models[1].mode, ColourMode::VerticalGradient);
  EXPECT_EQ(out.models[2].mode, ColourMode::VertexColour);
}

TEST_F(SceneParserTest, ModelZeroScale) {
  createTestFile("test_data/model_zero_scale.txt", "model true true M mesh.ply 0 0 0 0 0 0 0 0 0 Red 1 1 1 1\n");
  expectValidParse("test_data/model_zero_scale.txt");
  EXPECT_EQ(out.models.size(), 1);
  EXPECT_FLOAT_EQ(out.models[0].transform.size.x, 0.0f);
}
TEST_F(SceneParserTest, ModelDisabled) {
  createTestFile("test_data/model_disabled.txt", "model false true TestModel mesh.ply 0 0 0 0 0 0 1 1 1 Red 1 1 1 1\n");
  expectValidParse("test_data/model_disabled.txt");
  ASSERT_EQ(out.models.size(), 1);
  EXPECT_FALSE(out.models[0].isVisible);
}
TEST_F(SceneParserTest, ModelLightingDisabled) {
  createTestFile("test_data/model_lighting_disabled.txt", "model true false TestModel mesh.ply 0 0 0 0 0 0 1 1 1 Red 1 1 1 1\n");
  expectValidParse("test_data/model_lighting_disabled.txt");
  ASSERT_EQ(out.models.size(), 1);
  EXPECT_FALSE(out.models[0].isLighted);
}
TEST_F(SceneParserTest, ModelBothDisabled) {
  createTestFile("test_data/model_all_disabled.txt", "model false false TestModel mesh.ply 0 0 0 0 0 0 1 1 1 Red 1 1 1 1\n");
  expectValidParse("test_data/model_all_disabled.txt");
  ASSERT_EQ(out.models.size(), 1);
  EXPECT_FALSE(out.models[0].isVisible);
  EXPECT_FALSE(out.models[0].isLighted);
}

TEST_F(SceneParserTest, TextureConnectionValidSlot) {
  createTestFile("test_data/texture_connection.txt",
    "model true true TestModel mesh.ply 0 0 0 0 0 0 1 1 1 Red 1 1 1 1\n"
    "texture TestTex image.png 1.0 1 1\n"
    "textureAdd TestModel 0 TestTex 1.0\n");
  expectValidParse("test_data/texture_connection.txt");
  EXPECT_EQ(out.textureConnections.size(), 1);
  EXPECT_STREQ(out.textureConnections[0].modelName.c_str(), "TestModel");
  EXPECT_EQ(out.textureConnections[0].slot, 0);
}
TEST_F(SceneParserTest, TextureZeroIntensity) {
  createTestFile("test_data/texture_zero_intensity.txt", "texture TestTex image.png 0.0 1 1\n");
  expectValidParse("test_data/texture_zero_intensity.txt");
  EXPECT_FLOAT_EQ(out.textures[0].mix, 0.0f);
}
TEST_F(SceneParserTest, TextureZeroTiling) {
  createTestFile("test_data/texture_zero_tiling.txt", "texture TestTex image.png 1.0 0 0\n");
  expectValidParse("test_data/texture_zero_tiling.txt");
  EXPECT_FLOAT_EQ(out.textures[0].tiling.x, 0.0f);
  EXPECT_FLOAT_EQ(out.textures[0].tiling.y, 0.0f);
}
TEST_F(SceneParserTest, CubeTexture) {
  createTestFile("test_data/texture_cube.txt", "textureCube, SkyBox, right.png, left.png, top.png, bottom.png, front.png, back.png, 1.0, 1.0 1.0\n");
  expectValidParse("test_data/texture_cube.txt");
  ASSERT_EQ(out.textures.size(), 1);
  EXPECT_STREQ(out.textures[0].name.c_str(), "SkyBox");
  EXPECT_TRUE(out.textures[0].isCube);
}

TEST_F(SceneParserTest, TrianglePrimitive) {
  createTestFile("test_data/triangle.txt", "triangle, name, 0 0 0, 1 0 0, 0 1 0, Red, 1 1 1 1\n");
  expectValidParse("test_data/triangle.txt");
  EXPECT_EQ(out.primitives.size(), 1);
  EXPECT_EQ(out.primitives[0].type, PrimitiveType::Triangle);
}
TEST_F(SceneParserTest, SquarePrimitive) {
  createTestFile("test_data/square.txt", "square, name, 0 0 0, 1 0 0, 0 1 0, Red, 1 1 1 1\n");
  expectValidParse("test_data/square.txt");
  EXPECT_EQ(out.primitives.size(), 1);
  EXPECT_EQ(out.primitives[0].type, PrimitiveType::Square);
}
TEST_F(SceneParserTest, CubePrimitive) {
  createTestFile("test_data/cube.txt", "cube, name, 0 0 0, 1 0 0, 0 1 0, Red, 1 1 1 1\n");
  expectValidParse("test_data/cube.txt");
  EXPECT_EQ(out.primitives.size(), 1);
  EXPECT_EQ(out.primitives[0].type, PrimitiveType::Cube);
}

TEST_F(SceneParserTest, SquareGrid) {
  createTestFile("test_data/square.txt", "squareGrid, Grid1, 10 1.0, 0.0 0.0 0.0, 0.0 0.0 0.0, 0.0 0.0 0.0, Red, 1 1 1 1\n");
  expectValidParse("test_data/square.txt");
  EXPECT_EQ(out.grids.size(), 1);
  EXPECT_EQ(out.grids[0].type, GridType::Square);
}
TEST_F(SceneParserTest, CubeGrid) {
  createTestFile("test_data/cube.txt", "cubeGrid, GridBox, 5 2.0, 0.0 0.0 0.0, 0.0 0.0 0.0, 0.0 0.0 0.0, Red, 1 1 1 1\n");
  expectValidParse("test_data/cube.txt");
  EXPECT_EQ(out.grids.size(), 1);
  EXPECT_EQ(out.grids[0].type, GridType::Cube);
}

TEST_F(SceneParserTest, AmbientLighting) {
  createTestFile("test_data/ambient.txt", "ambient true 0.2 0.2 0.3\n");
  expectValidParse("test_data/ambient.txt");
  EXPECT_TRUE(out.ambientEnabled);
  EXPECT_FLOAT_EQ(out.ambientLight.x, 0.2f);
  EXPECT_FLOAT_EQ(out.ambientLight.y, 0.2f);
  EXPECT_FLOAT_EQ(out.ambientLight.z, 0.3f);
}

TEST_F(SceneParserTest, Velocity) {
  createTestFile("test_data/velocity.txt",
    "model true true TestModel mesh.ply 0 0 0 0 0 0 1 1 1 Red 1 1 1 1\n"
    "velocity TestModel 1.0 2.0 3.0\n");
  expectValidParse("test_data/velocity.txt");
  EXPECT_EQ(out.velocities.size(), 1);
  EXPECT_STREQ(out.velocities[0].modelName.c_str(), "TestModel");
  EXPECT_FLOAT_EQ(out.velocities[0].velocity.x, 1.0f);
  EXPECT_FLOAT_EQ(out.velocities[0].velocity.y, 2.0f);
  EXPECT_FLOAT_EQ(out.velocities[0].velocity.z, 3.0f);
}

TEST_F(SceneParserTest, CameraDisabled) {
  createTestFile("test_data/camera_disabled.txt", "camera false TestCam 0 0 -5 0 0 90 0.1 100 5\n");
  expectValidParse("test_data/camera_disabled.txt");
  ASSERT_EQ(out.cameras.size(), 1);
  EXPECT_FALSE(out.cameras[0].enabled);
}
TEST_F(SceneParserTest, LightDisabled) {
  createTestFile("test_data/light_disabled.txt", "light false TestLight Point 0 5 0 0 -1 0 Red 1 0 0 1 0 0\n");
  expectValidParse("test_data/light_disabled.txt");
  ASSERT_EQ(out.lights.size(), 1);
  EXPECT_FALSE(out.lights[0].enabled);
}
TEST_F(SceneParserTest, AmbientDisabled) {
  createTestFile("test_data/ambient_disabled.txt", "ambient false 0.2 0.2 0.3\n");
  expectValidParse("test_data/ambient_disabled.txt");
  EXPECT_FALSE(out.ambientEnabled);
  EXPECT_FLOAT_EQ(out.ambientLight.x, 0.2f);
  EXPECT_FLOAT_EQ(out.ambientLight.y, 0.2f);
  EXPECT_FLOAT_EQ(out.ambientLight.z, 0.3f);
}




// Error tests
TEST_F(SceneParserTest, UnknownTokenFails) {
  createTestFile("test_data/unknown_token.txt", "unknownEntity someData\n");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/unknown_token.txt");
  expectStderrContains({
    "Failed to handle: unknownEntity",
    "Failed to process scene line: \"unknownEntity someData\""
  });
}
TEST_F(SceneParserTest, InvalidFilePathFails) {
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/nonexistent_file.txt");
  expectStderrContains({"Failed to open file: test_data/nonexistent_file.txt"});
}
TEST_F(SceneParserTest, ModelMissingParametersFails) {
  createTestFile("test_data/model_missing_param.txt", "model, true, true, TestModel, mesh.ply, 0 0 0, 0 0 0, 1 1\n");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/model_missing_param.txt");
  expectStderrContains({
     "Failed to parse model scale",
     "Failed to parse model",
     "Failed to process scene line: \"model, true, true, TestModel, mesh.ply, 0 0 0, 0 0 0, 1 1\""
    });
}
TEST_F(SceneParserTest, CameraMissingParametersFails) {
  createTestFile("test_data/camera_missing_param.txt", "camera, true, TestCam, 0 0 -5, 0 0 90, 0.1\n");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/camera_missing_param.txt");
  expectStderrContains({
     "Failed to parse camera far plane",
     "Failed to parse camera",
     "Failed to process scene line: \"camera, true, TestCam, 0 0 -5, 0 0 90, 0.1\""
    });
}
TEST_F(SceneParserTest, InvalidNumericFormatFails) {
  createTestFile("test_data/invalid_number.txt", "camera, true, TestCam, NaN 0 -5, 0 0 90, 0.1 100 5\n");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/invalid_number.txt");
  expectStderrContains({ 
    "Failed to parse camera position",
    "Failed to parse camera",
    "Failed to process scene line: \"camera, true, TestCam, NaN 0 -5, 0 0 90, 0.1 100 5\"" }
  );
}
TEST_F(SceneParserTest, MalformedColourFallback) {
  createTestFile("test_data/invalid_colour.txt", "light true L Point 0 0 0 0 0 0 InvalidColour 1 1 1 1 0 0\n");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/invalid_colour.txt");
  expectStderrContains({
    "Failed to parse light diffuse",
    "Failed to parse light",
    "Failed to process scene line: \"light true L Point 0 0 0 0 0 0 InvalidColour 1 1 1 1 0 0\"" 
  });
}
TEST_F(SceneParserTest, InvalidLineStopsParsingEarly) {
  createTestFile("test_data/invalid_line.txt",
    "camera true TestCam 0 0 -5 0 0 90 0.1 100 5\n"
    "unknownCommand invalid data\n"
    "light true TestLight Point 0 5 0 0 -1 0 Red 1 0 0 1 0 0\n");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/invalid_line.txt");
  expectStderrContains({
    "Failed to handle: unknownCommand",
    "Failed to process scene line: \"unknownCommand invalid data\"" 
  });
  EXPECT_EQ(out.cameras.size(), 1);
  EXPECT_EQ(out.lights.size(), 0);
}

// Missing name
TEST_F(SceneParserTest, ModelMissingNameFails) {
  createTestFile("test_data/model_missing_name.txt", "model true true mesh.ply 0 0 0 0 0 0 1 1 1 Red 1 1 1 1\n");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/model_missing_name.txt");
  expectStderrContains({
    "Invalid model name: appears to be a file path",
    "Failed to parse model",
    "Failed to process scene line: \"model true true mesh.ply 0 0 0 0 0 0 1 1 1 Red 1 1 1 1\"" 
  });
}
TEST_F(SceneParserTest, LightMissingNameFails) {
  createTestFile("test_data/light_missing_name.txt", "light, true, Point, 0 0 0 0 0 0 Red 1 0 0 1 0 0\n");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/light_missing_name.txt");
  expectStderrContains({
    "Invalid light name: cannot be a light type",
    "Failed to parse light",
    "Failed to process scene line: \"light, true, Point, 0 0 0 0 0 0 Red 1 0 0 1 0 0\"" 
  });
}
TEST_F(SceneParserTest, CameraMissingNameFails) {
  createTestFile("test_data/camera_missing_name.txt", "camera true 0 0 -5 0 0 90 0.1 100 5\n");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/camera_missing_name.txt");
  expectStderrContains({
    "Invalid camera name: cannot start with a digit",
    "Failed to parse camera",
    "Failed to process scene line: \"camera true 0 0 -5 0 0 90 0.1 100 5\"" 
  });
}
TEST_F(SceneParserTest, TextureMissingNameFails) {
  createTestFile("test_data/texture_missing_name.txt", "texture image.png 1.0 1 1\n");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/texture_missing_name.txt");
  expectStderrContains({
    "Invalid texture name: appears to be a file path",
    "Failed to parse texture",
    "Failed to process scene line: \"texture image.png 1.0 1 1\"" 
  });
}
TEST_F(SceneParserTest, PrimitiveMissingNameFails) {
  createTestFile("test_data/primitive_missing_name.txt", "triangle 0 0 0 1 0 0 0 1 0 Red 1 1 1 1\n");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/primitive_missing_name.txt");
  expectStderrContains({
    "Invalid primitive name: cannot start with a digit",
    "Failed to parse triangle",
    "Failed to process scene line: \"triangle 0 0 0 1 0 0 0 1 0 Red 1 1 1 1\"" 
  });
}
TEST_F(SceneParserTest, GridMissingNameFails) {
  createTestFile("test_data/grid_missing_name.txt", "squareGrid 10 1.0 0 0 0 0 0 0 0 0 0 Red 1 1 1 1\n");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/grid_missing_name.txt");
  expectStderrContains({
    "Invalid grid name: cannot start with a digit",
    "Failed to parse square grid", 
    "Failed to process scene line: \"squareGrid 10 1.0 0 0 0 0 0 0 0 0 0 Red 1 1 1 1\""
  });
}

// Ambient light
TEST_F(SceneParserTest, AmbientMissingColourComponentFails) {
  createTestFile("test_data/ambient_missing_colour.txt", "ambient true 0.2 0.2\n");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/ambient_missing_colour.txt");
  expectStderrContains({
    "Failed to parse ambient colour", 
    "Failed to process scene line: \"ambient true 0.2 0.2\""
  });
}
TEST_F(SceneParserTest, AmbientMissingEnabledFails) {
  createTestFile("test_data/ambient_missing_enabled.txt", "ambient 0.2 0.2 0.3\n");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/ambient_missing_enabled.txt");
  expectStderrContains({
    "Ambient missing enabled boolean", 
    "Failed to process scene line: \"ambient 0.2 0.2 0.3\""
  });
}

// Light
TEST_F(SceneParserTest, LightMissingAttenuationFails) {
  createTestFile("test_data/light_missing_attenuation.txt", "light true TestLight Point 0 5 0 0 -1 0 Red\n");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/light_missing_attenuation.txt");
  expectStderrContains({
    "Failed to parse light attenuation",
    "Failed to parse light",
    "Failed to process scene line: \"light true TestLight Point 0 5 0 0 -1 0 Red\""
  });
}
TEST_F(SceneParserTest, LightMissingPositionFails) {
  createTestFile("test_data/light_missing_position.txt", "light true TestLight Point\n");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/light_missing_position.txt");
  expectStderrContains({
    "Failed to parse light position",
    "Failed to parse light",
    "Failed to process scene line: \"light true TestLight Point\""
  });
}
TEST_F(SceneParserTest, LightUnknownTypeFails) {
  createTestFile("test_data/light_unknown.txt", "light true TestLight UnknownType 0 0 0 0 0 0 Red 1 0 0 1 0 0\n");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/light_unknown.txt");
  expectStderrContains({
    "Unknown light type",
    "Failed to parse light type", 
    "Failed to parse light",
    "Failed to process scene line: \"light true TestLight UnknownType 0 0 0 0 0 0 Red 1 0 0 1 0 0\""
  });
}
TEST_F(SceneParserTest, LightInvalidNumericTypeFails) {
  createTestFile("test_data/light_invalid_number.txt", "light true TestLight 99 0 0 0 0 0 0 Red 1 0 0 1 0 0\n");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/light_invalid_number.txt");
  expectStderrContains({
    "Unknown light type", 
    "Failed to parse light type",
    "Failed to parse light", 
    "Failed to process scene line: \"light true TestLight 99 0 0 0 0 0 0 Red 1 0 0 1 0 0\""
  });
}

// Colour 
TEST_F(SceneParserTest, MalformedNumericColourTooFewComponentsFails) {
  createTestFile("test_data/colour_missing_params.txt", "light true L Point 0 0 0 0 0 0 1.0 1.0 1 0 0 1 0 0\n");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/colour_missing_params.txt");
  expectStderrContains({
    " Failed to parse light param1",
    "Failed to parse light",
    "Failed to process scene line: \"light true L Point 0 0 0 0 0 0 1.0 1.0 1 0 0 1 0 0\""
  });
}
TEST_F(SceneParserTest, MalformedNamedColourFails) {
  createTestFile("test_data/colour_invalid.txt", "light true L Point 0 0 0 0 0 0 reed 1 0 0 1 0 0\n");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/colour_invalid.txt");
  expectStderrContains({
    "Failed to parse light diffuse",
    "Failed to parse light",
    "Failed to process scene line: \"light true L Point 0 0 0 0 0 0 reed 1 0 0 1 0 0\""
  });
}
TEST_F(SceneParserTest, MalformedSpecialColourFails) {
  createTestFile("test_data/colour_invalid_special.txt", "model true true M mesh.ply 0 0 0 0 0 0 1 1 1 XYZRainbow 1 1 1 1\n");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/colour_invalid_special.txt");
  expectStderrContains({
    "Failed to parse model",
    "Failed to process scene line: \"model true true M mesh.ply 0 0 0 0 0 0 1 1 1 XYZRainbow 1 1 1 1\""
  });
}

// Texture
TEST_F(SceneParserTest, TextureMissingMixFails) {
  createTestFile("test_data/texture_missing_mix.txt", "texture TestTex image.png 1 1\n");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/texture_missing_mix.txt");
  expectStderrContains({
    "Failed to parse texture tiling",
    "Failed to parse texture",
    "Failed to process scene line: \"texture TestTex image.png 1 1\""
  });
}
TEST_F(SceneParserTest, TextureMissingTilingFails) {
  createTestFile("test_data/texture_missing_tiling.txt", "texture TestTex image.png 1.0\n");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/texture_missing_tiling.txt");
  expectStderrContains({
    "Failed to parse texture tiling",
    "Failed to parse texture",
    "Failed to process scene line: \"texture TestTex image.png 1.0\""
  });
}
TEST_F(SceneParserTest, TextureCubeMissingFacesFails) {
  createTestFile("test_data/texture_cube_missing_faces.txt", "textureCube SkyBox right.png left.png top.png bottom.png 1.0 1 1\n");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/texture_cube_missing_faces.txt");
  expectStderrContains({
    "Invalid cube map face: missing file extension",
    "Failed to parse cube texture",
    "Failed to process scene line: \"textureCube SkyBox right.png left.png top.png bottom.png 1.0 1 1\""
  });
}

// Texture connection
TEST_F(SceneParserTest, TextureConnectionInvalidSlotFails) {
  createTestFile("test_data/texture_invalid_slot.txt",
    "model true true TestModel mesh.ply 0 0 0 0 0 0 1 1 1 Red 1 1 1 1\n"
    "texture TestTex image.png 1.0 1 1\n"
    "textureAdd TestModel 99 TestTex 1.0\n"
  );
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/texture_invalid_slot.txt");
  expectStderrContains({
    "Invalid texture slot index: 99 for model: TestModel",
    "Failed to process scene line: \"textureAdd TestModel 99 TestTex 1.0\""
  });
}
TEST_F(SceneParserTest, TextureConnectionMissingParametersFails) {
  createTestFile("test_data/texture_missing_param.txt",
    "model true true TestModel mesh.ply 0 0 0 0 0 0 1 1 1 Red 1 1 1 1\n"
    "textureAdd TestModel 0\n");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/texture_missing_param.txt");
  expectStderrContains({
    "Failed to parse texture connection name",
    "Failed to process scene line: \"textureAdd TestModel 0\""
  });
}
TEST_F(SceneParserTest, TextureConnectionMissingMixFails) {
  createTestFile("test_data/texture_missing_mix.txt",
    "model true true TestModel mesh.ply 0 0 0 0 0 0 1 1 1 Red 1 1 1 1\n"
    "texture TestTex image.png 1.0 1 1\n"
    "textureAdd TestModel 0 TestTex\n");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/texture_missing_mix.txt");
  expectStderrContains({
    "Failed to parse texture connection mix",
    "Failed to process scene line: \"textureAdd TestModel 0 TestTex\""
  });
}

// Velocity
TEST_F(SceneParserTest, VelocityMissingComponentsFails) {
  createTestFile("test_data/velocity_incomplete.txt",
    "model true true TestModel mesh.ply 0 0 0 0 0 0 1 1 1 Red 1 1 1 1\n"
    "velocity TestModel 1.0 2.0\n");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/velocity_incomplete.txt");
  expectStderrContains({
    "Failed to parse velocity vec3",
    "Failed to parse velocity",
    "Failed to process scene line: \"velocity TestModel 1.0 2.0\"" 
  });
}
TEST_F(SceneParserTest, VelocityMissingModelNameFails) {
  createTestFile("test_data/velocity_missing_model.txt", "velocity 1.0 2.0 3.0\n");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/velocity_missing_model.txt");
  expectStderrContains({
    "Invalid model name: cannot start with a digit",
    "Failed to parse velocity",
    "Failed to process scene line: \"velocity 1.0 2.0 3.0\""
  });
}

// Primitives
TEST_F(SceneParserTest, TriangleMissingColourFails) {
  createTestFile("test_data/triangle_missing_colour.txt", "triangle name 0 0 0 1 0 0 0 1 0\n");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/triangle_missing_colour.txt");
  expectStderrContains({
    "Failed to parse primitive colour", 
    "Failed to parse triangle",
    "Failed to process scene line: \"triangle name 0 0 0 1 0 0 0 1 0\"" 
  });
}
TEST_F(SceneParserTest, SquareMissingScaleFails) {
  createTestFile("test_data/square_missing_scale.txt", "square name 0 0 0 1 0 0 Red 1 1 1 1\n");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/square_missing_scale.txt");
  expectStderrContains({
    "Failed to parse primitive size",
    "Failed to parse square",
    "Failed to process scene line: \"square name 0 0 0 1 0 0 Red 1 1 1 1\""
  });
}
TEST_F(SceneParserTest, CubeMissingPositionFails) {
  createTestFile("test_data/cube_missing_position.txt", "cube name\n");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/cube_missing_position.txt");
  expectStderrContains({
    "Failed to parse primitive position", 
    "Failed to parse cube", 
    "Failed to process scene line: \"cube name\""
  });
}

// Grids
TEST_F(SceneParserTest, SquareGridMissingSpacingFails) {
  createTestFile("test_data/grid_missing_spacing.txt", "squareGrid Grid1 10 0, 0 0 0, 0 0 0, 0 0  , Red 1 1 1 1\n");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/grid_missing_spacing.txt");
  expectStderrContains({
    "Failed to parse grid scale",
    "Failed to parse square grid",
    "Failed to process scene line: \"squareGrid Grid1 10 0, 0 0 0, 0 0 0, 0 0  , Red 1 1 1 1\"" 
  });
}
TEST_F(SceneParserTest, CubeGridMissingColourFails) {
  createTestFile("test_data/grid_missing_colour.txt", "cubeGrid GridBox 5 2.0, 0 0 0, 0 0 0, 0 0 0,\n");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/grid_missing_colour.txt");
  expectStderrContains({
    "Failed to parse grid colour",
    "Failed to parse cube grid",
    "Failed to process scene line: \"cubeGrid GridBox 5 2.0, 0 0 0, 0 0 0, 0 0 0\"" 
  });
}