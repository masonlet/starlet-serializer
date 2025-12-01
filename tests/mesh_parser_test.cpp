#include "test_helpers.hpp"

// PLY format detection
TEST_F(MeshParserTest, DetectFormatPlyLowercase) {
  createTestFile("test_data/model.ply", "ply");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/model.ply");
  expectStderrContains({ "PlyParser" });
}

TEST_F(MeshParserTest, DetectFormatPlyUppercase) {
  createTestFile("test_data/model.PLY", "ply");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/model.PLY");
  expectStderrContains({ "PlyParser" });
}

TEST_F(MeshParserTest, DetectFormatPlyMixedCase) {
  createTestFile("test_data/model.PlY", "ply");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/model.PlY");
  expectStderrContains({ "PlyParser" });
}


// OBJ format detection
TEST_F(MeshParserTest, DetectFormatObjLowercase) {
  createTestFile("test_data/model.obj", "# obj");
  expectValidParse("test_data/model.obj", 0, 0);
}

TEST_F(MeshParserTest, DetectFormatObjUppercase) {
  createTestFile("test_data/model.OBJ", "# obj");
  expectValidParse("test_data/model.OBJ", 0, 0);
}

TEST_F(MeshParserTest, DetectFormatObjMixedCase) {
  createTestFile("test_data/model.ObJ", "# obj");
  expectValidParse("test_data/model.ObJ", 0, 0);
}


// Edge cases
TEST_F(MeshParserTest, DetectFormatNoExtension) {
  createTestFile("test_data/model", "");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/model");
  expectStderrContains({ "Unsupported mesh format: test_data/model" });
}

TEST_F(MeshParserTest, DetectFormatUnknownExtension) {
  createTestFile("test_data/model.unknown", "");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/model.unknown");
  expectStderrContains({ "Unsupported mesh format: test_data/model.unknown" });
}

TEST_F(MeshParserTest, DetectFormatEmptyExtension) {
  createTestFile("test_data/model.", "");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/model.");
  expectStderrContains({ "Unsupported mesh format: test_data/model." });
}