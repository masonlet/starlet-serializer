#include "test_helpers.hpp"

// PLY format detection
TEST_F(MeshParserTest, DetectFormatPlyLowercase) {
  createTestFile("test_data/model.ply", "ply\nformat ascii 1.0\nelement vertex 0\nelement face 0\nend_header\n");

  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/model.ply", out));
  std::string output = testing::internal::GetCapturedStderr();
    
  // PLY extension was properly detected and routed to PlyParser
  EXPECT_NE(output.find("PlyParser"), std::string::npos);
}

TEST_F(MeshParserTest, DetectFormatPlyUppercase) {
  createTestFile("test_data/model.PLY", "ply\nformat ascii 1.0\nelement vertex 0\nelement face 0\nend_header\n");

  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/model.PLY", out));
  std::string output = testing::internal::GetCapturedStderr();

  // PLY extension was properly detected and routed to PlyParser
  EXPECT_NE(output.find("PlyParser"), std::string::npos);
}

TEST_F(MeshParserTest, DetectFormatPlyMixedCase) {
  createTestFile("test_data/model.PlY", "ply\nformat ascii 1.0\nelement vertex 0\nelement face 0\nend_header\n");

  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/model.PlY", out));
  std::string output = testing::internal::GetCapturedStderr();

  // PLY extension was properly detected and routed to PlyParser
  EXPECT_NE(output.find("PlyParser"), std::string::npos);
}


// Edge cases
TEST_F(MeshParserTest, DetectFormatNoExtension) {
  createTestFile("test_data/model", "ply\nformat ascii 1.0\nelement vertex 0\nelement face 0\nend_header\n");

  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/model", out));
  std::string output = testing::internal::GetCapturedStderr();

  EXPECT_NE(output.find("Unsupported mesh format: test_data/model"), std::string::npos);
}

TEST_F(MeshParserTest, DetectFormatUnknownExtension) {
  createTestFile("test_data/model.unknown", "ply\nformat ascii 1.0\nelement vertex 0\nelement face 0\nend_header\n");

  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/model.unknown", out));
  std::string output = testing::internal::GetCapturedStderr();

  EXPECT_NE(output.find("Unsupported mesh format: test_data/model.unknown"), std::string::npos);
}

TEST_F(MeshParserTest, DetectFormatEmptyExtension) {
  createTestFile("test_data/model.", "ply\n");

  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/model.", out));
  std::string output = testing::internal::GetCapturedStderr();

  EXPECT_NE(output.find("Unsupported mesh format: test_data/model."), std::string::npos);
}