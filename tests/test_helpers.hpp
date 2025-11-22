#pragma once
#include <gtest/gtest.h>

#include "starlet-serializer/parser/image_parser.hpp"
#include "starlet-serializer/data/image_data.hpp"

#include "starlet-serializer/parser/mesh_parser.hpp"
#include "starlet-serializer/data/mesh_data.hpp"

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

class MeshParserTest : public ::testing::Test {
protected:
  void expectValidParse(const std::string& fileName, uint32_t numVertices, uint32_t numTriangles) {
    EXPECT_TRUE(parser.parse(fileName, out));
    EXPECT_EQ(out.numVertices, numVertices);
    EXPECT_EQ(out.numTriangles, numTriangles);
    EXPECT_EQ(out.numIndices, numTriangles * 3);
    EXPECT_EQ(out.indices.size(), out.numIndices);
  }

  void expectInvalidParse(const std::string& filename) {
    EXPECT_FALSE(parser.parse(filename, out));
  }

  SSerializer::MeshParser parser;
  SSerializer::MeshData out;
};