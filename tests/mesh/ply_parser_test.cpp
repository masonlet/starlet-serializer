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


// Valid PLY parsing tests
TEST(PlyParserTest, ValidPlyMinimal) {
  const std::string_view plyContent = R"(ply
format ascii 1.0
element vertex 3
property float x
property float y
property float z
element face 1
property list uchar int vertex_indices
end_header
0.0 0.0 0.0
1 0.0 0
0 1 0
3 0 1 2 
)";
  createTestFile("test_data/minimal.ply", plyContent);
  SSerializer::MeshParser parser;
  SSerializer::MeshData out;
  EXPECT_TRUE(parser.parse("test_data/minimal.ply", out));
  EXPECT_EQ(out.numVertices, 3);
  EXPECT_EQ(out.numTriangles, 1);
  EXPECT_EQ(out.numIndices, 3);
  EXPECT_EQ(out.indices.size(), 3);
  EXPECT_EQ(out.vertices.size(), 3);
  EXPECT_FALSE(out.hasNormals);
  EXPECT_FALSE(out.hasColours);
  EXPECT_FALSE(out.hasTexCoords);
}

TEST(PlyParserTest, ValidPlyWithNormals) {
  const std::string_view plyContent = R"(ply
format ascii 1.0
element vertex 3
property float x
property float y
property float z
property float nx
property float ny
property float nz
element face 1
property list uchar int vertex_indices
end_header
0.0 0.0 0.0 0.0 0.0 1.0
1 0.0 0.0 0.0 0.0 1.0
0 1 0.0 0.0 1.0
3 0 1 2
)";
  createTestFile("test_data/normals.ply", plyContent);
  SSerializer::MeshParser parser;
  SSerializer::MeshData out;
  EXPECT_TRUE(parser.parse("test_data/normals.ply", out));
  EXPECT_EQ(out.numVertices, 3);
  EXPECT_EQ(out.numTriangles, 1);
  EXPECT_EQ(out.numIndices, 3);
  EXPECT_EQ(out.indices.size(), 3);
  EXPECT_EQ(out.vertices.size(), 3);
  EXPECT_TRUE(out.hasNormals);
  EXPECT_FALSE(out.hasColours);
  EXPECT_FALSE(out.hasTexCoords);
}

TEST(PlyParserTest, ValidPlyWithColours) {
  const std::string_view plyContent = R"(ply
format ascii 1.0
element vertex 3
property float x
property float y
property float z
property uchar red
property uchar green
property uchar blue
element face 1
property list uchar int vertex_indices
end_header
0.0 0.0 0.0 255 0 0
1 0.0 0.0 0 255 0
0 1 0.0 0 0 255
3 0 1 2
)";
  createTestFile("test_data/colours.ply", plyContent);
  SSerializer::MeshParser parser;
  SSerializer::MeshData out;
  EXPECT_TRUE(parser.parse("test_data/colours.ply", out));
  EXPECT_EQ(out.numVertices, 3);
  EXPECT_EQ(out.numTriangles, 1);
  EXPECT_EQ(out.numIndices, 3);
  EXPECT_EQ(out.indices.size(), 3);
  EXPECT_EQ(out.vertices.size(), 3);
  EXPECT_FALSE(out.hasNormals);
  EXPECT_TRUE(out.hasColours);
  EXPECT_FLOAT_EQ(out.vertices[0].col.x, 1.0f);
  EXPECT_FLOAT_EQ(out.vertices[0].col.y, 0.0f);
  EXPECT_FLOAT_EQ(out.vertices[0].col.z, 0.0f);
}

TEST(PlyParserTest, ValidPlyWithTexCoords) {
  const std::string_view plyContent = R"(ply
format ascii 1.0
element vertex 3
property float x
property float y
property float z
property float u
property float v
element face 1
property list uchar int vertex_indices
end_header
0.0 0.0 0.0 0.0 0.0
1 0.0 0.0 1.0 0
0 1 0.0 0.5 1.0
3 0 1 2
)";
  createTestFile("test_data/texcoords.ply", plyContent);
  SSerializer::MeshParser parser;
  SSerializer::MeshData out;
  EXPECT_TRUE(parser.parse("test_data/texcoords.ply", out));
  EXPECT_EQ(out.numVertices, 3);
  EXPECT_EQ(out.numTriangles, 1);
  EXPECT_EQ(out.numIndices, 3);
  EXPECT_EQ(out.indices.size(), 3);
  EXPECT_EQ(out.vertices.size(), 3);
  EXPECT_FALSE(out.hasNormals);
  EXPECT_FALSE(out.hasColours);
  EXPECT_TRUE(out.hasTexCoords);
  EXPECT_FLOAT_EQ(out.vertices[2].texCoord.x, 0.5f);
  EXPECT_FLOAT_EQ(out.vertices[2].texCoord.y, 1.0f);
}

TEST(PlyParserTest, ValidPlyMultipleTriangles) {
  const std::string_view plyContent = R"(ply
format ascii 1.0
element vertex 4
property float x
property float y
property float z
element face 2
property list uchar int vertex_indices
end_header
0.0 0.0 0.0
1.0 0.0 0.0
1.0 1.0 0.0
0.0 1.0 0.0
3 0 1 2
3 0 2 3
)";
  createTestFile("test_data/quad.ply", plyContent);
  SSerializer::MeshParser parser;
  SSerializer::MeshData out;
  EXPECT_TRUE(parser.parse("test_data/quad.ply", out));
  EXPECT_EQ(out.numVertices, 4);
  EXPECT_EQ(out.numTriangles, 2);
  EXPECT_EQ(out.numIndices, 6);
  EXPECT_EQ(out.indices[0], 0);
  EXPECT_EQ(out.indices[1], 1);
  EXPECT_EQ(out.indices[2], 2);
  EXPECT_EQ(out.indices[3], 0);
  EXPECT_EQ(out.indices[4], 2);
  EXPECT_EQ(out.indices[5], 3);
}

TEST(PlyParserTest, PlyExtremeFloatValues) {
  const std::string_view plyContent = R"(ply
format ascii 1.0
element vertex 2
property float x
property float y
property float z
element face 1
property list uchar int vertex_indices
end_header
1e38 0 0
-1e38 0 0
3 0 1 1
)";
  createTestFile("test_data/extreme_float.ply", plyContent);
  SSerializer::MeshParser parser;
  SSerializer::MeshData out;
  EXPECT_TRUE(parser.parse("test_data/extreme_float.ply", out));
  EXPECT_FLOAT_EQ(out.vertices[0].pos.x, 1e38f);
  EXPECT_FLOAT_EQ(out.vertices[1].pos.x, -1e38f);
}

TEST(PlyParserTest, LargePly) {
  const int NUM = 100000;
  std::ofstream file("test_data/large.ply");
  file << "ply\nformat ascii 1.0\nelement vertex " << NUM << "\n"
    << "property float x\nproperty float y\nproperty float z\n"
    << "element face " << (NUM - 2) << "\nproperty list uchar int vertex_indices\nend_header\n";
  for (int i = 0; i < NUM; i++) file << i << " 0 0\n";
  for (int i = 0; i < NUM - 2; i++) file << "3 " << i << " " << (i + 1) << " " << (i + 2) << "\n";
  file.close();

  SSerializer::MeshParser parser;
  SSerializer::MeshData out;
  EXPECT_TRUE(parser.parse("test_data/large.ply", out));
  EXPECT_EQ(out.numVertices, NUM);
  EXPECT_EQ(out.numTriangles, NUM - 2);
}

TEST(PlyParserTest, PartialNormalsNotCounted) {
  const std::string_view ply = R"(ply
format ascii 1.0
element vertex 1
property float x
property float y
property float z
property float nx
property float ny
element face 1
property list uchar int vertex_indices
end_header
0 0 0 0 0
3 0 0 0
)";
  createTestFile("test_data/partial_normals.ply", ply);
  SSerializer::MeshParser parser;
  SSerializer::MeshData out;
  EXPECT_TRUE(parser.parse("test_data/partial_normals.ply", out));
  EXPECT_FALSE(out.hasNormals);
}

TEST(PlyParserTest, AcceptAlternatePropertyNames) {
  const std::string_view ply = R"(ply
format ascii 1.0
element vertex 1
property float x
property float y
property float z
property float normal_x
property float normal_y
property float normal_z
property float texture_u
property float texture_v
element face 1
property list uchar int vertex_indices
end_header
0 0 0 0 0 1 0.1 0.2
3 0 0 0
)";
  createTestFile("test_data/alt_names.ply", ply);
  SSerializer::MeshParser parser;
  SSerializer::MeshData out;
  EXPECT_TRUE(parser.parse("test_data/alt_names.ply", out));
  EXPECT_TRUE(out.hasNormals);
  EXPECT_TRUE(out.hasTexCoords);
}

TEST(PlyParserTest, FloatColourParsing) {
  const std::string_view ply = R"(ply
format ascii 1.0
element vertex 1
property float x
property float y
property float z
property float red
property float green
property float blue
element face 1
property list uchar int vertex_indices
end_header
0 0 0 0.5 0.25 1.0
3 0 0 0
)";
  createTestFile("test_data/float_colours.ply", ply);
  SSerializer::MeshParser parser;
  SSerializer::MeshData out;
  EXPECT_TRUE(parser.parse("test_data/float_colours.ply", out));
  EXPECT_TRUE(out.hasColours);
  EXPECT_FLOAT_EQ(out.vertices[0].col.x, 0.5f);
  EXPECT_FLOAT_EQ(out.vertices[0].col.y, 0.25f);
  EXPECT_FLOAT_EQ(out.vertices[0].col.z, 1.0f);
}

TEST(PlyParserTest, UcharColourWithAlpha) {
  const std::string_view ply = R"(ply
format ascii 1.0
element vertex 1
property float x
property float y
property float z
property uchar red
property uchar green
property uchar blue
property uchar alpha
element face 1
property list uchar int vertex_indices
end_header
0 0 0 10 20 30 128
3 0 0 0
)";
  createTestFile("test_data/uchar_alpha.ply", ply);
  SSerializer::MeshParser parser;
  SSerializer::MeshData out;
  EXPECT_TRUE(parser.parse("test_data/uchar_alpha.ply", out));
  EXPECT_TRUE(out.hasColours);
  EXPECT_FLOAT_EQ(out.vertices[0].col.w, 128.0f / 255.0f);
}

TEST(PlyParserTest, PlyExtraVertexProperties) {
  const std::string_view plyContent = R"(ply
format ascii 1.0
element vertex 2
property float x
property float y
property float z
property float nx
property float ny
property float nz
property float extra
element face 1
property list uchar int vertex_indices
end_header
0 0 0 0 0 1 99
1 0 0 0 0 1 88
3 0 1 0
)";
  createTestFile("test_data/extra_props.ply", plyContent);
  SSerializer::MeshParser parser;
  SSerializer::MeshData out;
  EXPECT_TRUE(parser.parse("test_data/extra_props.ply", out));
  EXPECT_EQ(out.numVertices, 2);
}

TEST(PlyParserTest, PlyVertexPositionBounds) {
  const std::string_view plyContent = R"(ply
format ascii 1.0
element vertex 3
property float x
property float y
property float z
element face 1
property list uchar int vertex_indices
end_header
0.0 -5.0 0.0
1.0 0.0 0.0
0.0 10.0 0.0
3 0 1 2
)";
  createTestFile("test_data/bounds.ply", plyContent);
  Starlet::Serializer::MeshParser parser;
  Starlet::Serializer::MeshData out;
  EXPECT_TRUE(parser.parse("test_data/bounds.ply", out));
  EXPECT_FLOAT_EQ(out.minY, -5.0f);
  EXPECT_FLOAT_EQ(out.maxY, 10.0f);
}

TEST(PlyParserTest, HandlesWhitespaceBetweenSections) {
  const std::string_view ply = R"(ply
format ascii 1.0
element vertex 2
property float x
property float y
property float z
element face 1
property list uchar int vertex_indices
end_header

0 0 0

1 0 0

3 0 1 0
)";
  createTestFile("test_data/whitespace.ply", ply);
  SSerializer::MeshParser parser;
  SSerializer::MeshData out;
  EXPECT_TRUE(parser.parse("test_data/whitespace.ply", out));
}

TEST(PlyParserTest, SupportsComments) {
  const std::string_view ply = R"(ply
format ascii 1.0
comment created by test
element vertex 1
property float x
property float y
property float z
element face 1
property list uchar int vertex_indices
end_header
0 0 0
3 0 0 0
)";
  createTestFile("test_data/comments.ply", ply);
  SSerializer::MeshParser parser;
  SSerializer::MeshData out;
  EXPECT_TRUE(parser.parse("test_data/comments.ply", out));
}

TEST(PlyParserTest, ExtraFacePropertiesIgnored) {
  const std::string_view ply = R"(ply
format ascii 1.0
element vertex 3
property float x
property float y
property float z
element face 1
property list uchar int vertex_indices
property float something
end_header
0 0 0
1 0 0
0 1 0
3 0 1 2 42.0
)";
  createTestFile("test_data/extra_face_props.ply", ply);
  SSerializer::MeshParser parser;
  SSerializer::MeshData out;
  EXPECT_TRUE(parser.parse("test_data/extra_face_props.ply", out));
}


// Error tests
TEST(PlyParserTest, PlyEmpty) {
  createTestFile("test_data/empty.ply", "");
  SSerializer::MeshParser parser;
  SSerializer::MeshData out;
  EXPECT_FALSE(parser.parse("test_data/empty.ply", out));
}

TEST(PlyParserTest, PlyNoHeader) {
  createTestFile("test_data/noheader.ply", "0.0 0.0 0.0\n");
  SSerializer::MeshParser parser;
  SSerializer::MeshData out;
  EXPECT_FALSE(parser.parse("test_data/noheader.ply", out));
}

TEST(PlyParserTest, PlyZeroVertices) {
  const std::string_view plyContent = R"(ply
format ascii 1.0
element vertex 0
element face 0
end_header
)";
  createTestFile("test_data/zero.ply", plyContent);
  Starlet::Serializer::MeshParser parser;
  Starlet::Serializer::MeshData out;
  EXPECT_FALSE(parser.parse("test_data/zero.ply", out));
}

TEST(PlyParserTest, PlyZeroFaces) {
  const std::string_view plyContent = R"(ply
format ascii 1.0
element vertex 3
property float x
property float y
property float z
element face 0
end_header
0.0 0.0 0.0
1.0 0.0 0
0 1 0
)";
  createTestFile("test_data/nofaces.ply", plyContent);
  Starlet::Serializer::MeshParser parser;
  Starlet::Serializer::MeshData out;
  EXPECT_FALSE(parser.parse("test_data/nofaces.ply", out));
}

TEST(PlyParserTest, PlyInvalidFloat) {
  const std::string_view plyContent = R"(ply
format ascii 1.0
element vertex 1
property float x
property float y
property float z
element face 0
end_header
a b c
)";
  createTestFile("test_data/invalid_float.ply", plyContent);
  SSerializer::MeshParser parser;
  SSerializer::MeshData out;
  EXPECT_FALSE(parser.parse("test_data/invalid_float.ply", out));
}

TEST(PlyParserTest, PlyMissingEndHeader) {
  const std::string_view plyContent = R"(ply
format ascii 1.0
element vertex 3
property float x
property float y
property float z
0.0 0.0 0.0
)";
  createTestFile("test_data/noend.ply", plyContent);
  Starlet::Serializer::MeshParser parser;
  Starlet::Serializer::MeshData out;
  EXPECT_FALSE(parser.parse("test_data/noend.ply", out));
}

TEST(PlyParserTest, PlyNonexistentFile) {
  Starlet::Serializer::MeshParser parser;
  Starlet::Serializer::MeshData out;
  EXPECT_FALSE(parser.parse("test_data/nonexistent.ply", out));
}

TEST(PlyParserTest, PlyFewerVerticesThanDeclared) {
  const std::string_view ply = R"(ply
format ascii 1.0
element vertex 3
property float x
property float y
property float z
element face 1
property list uchar int vertex_indices
end_header
0 0 0
1 0 0
)";
  createTestFile("test_data/fewer_verts.ply", ply);
  SSerializer::MeshParser parser;
  SSerializer::MeshData out;
  EXPECT_FALSE(parser.parse("test_data/fewer_verts.ply", out));
}

TEST(PlyParserTest, PlyFewerFacesThanDeclared) {
  const std::string_view ply = R"(ply
format ascii 1.0
element vertex 3
property float x
property float y
property float z
element face 2
property list uchar int vertex_indices
end_header
0 0 0
1 0 0
0 1 0
3 0 1 2
)";
  createTestFile("test_data/fewer_faces.ply", ply);
  SSerializer::MeshParser parser;
  SSerializer::MeshData out;
  EXPECT_FALSE(parser.parse("test_data/fewer_faces.ply", out));
}

TEST(PlyParserTest, PlyInvalidVertexIndex) {
  const std::string_view ply = R"(ply
format ascii 1.0
element vertex 2
property float x
property float y
property float z
element face 1
property list uchar int vertex_indices
end_header
0 0 0
1 0 0
3 0 1 5
)";
  createTestFile("test_data/invalid_index.ply", ply);
  SSerializer::MeshParser parser;
  SSerializer::MeshData out;
  EXPECT_FALSE(parser.parse("test_data/invalid_index.ply", out));
}

TEST(PlyParserTest, RejectNonTriangleFace) {
  const std::string_view ply = R"(ply
format ascii 1.0
element vertex 4
property float x
property float y
property float z
element face 1
property list uchar int vertex_indices
end_header
0 0 0
1 0 0
1 1 0
0 1 0
4 0 1 2 3
)";
  createTestFile("test_data/quad_face.ply", ply);
  SSerializer::MeshParser parser;
  SSerializer::MeshData out;
  EXPECT_FALSE(parser.parse("test_data/quad_face.ply", out));
}

TEST(PlyParserTest, RejectFaceWithZeroIndices) {
  const std::string_view ply = R"(ply
format ascii 1.0
element vertex 1
property float x
property float y
property float z
element face 1
property list uchar int vertex_indices
end_header
0 0 0
0
)";
  createTestFile("test_data/zero_face.ply", ply);
  SSerializer::MeshParser parser;
  SSerializer::MeshData out;
  EXPECT_FALSE(parser.parse("test_data/zero_face.ply", out));
}

TEST(PlyParserTest, RejectFaceWithOneIndex) {
  const std::string_view ply = R"(ply
format ascii 1.0
element vertex 1
property float x
property float y
property float z
element face 1
property list uchar int vertex_indices
end_header
0 0 0
1 0
)";
  createTestFile("test_data/one_face.ply", ply);
  SSerializer::MeshParser parser;
  SSerializer::MeshData out;
  EXPECT_FALSE(parser.parse("test_data/one_face.ply", out));
}

TEST(PlyParserTest, RejectIncompleteVertexData) {
  const std::string_view ply = R"(ply
format ascii 1.0
element vertex 2
property float x
property float y
property float z
element face 1
property list uchar int vertex_indices
end_header
0 0 0
1 0
)";
  createTestFile("test_data/incomplete_vertex.ply", ply);
  SSerializer::MeshParser parser;
  SSerializer::MeshData out;
  EXPECT_FALSE(parser.parse("test_data/incomplete_vertex.ply", out));
}