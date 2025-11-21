#include <gtest/gtest.h>

#include "starlet-serializer/parser/imageParser.hpp"
#include "starlet-serializer/data/imageData.hpp"
  
#include <filesystem>
#include <fstream>

namespace SSerializer = Starlet::Serializer;

namespace {
  void createTestFile(const std::string& path, const std::string_view& content) {
    std::filesystem::path p(path);
    std::filesystem::create_directories(p.parent_path());
    std::ofstream file(path, std::ios::binary);
    file.write(content.data(), content.size());
  }
}


// BMP format detection
TEST(ImageParserTest, DetectFormatBmpLowercase) {
  SSerializer::ImageParser parser;
  SSerializer::ImageData out;
  createTestFile("test_data/image.bmp", "BM");

  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/image.bmp", out));
  std::string output = testing::internal::GetCapturedStderr();

  // BMP extension was properly detected and routed to BmpParser
  EXPECT_NE(output.find("BmpParser"), std::string::npos);
}

TEST(ImageParserTest, DetectFormatBmpUppercase) {
  SSerializer::ImageParser parser;
  SSerializer::ImageData out;
  createTestFile("test_data/image.BMP", "BM");

  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/image.BMP", out));
  std::string output = testing::internal::GetCapturedStderr();

  // BMP extension was properly detected and routed to BmpParser
  EXPECT_NE(output.find("BmpParser"), std::string::npos);
}

TEST(ImageParserTest, DetectFormatBmpMixedCase) {
  SSerializer::ImageParser parser;
  SSerializer::ImageData out;
  createTestFile("test_data/image.BmP", "BM");

  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/image.BmP", out));
  std::string output = testing::internal::GetCapturedStderr();

  // BMP extension was properly detected and routed to BmpParser
  EXPECT_NE(output.find("BmpParser"), std::string::npos);
}


// TGA format detection
TEST(ImageParserTest, DetectFormatTgaLowercase) {
  SSerializer::ImageParser parser;
  SSerializer::ImageData out;
  createTestFile("test_data/image.tga", std::string(18, '\0'));

  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/image.tga", out));
  std::string output = testing::internal::GetCapturedStderr();

  // TGA extension was properly detected and routed to TgaParser
  EXPECT_NE(output.find("TgaParser"), std::string::npos);
}

TEST(ImageParserTest, DetectFormatTgaUppercase) {
  SSerializer::ImageParser parser;
  SSerializer::ImageData out;
  createTestFile("test_data/image.TGA", std::string(18, '\0'));

  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/image.TGA", out));
  std::string output = testing::internal::GetCapturedStderr();

  // TGA extension was properly detected and routed to TgaParser
  EXPECT_NE(output.find("TgaParser"), std::string::npos);
}

TEST(ImageParserTest, DetectFormatTgaMixedCase) {
  SSerializer::ImageParser parser;
  SSerializer::ImageData out;
  createTestFile("test_data/image.TgA", std::string(18, '\0'));

  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/image.TgA", out));
  std::string output = testing::internal::GetCapturedStderr();

  // TGA extension was properly detected and routed to TgaParser
  EXPECT_NE(output.find("TgaParser"), std::string::npos);
}

// Edge cases
TEST(ImageParserTest, DetectFormatNoExtension) {
  SSerializer::ImageParser parser;
  SSerializer::ImageData out;
  createTestFile("test_data/image", "No extension");

  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/image", out));
  std::string output = testing::internal::GetCapturedStderr();

  EXPECT_NE(output.find("Unsupported image format: test_data/image"), std::string::npos);
}

TEST(ImageParserTest, DetectFormatUnknownExtension) {
  SSerializer::ImageParser parser;
  SSerializer::ImageData out;
  createTestFile("test_data/image.unknown", "BM");

  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/image.unknown", out));
  std::string output = testing::internal::GetCapturedStderr();

  EXPECT_NE(output.find("Unsupported image format: test_data/image.unknown"), std::string::npos);
}

TEST(ImageParserTest, DetectFormatEmptyExtension) {
  SSerializer::ImageParser parser;
  SSerializer::ImageData out;
  createTestFile("test_data/image.", "BM");

  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/image.", out));
  std::string output = testing::internal::GetCapturedStderr();

  EXPECT_NE(output.find("Unsupported image format: test_data/image."), std::string::npos);
}
