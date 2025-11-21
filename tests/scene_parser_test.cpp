#include "gtest/gtest.h"

#include "starlet-serializer/parser/sceneParser.hpp"
#include "starlet-serializer/data/sceneData.hpp"
#include "test_helpers.hpp"

#include "starlet-math/vec4.hpp"

using namespace Starlet::Serializer;

class SceneParserTest : public ::testing::Test {
protected:
  SSerializer::SceneParser parser;
  SSerializer::SceneData sceneData;
  const std::string testFileName = "test_data/test_scene.txt";
};


TEST_F(SceneParserTest, MinimalScene) {
  createTestFile(testFileName, "camera true TestCam 0 0 -5 0 0 90 0.1 100 5\n");
  ASSERT_TRUE(parser.parse(testFileName, sceneData));
  EXPECT_EQ(sceneData.cameras.size(), 1);
}

TEST_F(SceneParserTest, MultipleEntityTypes) {
  createTestFile(testFileName,
    "model true true TestModel mesh.obj 0 0 0 0 0 0 1 1 1 Red 1 1 1 1\n"
    "light true TestLight Point 0 5 0 0 -1 0 1 1 1 1 1 0 0 1 0 0\n"
    "camera true TestCam 0 0 -5 0 0 90 0.1 100 5\n"
    "texture TestTex image.png 1.0 1 1\n");
  ASSERT_TRUE(parser.parse(testFileName, sceneData));
  EXPECT_EQ(sceneData.models.size(), 1);
  EXPECT_EQ(sceneData.lights.size(), 1);
  EXPECT_EQ(sceneData.cameras.size(), 1);
  EXPECT_EQ(sceneData.textures.size(), 1);
}

TEST_F(SceneParserTest, NegativeValues) {
  createTestFile(testFileName, "light true L Point -10 -20 -30 0 0 0 Red -1.0 0 0 1 0 0\n");

  ASSERT_TRUE(parser.parse(testFileName, sceneData));
  EXPECT_FLOAT_EQ(sceneData.lights[0].transform.pos.x, -10.0f);
  EXPECT_FLOAT_EQ(sceneData.lights[0].attenuation.x, -1.0f);
}

TEST_F(SceneParserTest, MaxFloatValues) {
  createTestFile(testFileName, "camera true Cam 1e10 1e10 1e10 0 0 90 0.1 1e10 5\n");

  ASSERT_TRUE(parser.parse(testFileName, sceneData));
  EXPECT_FLOAT_EQ(sceneData.cameras[0].transform.pos.x, 1e10f);
}

TEST_F(SceneParserTest, EmptyLineHandling) {
  createTestFile(testFileName, "\n\n  \n\ncamera true C 0 0 0 0 0 90 0.1 100 5\n\n\n");

  ASSERT_TRUE(parser.parse(testFileName, sceneData));
  EXPECT_EQ(sceneData.cameras.size(), 1);
}

TEST_F(SceneParserTest, BooleanCaseInsensitiveTrue) {
  createTestFile(testFileName, "camera TrUe TestCam 0 0 -5 0 0 90 0.1 100 5\n");
  ASSERT_TRUE(parser.parse(testFileName, sceneData));
  ASSERT_EQ(sceneData.cameras.size(), 1);
  EXPECT_TRUE(sceneData.cameras[0].enabled);
}
TEST_F(SceneParserTest, BooleanCaseInsensitiveFalse) {
  createTestFile(testFileName, "light FaLsE TestLight Point 0 5 0 0 -1 0 Red 1 0 0 1 0 0\n");
  ASSERT_TRUE(parser.parse(testFileName, sceneData));
  ASSERT_EQ(sceneData.lights.size(), 1);
  EXPECT_FALSE(sceneData.lights[0].enabled);
}

TEST_F(SceneParserTest, LightTypeVariations) {
  createTestFile(testFileName,
    "light true Spot Spot 0 0 0 0 -1 0 Red 1 0 0 1 0 0\n"
    "light true Dir Directional 0 0 0 0 -1 0 Red 1 0 0 1 0 0\n"
    "light true P 0 0 0 0 0 -1 0 Red 1 0 0 1 0 0\n"
    "light true Num 1 0 0 0 0 -1 0 Red 1 0 0 1 0 0\n");

  ASSERT_TRUE(parser.parse(testFileName, sceneData));
  EXPECT_EQ(sceneData.lights[0].type, LightType::Spot);
  EXPECT_EQ(sceneData.lights[1].type, LightType::Directional);
  EXPECT_EQ(sceneData.lights[2].type, LightType::Point);
  EXPECT_EQ(sceneData.lights[3].type, LightType::Spot);
}
TEST_F(SceneParserTest, LightTypePointNumericZero) {
  createTestFile(testFileName, "light true TestLight 0 0 0 0 0 -1 0 Red 1 0 0 1 0 0\n");
  ASSERT_TRUE(parser.parse(testFileName, sceneData));
  ASSERT_EQ(sceneData.lights.size(), 1);
  EXPECT_EQ(sceneData.lights[0].type, LightType::Point);
}

TEST_F(SceneParserTest, EmptyFile) {
  createTestFile(testFileName, "");

  ASSERT_TRUE(parser.parse(testFileName, sceneData));
  EXPECT_EQ(sceneData.models.size(), 0);
  EXPECT_EQ(sceneData.lights.size(), 0);
  EXPECT_EQ(sceneData.cameras.size(), 0);
}
TEST_F(SceneParserTest, OnlyCommentsFile) {
  createTestFile(testFileName,
    "# Comment line 1\n"
    "comment Comment line 2\n"
    "# Comment line 3\n");

  ASSERT_TRUE(parser.parse(testFileName, sceneData));
  EXPECT_EQ(sceneData.cameras.size(), 0);
}
TEST_F(SceneParserTest, CommentAndBlankLines) {
  createTestFile(testFileName,
    "# Full line comment\n"
    "\n"
    "camera true TestCam 0 0 -5 0 0 90 0.1 100 5\n"
    "comment This is also ignored\n"
    "light true TestLight Point 0 5 0 0 -1 0 1 1 1 1 1 0 0 1 0 0\n");
  ASSERT_TRUE(parser.parse(testFileName, sceneData));
  EXPECT_EQ(sceneData.cameras.size(), 1);
  EXPECT_EQ(sceneData.lights.size(), 1);
}

TEST_F(SceneParserTest, ColourNamed) {
  createTestFile(testFileName, "light, on, whitePoint, Point, 20.000 20.000 0.000, 0.000 0.000 0.000, Red, 0.000 0.010 0.005 0.000, 0.000 0.000\n");
  ASSERT_TRUE(parser.parse(testFileName, sceneData));
  ASSERT_EQ(sceneData.lights.size(), 1);
  const Starlet::Math::Vec4<float>& c = sceneData.lights[0].colour.colour;
  EXPECT_FLOAT_EQ(c.x, 1.0f);
  EXPECT_FLOAT_EQ(c.y, 0.0f);
  EXPECT_FLOAT_EQ(c.z, 0.0f);
}
TEST_F(SceneParserTest, ColoursNamed) {
  createTestFile(testFileName,
    "light true L1 Point 0 0 0 0 0 0 Red 1 0 0 1 0 0\n"
    "light true L2 Point 0 0 0 0 0 0 Green 1 0 0 1 0 0\n"
    "light true L3 Point 0 0 0 0 0 0 Blue 1 0 0 1 0 0\n"
    "light true L4 Point 0 0 0 0 0 0 Yellow 1 0 0 1 0 0\n"
    "light true L5 Point 0 0 0 0 0 0 White 1 0 0 1 0 0\n"
    "light true L6 Point 0 0 0 0 0 0 Gray 1 0 0 1 0 0\n"
    "light true L7 Point 0 0 0 0 0 0 Grey 1 0 0 1 0 0\n");
  ASSERT_TRUE(parser.parse(testFileName, sceneData));
  EXPECT_EQ(sceneData.lights.size(), 7);
  EXPECT_FLOAT_EQ(sceneData.lights[0].colour.colour.x, 1.0f);  // Red
  EXPECT_FLOAT_EQ(sceneData.lights[1].colour.colour.y, 1.0f);  // Green
  EXPECT_FLOAT_EQ(sceneData.lights[2].colour.colour.z, 1.0f);  // Blue
  EXPECT_FLOAT_EQ(sceneData.lights[5].colour.colour.x, 0.5f);  // Gray
}
TEST_F(SceneParserTest, ColourRGB) {
  createTestFile(testFileName, "light, true, TestLight, Point, 0 5 0, 0 -1 0, 0.5 0.25 1.0 1.0, 1 0 0 1, 0 0\n");
  ASSERT_TRUE(parser.parse(testFileName, sceneData));
  ASSERT_EQ(sceneData.lights.size(), 1);
  const Starlet::Math::Vec4<float>& c = sceneData.lights[0].colour.colour;
  EXPECT_FLOAT_EQ(c.x, 0.5f);
  EXPECT_FLOAT_EQ(c.y, 0.25f);
  EXPECT_FLOAT_EQ(c.z, 1.0f);
  EXPECT_FLOAT_EQ(c.a, 1.0f);
}
TEST_F(SceneParserTest, Colour255Scale) {
  createTestFile(testFileName, "light, true, L Point 0 0 0 0 0 0 255 128 64 1.0 1 0 0 1 0 0\n");
  ASSERT_TRUE(parser.parse(testFileName, sceneData));
  EXPECT_FLOAT_EQ(sceneData.lights[0].colour.colour.x, 1.0f);
  EXPECT_FLOAT_EQ(sceneData.lights[0].colour.colour.y, 128.0f / 255.0f);
  EXPECT_FLOAT_EQ(sceneData.lights[0].colour.colour.z, 64.0f / 255.0f);
}
TEST_F(SceneParserTest, ColourMixedScaleComponents) {
  createTestFile(testFileName, "light true L Point 0 0 0 0 0 0 1.0 255 0.5 1.0 1 0 0 1 0 0\n");
  ASSERT_TRUE(parser.parse(testFileName, sceneData));
  EXPECT_FLOAT_EQ(sceneData.lights[0].colour.colour.x, 1.0f);
  EXPECT_FLOAT_EQ(sceneData.lights[0].colour.colour.y, 1.0f);
  EXPECT_FLOAT_EQ(sceneData.lights[0].colour.colour.z, 0.5f);
}
TEST_F(SceneParserTest, ColourNamedCaseInsensitive) {
  createTestFile(testFileName, "light, true, L, Point, 0 0 0, 0 0 0, rEd, 1 1 1 1, 0 0\n");
  ASSERT_TRUE(parser.parse(testFileName, sceneData));
  EXPECT_FLOAT_EQ(sceneData.lights[0].colour.colour.x, 1.0f);
}
TEST_F(SceneParserTest, ColourSpecialModes) {
  createTestFile(testFileName,
    "model true true M mesh.obj 0 0 0 0 0 0 1 1 1 Random 1 1 1 1\n"
    "model true true M2 mesh.obj 0 0 0 0 0 0 1 1 1 Rainbow 1 1 1 1\n"
    "model true true M3 mesh.obj 0 0 0 0 0 0 1 1 1 PLY 1 1 1 1\n");
  ASSERT_TRUE(parser.parse(testFileName, sceneData));
  EXPECT_EQ(sceneData.models[0].mode, ColourMode::Random);
  EXPECT_EQ(sceneData.models[1].mode, ColourMode::VerticalGradient);
  EXPECT_EQ(sceneData.models[2].mode, ColourMode::PLYColour);
}

TEST_F(SceneParserTest, ModelZeroScale) {
  createTestFile(testFileName, "model true true M mesh.obj 0 0 0 0 0 0 0 0 0 Red 1 1 1 1\n");
  ASSERT_TRUE(parser.parse(testFileName, sceneData));
  EXPECT_EQ(sceneData.models.size(), 1);
  EXPECT_FLOAT_EQ(sceneData.models[0].transform.size.x, 0.0f);
}
TEST_F(SceneParserTest, ModelDisabled) {
  createTestFile(testFileName, "model false true TestModel mesh.obj 0 0 0 0 0 0 1 1 1 Red 1 1 1 1\n");
  ASSERT_TRUE(parser.parse(testFileName, sceneData));
  ASSERT_EQ(sceneData.models.size(), 1);
  EXPECT_FALSE(sceneData.models[0].isVisible);
}
TEST_F(SceneParserTest, ModelLightingDisabled) {
  createTestFile(testFileName, "model true false TestModel mesh.obj 0 0 0 0 0 0 1 1 1 Red 1 1 1 1\n");
  ASSERT_TRUE(parser.parse(testFileName, sceneData));
  ASSERT_EQ(sceneData.models.size(), 1);
  EXPECT_FALSE(sceneData.models[0].isLighted);
}
TEST_F(SceneParserTest, ModelBothDisabled) {
  createTestFile(testFileName, "model false false TestModel mesh.obj 0 0 0 0 0 0 1 1 1 Red 1 1 1 1\n");
  ASSERT_TRUE(parser.parse(testFileName, sceneData));
  ASSERT_EQ(sceneData.models.size(), 1);
  EXPECT_FALSE(sceneData.models[0].isVisible);
  EXPECT_FALSE(sceneData.models[0].isLighted);
}

TEST_F(SceneParserTest, TextureConnectionValidSlot) {
  createTestFile(testFileName,
    "model true true TestModel mesh.obj 0 0 0 0 0 0 1 1 1 Red 1 1 1 1\n"
    "texture TestTex image.png 1.0 1 1\n"
    "textureAdd TestModel 0 TestTex 1.0\n");
  ASSERT_TRUE(parser.parse(testFileName, sceneData));
  EXPECT_EQ(sceneData.textureConnections.size(), 1);
  EXPECT_STREQ(sceneData.textureConnections[0].modelName.c_str(), "TestModel");
  EXPECT_EQ(sceneData.textureConnections[0].slot, 0);
}
TEST_F(SceneParserTest, TextureZeroIntensity) {
  createTestFile(testFileName, "texture TestTex image.png 0.0 1 1\n");
  ASSERT_TRUE(parser.parse(testFileName, sceneData));
  EXPECT_FLOAT_EQ(sceneData.textures[0].mix, 0.0f);
}
TEST_F(SceneParserTest, TextureZeroTiling) {
  createTestFile(testFileName, "texture TestTex image.png 1.0 0 0\n");
  ASSERT_TRUE(parser.parse(testFileName, sceneData));
  EXPECT_FLOAT_EQ(sceneData.textures[0].tiling.x, 0.0f);
  EXPECT_FLOAT_EQ(sceneData.textures[0].tiling.y, 0.0f);
}
TEST_F(SceneParserTest, CubeTexture) {
  createTestFile(testFileName, "textureCube, SkyBox, right.png, left.png, top.png, bottom.png, front.png, back.png, 1.0, 1.0 1.0\n");
  ASSERT_TRUE(parser.parse(testFileName, sceneData));
  ASSERT_EQ(sceneData.textures.size(), 1);
  EXPECT_STREQ(sceneData.textures[0].name.c_str(), "SkyBox");
  EXPECT_TRUE(sceneData.textures[0].isCube);
}

TEST_F(SceneParserTest, TrianglePrimitive) {
  createTestFile(testFileName, "triangle, name, 0 0 0, 1 0 0, 0 1 0, Red, 1 1 1 1\n");
  ASSERT_TRUE(parser.parse(testFileName, sceneData));
  EXPECT_EQ(sceneData.primitives.size(), 1);
  EXPECT_EQ(sceneData.primitives[0].type, PrimitiveType::Triangle);
}
TEST_F(SceneParserTest, SquarePrimitive) {
  createTestFile(testFileName, "square, name, 0 0 0, 1 0 0, 0 1 0, Red, 1 1 1 1\n");
  ASSERT_TRUE(parser.parse(testFileName, sceneData));
  EXPECT_EQ(sceneData.primitives.size(), 1);
  EXPECT_EQ(sceneData.primitives[0].type, PrimitiveType::Square);
}
TEST_F(SceneParserTest, CubePrimitive) {
  createTestFile(testFileName, "cube, name, 0 0 0, 1 0 0, 0 1 0, Red, 1 1 1 1\n");
  ASSERT_TRUE(parser.parse(testFileName, sceneData));
  EXPECT_EQ(sceneData.primitives.size(), 1);
  EXPECT_EQ(sceneData.primitives[0].type, PrimitiveType::Cube);
}

TEST_F(SceneParserTest, SquareGrid) {
  createTestFile(testFileName, "squareGrid, Grid1, 10 1.0, 0.0 0.0 0.0, 0.0 0.0 0.0, 0.0 0.0 0.0, Red, 1 1 1 1\n");
  ASSERT_TRUE(parser.parse(testFileName, sceneData));
  EXPECT_EQ(sceneData.grids.size(), 1);
  EXPECT_EQ(sceneData.grids[0].type, GridType::Square);
}
TEST_F(SceneParserTest, CubeGrid) {
  createTestFile(testFileName, "cubeGrid, GridBox, 5 2.0, 0.0 0.0 0.0, 0.0 0.0 0.0, 0.0 0.0 0.0, Red, 1 1 1 1\n");
  ASSERT_TRUE(parser.parse(testFileName, sceneData));
  EXPECT_EQ(sceneData.grids.size(), 1);
  EXPECT_EQ(sceneData.grids[0].type, GridType::Cube);
}

TEST_F(SceneParserTest, AmbientLighting) {
  createTestFile(testFileName, "ambient true 0.2 0.2 0.3\n");
  ASSERT_TRUE(parser.parse(testFileName, sceneData));
  EXPECT_TRUE(sceneData.ambientEnabled);
  EXPECT_FLOAT_EQ(sceneData.ambientLight.x, 0.2f);
  EXPECT_FLOAT_EQ(sceneData.ambientLight.y, 0.2f);
  EXPECT_FLOAT_EQ(sceneData.ambientLight.z, 0.3f);
}

TEST_F(SceneParserTest, Velocity) {
  createTestFile(testFileName,
    "model true true TestModel mesh.obj 0 0 0 0 0 0 1 1 1 Red 1 1 1 1\n"
    "velocity TestModel 1.0 2.0 3.0\n");
  ASSERT_TRUE(parser.parse(testFileName, sceneData));
  EXPECT_EQ(sceneData.velocities.size(), 1);
  EXPECT_STREQ(sceneData.velocities[0].modelName.c_str(), "TestModel");
  EXPECT_FLOAT_EQ(sceneData.velocities[0].velocity.x, 1.0f);
  EXPECT_FLOAT_EQ(sceneData.velocities[0].velocity.y, 2.0f);
  EXPECT_FLOAT_EQ(sceneData.velocities[0].velocity.z, 3.0f);
}

TEST_F(SceneParserTest, CameraDisabled) {
  createTestFile(testFileName, "camera false TestCam 0 0 -5 0 0 90 0.1 100 5\n");
  ASSERT_TRUE(parser.parse(testFileName, sceneData));
  ASSERT_EQ(sceneData.cameras.size(), 1);
  EXPECT_FALSE(sceneData.cameras[0].enabled);
}
TEST_F(SceneParserTest, LightDisabled) {
  createTestFile(testFileName, "light false TestLight Point 0 5 0 0 -1 0 Red 1 0 0 1 0 0\n");
  ASSERT_TRUE(parser.parse(testFileName, sceneData));
  ASSERT_EQ(sceneData.lights.size(), 1);
  EXPECT_FALSE(sceneData.lights[0].enabled);
}
TEST_F(SceneParserTest, AmbientDisabled) {
  createTestFile(testFileName, "ambient false 0.2 0.2 0.3\n");
  ASSERT_TRUE(parser.parse(testFileName, sceneData));
  EXPECT_FALSE(sceneData.ambientEnabled);
  EXPECT_FLOAT_EQ(sceneData.ambientLight.x, 0.2f);
  EXPECT_FLOAT_EQ(sceneData.ambientLight.y, 0.2f);
  EXPECT_FLOAT_EQ(sceneData.ambientLight.z, 0.3f);
}




// Error tests
TEST_F(SceneParserTest, UnknownTokenFails) {
  createTestFile(testFileName, "unknownEntity someData\n");
  EXPECT_FALSE(parser.parse(testFileName, sceneData));
}
TEST_F(SceneParserTest, InvalidFilePathFails) {
  EXPECT_FALSE(parser.parse("nonexistent_file.txt", sceneData));
}
TEST_F(SceneParserTest, ModelMissingParametersFails) {
  createTestFile(testFileName, "model, true, true, TestModel, mesh.obj, 0 0 0, 0 0 0, 1 1\n"); 
  EXPECT_FALSE(parser.parse(testFileName, sceneData));
}
TEST_F(SceneParserTest, CameraMissingParametersFails) {
  createTestFile(testFileName, "camera, true, TestCam, 0 0 -5, 0 0 90, 0.1\n"); 
  EXPECT_FALSE(parser.parse(testFileName, sceneData));
}
TEST_F(SceneParserTest, InvalidNumericFormatFails) {
  createTestFile(testFileName, "camera, true, TestCam, NaN 0 -5, 0 0 90, 0.1 100 5\n");
  EXPECT_FALSE(parser.parse(testFileName, sceneData));
}
TEST_F(SceneParserTest, MalformedColourFallback) {
  createTestFile(testFileName, "light true L Point 0 0 0 0 0 0 InvalidColour 1 1 1 1 0 0\n");
  EXPECT_FALSE(parser.parse(testFileName, sceneData));
}
TEST_F(SceneParserTest, InvalidLineStopsParsingEarly) {
  createTestFile(testFileName,
    "camera true TestCam 0 0 -5 0 0 90 0.1 100 5\n"
    "unknownCommand invalid data\n"
    "light true TestLight Point 0 5 0 0 -1 0 Red 1 0 0 1 0 0\n");

  EXPECT_FALSE(parser.parse(testFileName, sceneData));
  EXPECT_EQ(sceneData.cameras.size(), 1);
  EXPECT_EQ(sceneData.lights.size(), 0);
}

// Missing name
TEST_F(SceneParserTest, ModelMissingNameFails) {
  createTestFile(testFileName, "model true true mesh.obj 0 0 0 0 0 0 1 1 1 Red 1 1 1 1\n");
  EXPECT_FALSE(parser.parse(testFileName, sceneData));
}
TEST_F(SceneParserTest, LightMissingNameFails) {
  createTestFile(testFileName, "light true Point 0 0 0 0 0 0 Red 1 0 0 1 0 0\n");
  EXPECT_FALSE(parser.parse(testFileName, sceneData));
}
TEST_F(SceneParserTest, CameraMissingNameFails) {
  createTestFile(testFileName, "camera true 0 0 -5 0 0 90 0.1 100 5\n");
  EXPECT_FALSE(parser.parse(testFileName, sceneData));
}
TEST_F(SceneParserTest, TextureMissingNameFails) {
  createTestFile(testFileName, "texture image.png 1.0 1 1\n");
  EXPECT_FALSE(parser.parse(testFileName, sceneData));
}
TEST_F(SceneParserTest, PrimitiveMissingNameFails) {
  createTestFile(testFileName, "triangle 0 0 0 1 0 0 0 1 0 Red 1 1 1 1\n");
  EXPECT_FALSE(parser.parse(testFileName, sceneData));
}
TEST_F(SceneParserTest, GridMissingNameFails) {
  createTestFile(testFileName, "squareGrid 10 1.0 0 0 0 0 0 0 0 0 0 Red 1 1 1 1\n");
  EXPECT_FALSE(parser.parse(testFileName, sceneData));
}

// Texture connection
TEST_F(SceneParserTest, TextureConnectionInvalidSlotFails) {
  createTestFile(testFileName,
    "model true true TestModel mesh.obj 0 0 0 0 0 0 1 1 1 Red 1 1 1 1\n"
    "texture TestTex image.png 1.0 1 1\n"
    "textureAdd TestModel 99 TestTex 1.0\n"
  );
  EXPECT_FALSE(parser.parse(testFileName, sceneData));
}
TEST_F(SceneParserTest, TextureConnectionMissingParametersFails) {
  createTestFile(testFileName,
    "model true true TestModel mesh.obj 0 0 0 0 0 0 1 1 1 Red 1 1 1 1\n"
    "textureAdd TestModel 0\n");
  EXPECT_FALSE(parser.parse(testFileName, sceneData));
}
TEST_F(SceneParserTest, TextureConnectionMissingMixFails) {
  createTestFile(testFileName,
    "model true true TestModel mesh.obj 0 0 0 0 0 0 1 1 1 Red 1 1 1 1\n"
    "texture TestTex image.png 1.0 1 1\n"
    "textureAdd TestModel 0 TestTex\n");
  EXPECT_FALSE(parser.parse(testFileName, sceneData));
}

// Ambient light
TEST_F(SceneParserTest, AmbientMissingColourComponentFails) {
  createTestFile(testFileName, "ambient true 0.2 0.2\n");
  EXPECT_FALSE(parser.parse(testFileName, sceneData));
}
TEST_F(SceneParserTest, AmbientMissingEnabledFails) {
  createTestFile(testFileName, "ambient 0.2 0.2 0.3\n");
  EXPECT_FALSE(parser.parse(testFileName, sceneData));
}

// Light
TEST_F(SceneParserTest, LightMissingAttenuationFails) {
  createTestFile(testFileName, "light true TestLight Point 0 5 0 0 -1 0 Red\n");
  EXPECT_FALSE(parser.parse(testFileName, sceneData));
}
TEST_F(SceneParserTest, LightMissingPositionFails) {
  createTestFile(testFileName, "light true TestLight Point\n");
  EXPECT_FALSE(parser.parse(testFileName, sceneData));
}
TEST_F(SceneParserTest, LightUnknownTypeFails) {
  createTestFile(testFileName, "light true TestLight UnknownType 0 0 0 0 0 0 Red 1 0 0 1 0 0\n");
  EXPECT_FALSE(parser.parse(testFileName, sceneData));
}
TEST_F(SceneParserTest, LightInvalidNumericTypeFails) {
  createTestFile(testFileName, "light true TestLight 99 0 0 0 0 0 0 Red 1 0 0 1 0 0\n");
  EXPECT_FALSE(parser.parse(testFileName, sceneData));
}

// Colour 
TEST_F(SceneParserTest, MalformedNumericColourTooFewComponentsFails) {
  createTestFile(testFileName, "light true L Point 0 0 0 0 0 0 1.0 1.0 1 0 0 1 0 0\n");
  EXPECT_FALSE(parser.parse(testFileName, sceneData));
}
TEST_F(SceneParserTest, MalformedSpecialColourFails) {
  createTestFile(testFileName, "model true true M mesh.obj 0 0 0 0 0 0 1 1 1 XYZRainbow 1 1 1 1\n");
  EXPECT_FALSE(parser.parse(testFileName, sceneData));
}
TEST_F(SceneParserTest, MalformedNamedColourFails) {
  createTestFile(testFileName, "light true L Point 0 0 0 0 0 0 reed 1 0 0 1 0 0\n");
  EXPECT_FALSE(parser.parse(testFileName, sceneData));
}

// Texture
TEST_F(SceneParserTest, TextureMissingMixFails) {
  createTestFile(testFileName, "texture TestTex image.png 1 1\n");
  EXPECT_FALSE(parser.parse(testFileName, sceneData));
}
TEST_F(SceneParserTest, TextureMissingTilingFails) {
  createTestFile(testFileName, "texture TestTex image.png 1.0\n");
  EXPECT_FALSE(parser.parse(testFileName, sceneData));
}
TEST_F(SceneParserTest, TextureCubeMissingFacesFails) {
  createTestFile(testFileName, "textureCube SkyBox right.png left.png top.png bottom.png 1.0 1 1\n");
  EXPECT_FALSE(parser.parse(testFileName, sceneData));
}

// Velocity
TEST_F(SceneParserTest, VelocityMissingComponentsFails) {
  createTestFile(testFileName,
    "model true true TestModel mesh.obj 0 0 0 0 0 0 1 1 1 Red 1 1 1 1\n"
    "velocity TestModel 1.0 2.0\n");
  EXPECT_FALSE(parser.parse(testFileName, sceneData));
}
TEST_F(SceneParserTest, VelocityMissingModelNameFails) {
  createTestFile(testFileName, "velocity 1.0 2.0 3.0\n");
  EXPECT_FALSE(parser.parse(testFileName, sceneData));
}

// Primitives
TEST_F(SceneParserTest, TriangleMissingColourFails) {
  createTestFile(testFileName, "triangle name 0 0 0 1 0 0 0 1 0\n");
  EXPECT_FALSE(parser.parse(testFileName, sceneData));
}
TEST_F(SceneParserTest, SquareMissingScaleFails) {
  createTestFile(testFileName, "square name 0 0 0 1 0 0 Red 1 1 1 1\n");
  EXPECT_FALSE(parser.parse(testFileName, sceneData));
}
TEST_F(SceneParserTest, CubeMissingPositionFails) {
  createTestFile(testFileName, "cube name\n");
  EXPECT_FALSE(parser.parse(testFileName, sceneData));
}

// Grids
TEST_F(SceneParserTest, SquareGridMissingSpacingFails) {
  createTestFile(testFileName, "squareGrid Grid1 10 0 0 0 0 0 0 0 0 0 Red 1 1 1 1\n");
  EXPECT_FALSE(parser.parse(testFileName, sceneData));
}
TEST_F(SceneParserTest, CubeGridMissingColourFails) {
  createTestFile(testFileName, "cubeGrid GridBox 5 2.0 0 0 0 0 0 0 0 0 0\n");
  EXPECT_FALSE(parser.parse(testFileName, sceneData));
}