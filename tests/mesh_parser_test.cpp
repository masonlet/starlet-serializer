#include <gtest/gtest.h>

#include "starlet-serializer/parser/meshParser.hpp"
#include "starlet-serializer/data/meshData.hpp"

#include <filesystem>
#include <fstream>

namespace SSerializer = Starlet::Serializer;

namespace {
  void createTestFile(const std::string& path, const std::string_view& content) {
    std::filesystem::path p(path);
    std::filesystem::create_directories(p.parent_path());
    std::ofstream file(path);
    file << content;
  }
}


// PLY format detection
TEST(MeshParserTest, DetectFormatPlyLowercase) {
  SSerializer::MeshParser parser;
  SSerializer::MeshData out;
  createTestFile("test_data/model.ply", "ply\nformat ascii 1.0\nelement vertex 0\nelement face 0\nend_header\n");
  EXPECT_FALSE(parser.parse("test_data/model.ply", out));
}

TEST(MeshParserTest, DetectFormatPlyUppercase) {
  SSerializer::MeshParser parser;
  SSerializer::MeshData out;
  createTestFile("test_data/model.PLY", "PLY\nformat ascii 1.0\nelement vertex 0\nelement face 0\nend_header\n");
  EXPECT_FALSE(parser.parse("test_data/model.PLY", out));
}

TEST(MeshParserTest, DetectFormatPlyMixedCase) {
  SSerializer::MeshParser parser;
  SSerializer::MeshData out;
  createTestFile("test_data/model.PlY", "PlY\nformat ascii 1.0\nelement vertex 0\nelement face 0\nend_header\n");
  EXPECT_FALSE(parser.parse("test_data/model.PlY", out));
}

// Edge cases
TEST(MeshParserTest, DetectFormatNoExtension) {
  SSerializer::MeshParser parser;
  SSerializer::MeshData out;
  createTestFile("test_data/model", "ply\nformat ascii 1.0\nelement vertex 0\nelement face 0\nend_header\n");
  EXPECT_FALSE(parser.parse("test_data/model", out));
}

TEST(MeshParserTest, DetectFormatUnknownExtension) {
  SSerializer::MeshParser parser;
  SSerializer::MeshData out;
  createTestFile("test_data/model.unknown", "ply\nformat ascii 1.0\nelement vertex 0\nelement face 0\nend_header\n");
  EXPECT_FALSE(parser.parse("test_data/model.unknown", out));
}

TEST(MeshParserTest, DetectFormatEmptyExtension) {
  Starlet::Serializer::MeshParser parser;
  Starlet::Serializer::MeshData out;
  createTestFile("test_data/model.", "ply\n");
  EXPECT_FALSE(parser.parse("test_data/model.", out));
}