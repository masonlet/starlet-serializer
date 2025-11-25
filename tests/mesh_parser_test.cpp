#include "test_helpers.hpp"

// PLY format detection
TEST_F(MeshParserTest, DetectFormatPlyLowercase) {
  createTestFile("test_data/model.ply", "ply");

  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/model.ply", out));
  std::string output = testing::internal::GetCapturedStderr();
    
  // PLY extension was properly detected and routed to PlyParser
  EXPECT_NE(output.find("PlyParser"), std::string::npos);
}

TEST_F(MeshParserTest, DetectFormatPlyUppercase) {
  createTestFile("test_data/model.PLY", "ply");

  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/model.PLY", out));
  std::string output = testing::internal::GetCapturedStderr();

  // PLY extension was properly detected and routed to PlyParser
  EXPECT_NE(output.find("PlyParser"), std::string::npos);
}

TEST_F(MeshParserTest, DetectFormatPlyMixedCase) {
  createTestFile("test_data/model.PlY", "ply");

  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/model.PlY", out));
  std::string output = testing::internal::GetCapturedStderr();

  // PLY extension was properly detected and routed to PlyParser
  EXPECT_NE(output.find("PlyParser"), std::string::npos);
}


// OBJ format detection
TEST_F(MeshParserTest, DetectFormatObjLowercase) {
  createTestFile("test_data/model.obj", "obj");

  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/model.obj", out));
  std::string output = testing::internal::GetCapturedStderr();

  // OBJ extension was properly detected and routed to ObjParser
  EXPECT_NE(output.find("ObjParser"), std::string::npos);
}

TEST_F(MeshParserTest, DetectFormatObjUppercase) {
  createTestFile("test_data/model.OBJ", "obj");

  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/model.OBJ", out));
  std::string output = testing::internal::GetCapturedStderr();

  // OBJ extension was properly detected and routed to ObjParser
  EXPECT_NE(output.find("ObjParser"), std::string::npos);
}

TEST_F(MeshParserTest, DetectFormatObjMixedCase) {
  createTestFile("test_data/model.ObJ", "obj");

  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/model.ObJ", out));
  std::string output = testing::internal::GetCapturedStderr();

  // OBJ extension was properly detected and routed to ObjParser
  EXPECT_NE(output.find("ObjParser"), std::string::npos);
}


// Edge cases
TEST_F(MeshParserTest, DetectFormatNoExtension) {
  createTestFile("test_data/model", "");

  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/model", out));
  std::string output = testing::internal::GetCapturedStderr();

  EXPECT_NE(output.find("Unsupported mesh format: test_data/model"), std::string::npos);
}

TEST_F(MeshParserTest, DetectFormatUnknownExtension) {
  createTestFile("test_data/model.unknown", "");

  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/model.unknown", out));
  std::string output = testing::internal::GetCapturedStderr();

  EXPECT_NE(output.find("Unsupported mesh format: test_data/model.unknown"), std::string::npos);
}

TEST_F(MeshParserTest, DetectFormatEmptyExtension) {
  createTestFile("test_data/model.", "");

  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/model.", out));
  std::string output = testing::internal::GetCapturedStderr();

  EXPECT_NE(output.find("Unsupported mesh format: test_data/model."), std::string::npos);
}
