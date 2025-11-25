#include "test_helpers.hpp"

// BMP format detection
TEST_F(ImageParserTest, DetectFormatBmpLowercase) {
  createTestFile("test_data/image.bmp", "BM");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/image.bmp");
  const std::string output = testing::internal::GetCapturedStderr();
  // BMP extension was properly detected and routed to BmpParser
  EXPECT_NE(output.find("BmpParser"), std::string::npos);
}

TEST_F(ImageParserTest, DetectFormatBmpUppercase) {
  createTestFile("test_data/image.BMP", "BM");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/image.BMP");
  const std::string output = testing::internal::GetCapturedStderr();
  // BMP extension was properly detected and routed to BmpParser
  EXPECT_NE(output.find("BmpParser"), std::string::npos);
}

TEST_F(ImageParserTest, DetectFormatBmpMixedCase) {
  createTestFile("test_data/image.BmP", "BM");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/image.BmP");
  const std::string output = testing::internal::GetCapturedStderr();
  // BMP extension was properly detected and routed to BmpParser
  EXPECT_NE(output.find("BmpParser"), std::string::npos);
}


// TGA format detection
TEST_F(ImageParserTest, DetectFormatTgaLowercase) {
  createTestFile("test_data/image.tga", std::string(18, '\0'));
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/image.tga");
  const std::string output = testing::internal::GetCapturedStderr();
  // TGA extension was properly detected and routed to TgaParser
  EXPECT_NE(output.find("TgaParser"), std::string::npos);
}

TEST_F(ImageParserTest, DetectFormatTgaUppercase) {
  createTestFile("test_data/image.TGA", std::string(18, '\0'));
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/image.TGA");
  const std::string output = testing::internal::GetCapturedStderr();
  // TGA extension was properly detected and routed to TgaParser
  EXPECT_NE(output.find("TgaParser"), std::string::npos);
}

TEST_F(ImageParserTest, DetectFormatTgaMixedCase) {
  createTestFile("test_data/image.TgA", std::string(18, '\0'));
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/image.TgA");
  const std::string output = testing::internal::GetCapturedStderr();
  // TGA extension was properly detected and routed to TgaParser
  EXPECT_NE(output.find("TgaParser"), std::string::npos);
}

// Edge cases
TEST_F(ImageParserTest, DetectFormatNoExtension) {
  createTestFile("test_data/image", "No extension");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/image");
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Unsupported image format: test_data/image"), std::string::npos);
}

TEST_F(ImageParserTest, DetectFormatUnknownExtension) {
  createTestFile("test_data/image.unknown", "BM");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/image.unknown");
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Unsupported image format: test_data/image.unknown"), std::string::npos);
}

TEST_F(ImageParserTest, DetectFormatEmptyExtension) {
  createTestFile("test_data/image.", "BM");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/image.");
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Unsupported image format: test_data/image."), std::string::npos);
}
