#pragma once
#include <gtest/gtest.h>

#include "starlet-serializer/parser/imageParser.hpp"
#include "starlet-serializer/data/imageData.hpp"

#include <filesystem>
#include <fstream>

namespace SSerializer = Starlet::Serializer;

inline void createTestFile(const std::string& path, const std::string_view& content) {
  std::filesystem::path p(path);
  std::filesystem::create_directories(p.parent_path());
  std::ofstream file(path, std::ios::binary);
  file.write(content.data(), content.size());
}
inline void createBinaryFile(const std::string& path, const std::vector<unsigned char>& data) {
  std::filesystem::path p(path);
  std::filesystem::create_directories(p.parent_path());
  std::ofstream file(path, std::ios::binary);
  file.write(reinterpret_cast<const char*>(data.data()), data.size());
}

class ImageParserTest : public ::testing::Test {
protected:
  void expectValidParse(const std::string& filename, uint32_t width, uint32_t height) {
    EXPECT_TRUE(parser.parse(filename, out));
    EXPECT_EQ(out.width, width);
    EXPECT_EQ(out.height, height);
    EXPECT_EQ(out.pixelSize, 3);
    EXPECT_EQ(out.byteSize, width * height * 3);
    EXPECT_EQ(out.pixels.size(), out.byteSize);
  }

  void expectInvalidParse(const std::string& filename) {
    EXPECT_FALSE(parser.parse(filename, out));
	}

  SSerializer::ImageParser parser;
  SSerializer::ImageData out;
};