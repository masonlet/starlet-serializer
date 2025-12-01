#include "test_helpers.hpp"

// BMP format detection
TEST_F(ImageParserTest, DetectFormatBmpLowercase) {
  createTestFile("test_data/image.bmp", "BM");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/image.bmp");
  expectStderrContains({ "BmpParser" });
}

TEST_F(ImageParserTest, DetectFormatBmpUppercase) {
  createTestFile("test_data/image.BMP", "BM");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/image.BMP");
  expectStderrContains({ "BmpParser" });
}

TEST_F(ImageParserTest, DetectFormatBmpMixedCase) {
  createTestFile("test_data/image.BmP", "BM");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/image.BmP");
  expectStderrContains({ "BmpParser" });
}


// TGA format detection
TEST_F(ImageParserTest, DetectFormatTgaLowercase) {
  createTestFile("test_data/image.tga", std::string(18, '\0'));
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/image.tga");
  expectStderrContains({ "TgaParser" });
}

TEST_F(ImageParserTest, DetectFormatTgaUppercase) {
  createTestFile("test_data/image.TGA", std::string(18, '\0'));
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/image.TGA");
  expectStderrContains({ "TgaParser" });
}

TEST_F(ImageParserTest, DetectFormatTgaMixedCase) {
  createTestFile("test_data/image.TgA", std::string(18, '\0'));
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/image.TgA");
  expectStderrContains({ "TgaParser" });
}

// Edge cases
TEST_F(ImageParserTest, DetectFormatNoExtension) {
  createTestFile("test_data/image", "No extension");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/image");
  expectStderrContains({ "Unsupported image format: test_data/image" });
}

TEST_F(ImageParserTest, DetectFormatUnknownExtension) {
  createTestFile("test_data/image.unknown", "BM");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/image.unknown");
  expectStderrContains({ "Unsupported image format: test_data/image.unknown" });
}

TEST_F(ImageParserTest, DetectFormatEmptyExtension) {
  createTestFile("test_data/image.", "BM");
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/image.");
  expectStderrContains({ "Unsupported image format: test_data/image." });
}