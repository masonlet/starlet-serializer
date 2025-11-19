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
  EXPECT_FALSE(parser.parse("test_data/image.bmp", out));
}

TEST(ImageParserTest, DetectFormatBmpUppercase) {
  SSerializer::ImageParser parser;
  SSerializer::ImageData out;
  createTestFile("test_data/image.BMP", "BM");
  EXPECT_FALSE(parser.parse("test_data/image.BMP", out));
}

TEST(ImageParserTest, DetectFormatBmpMixedCase) {
  SSerializer::ImageParser parser;
  SSerializer::ImageData out;
  createTestFile("test_data/image.BmP", "BM");
  EXPECT_FALSE(parser.parse("test_data/image.BmP", out));
}


// TGA format detection
TEST(ImageParserTest, DetectFormatTgaLowercase) {
  SSerializer::ImageParser parser;
  SSerializer::ImageData out;
  createTestFile("test_data/image.tga", std::string(18, '\0'));
  EXPECT_FALSE(parser.parse("test_data/image.tga", out));
}

TEST(ImageParserTest, DetectFormatTgaUppercase) {
  SSerializer::ImageParser parser;
  SSerializer::ImageData out;
  createTestFile("test_data/image.TGA", std::string(18, '\0'));
  EXPECT_FALSE(parser.parse("test_data/image.TGA", out));
}

TEST(ImageParserTest, DetectFormatTgaMixedCase) {
  SSerializer::ImageParser parser;
  SSerializer::ImageData out;
  createTestFile("test_data/image.TgA", std::string(18, '\0'));
  EXPECT_FALSE(parser.parse("test_data/image.TgA", out));
}

// Edge cases
TEST(ImageParserTest, DetectFormatNoExtension) {
  SSerializer::ImageParser parser;
  SSerializer::ImageData out;
  createTestFile("test_data/image", "BM");
  EXPECT_FALSE(parser.parse("test_data/image", out));
}

TEST(ImageParserTest, DetectFormatUnknownExtension) {
  SSerializer::ImageParser parser;
  SSerializer::ImageData out;
  createTestFile("test_data/image.unknown", "BM");
  EXPECT_FALSE(parser.parse("test_data/image.unknown", out));
}

TEST(ImageParserTest, DetectFormatEmptyExtension) {
  SSerializer::ImageParser parser;
  SSerializer::ImageData out;
  createTestFile("test_data/image.", "BM");
  EXPECT_FALSE(parser.parse("test_data/image.", out));
}
