#include "../test_helpers.hpp"

class PlyParserTest : public MeshParserTest {};

// Valid PLY parsing tests
TEST_F(PlyParserTest, ValidPlyMinimal) {
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
  expectValidParse("test_data/minimal.ply", 3, 1);
  EXPECT_FALSE(out.hasNormals);
  EXPECT_FALSE(out.hasColours);
  EXPECT_FALSE(out.hasTexCoords);
}

TEST_F(PlyParserTest, ValidPlyWithNormals) {
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
  expectValidParse("test_data/normals.ply", 3, 1);
  EXPECT_TRUE(out.hasNormals);
  EXPECT_FALSE(out.hasColours);
  EXPECT_FALSE(out.hasTexCoords);
}

TEST_F(PlyParserTest, ValidPlyWithColours) {
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
  expectValidParse("test_data/colours.ply", 3, 1);
  EXPECT_FALSE(out.hasNormals);
  EXPECT_TRUE(out.hasColours);
  EXPECT_FLOAT_EQ(out.vertices[0].col.x, 1.0f);
  EXPECT_FLOAT_EQ(out.vertices[0].col.y, 0.0f);
  EXPECT_FLOAT_EQ(out.vertices[0].col.z, 0.0f);
}

TEST_F(PlyParserTest, ValidPlyWithTexCoords) {
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
  expectValidParse("test_data/texcoords.ply", 3, 1);
  EXPECT_FALSE(out.hasNormals);
  EXPECT_FALSE(out.hasColours);
  EXPECT_TRUE(out.hasTexCoords);
  EXPECT_FLOAT_EQ(out.vertices[2].texCoord.x, 0.5f);
  EXPECT_FLOAT_EQ(out.vertices[2].texCoord.y, 1.0f);
}

TEST_F(PlyParserTest, ValidPlyMultipleTriangles) {
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
  expectValidParse("test_data/quad.ply", 4, 2);
  EXPECT_EQ(out.indices[0], 0);
  EXPECT_EQ(out.indices[1], 1);
  EXPECT_EQ(out.indices[2], 2);
  EXPECT_EQ(out.indices[3], 0);
  EXPECT_EQ(out.indices[4], 2);
  EXPECT_EQ(out.indices[5], 3);
}

TEST_F(PlyParserTest, PlyExtremeFloatValues) {
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
  expectValidParse("test_data/extreme_float.ply", 2, 1);
  EXPECT_FLOAT_EQ(out.vertices[0].pos.x, 1e38f);
  EXPECT_FLOAT_EQ(out.vertices[1].pos.x, -1e38f);
}

TEST_F(PlyParserTest, LargePly) {
  const int NUM = 100000;
  std::ofstream file("test_data/large.ply");
  file << "ply\nformat ascii 1.0\nelement vertex " << NUM << "\n"
    << "property float x\nproperty float y\nproperty float z\n"
    << "element face " << (NUM - 2) << "\nproperty list uchar int vertex_indices\nend_header\n";
  for (int i = 0; i < NUM; i++) file << i << " 0 0\n";
  for (int i = 0; i < NUM - 2; i++) file << "3 " << i << " " << (i + 1) << " " << (i + 2) << "\n";
  file.close();

  expectValidParse("test_data/large.ply", NUM, NUM - 2);
}

TEST_F(PlyParserTest, PartialNormalsNotCounted) {
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
  expectValidParse("test_data/partial_normals.ply", 1, 1);
  EXPECT_FALSE(out.hasNormals);
}

TEST_F(PlyParserTest, AcceptAlternatePropertyNames) {
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
  expectValidParse("test_data/alt_names.ply", 1, 1);
  EXPECT_TRUE(out.hasNormals);
  EXPECT_TRUE(out.hasTexCoords);
}

TEST_F(PlyParserTest, FloatColourParsing) {
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
  expectValidParse("test_data/float_colours.ply", 1, 1);
  EXPECT_TRUE(out.hasColours);
  EXPECT_FLOAT_EQ(out.vertices[0].col.x, 0.5f);
  EXPECT_FLOAT_EQ(out.vertices[0].col.y, 0.25f);
  EXPECT_FLOAT_EQ(out.vertices[0].col.z, 1.0f);
}

TEST_F(PlyParserTest, UcharColourWithAlpha) {
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
  expectValidParse("test_data/uchar_alpha.ply", 1, 1);
  EXPECT_TRUE(out.hasColours);
  EXPECT_FLOAT_EQ(out.vertices[0].col.w, 128.0f / 255.0f);
}

TEST_F(PlyParserTest, PlyExtraVertexProperties) {
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
  expectValidParse("test_data/extra_props.ply", 2, 1);
}

TEST_F(PlyParserTest, PlyVertexPositionBounds) {
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
  expectValidParse("test_data/bounds.ply", 3, 1);
  EXPECT_FLOAT_EQ(out.minY, -5.0f);
  EXPECT_FLOAT_EQ(out.maxY, 10.0f);
}

TEST_F(PlyParserTest, HandlesWhitespaceBetweenSections) {
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
  expectValidParse("test_data/whitespace.ply", 2, 1);
}

TEST_F(PlyParserTest, SupportsComments) {
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
  expectValidParse("test_data/comments.ply", 1, 1);
}

TEST_F(PlyParserTest, ExtraFacePropertiesIgnored) {
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
  expectValidParse("test_data/extra_face_props.ply", 3, 1);
}


// Error tests
TEST_F(PlyParserTest, PlyEmpty) {
  createTestFile("test_data/empty.ply", "");

  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/empty.ply");
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("File is empty"), std::string::npos);
}

TEST_F(PlyParserTest, PlyNoHeader) {
  createTestFile("test_data/noheader.ply", "0.0 0.0 0.0\n");

  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/noheader.ply");
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Failed to parse header, 'end_header' not found"), std::string::npos);
}

TEST_F(PlyParserTest, PlyZeroVertices) {
  const std::string_view plyContent = R"(ply
format ascii 1.0
element vertex 0
element face 0
end_header
)";
  createTestFile("test_data/zero.ply", plyContent);

  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/zero.ply");
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Failed to parse header, no vertices/triangles declared"), std::string::npos);
}

TEST_F(PlyParserTest, PlyZeroFaces) {
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

  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/nofaces.ply");
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Failed to parse header, no vertices/triangles declared"), std::string::npos);
}

TEST_F(PlyParserTest, PlyInvalidFloat) {
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

  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/invalid_float.ply");
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Failed to parse header, no vertices/triangles declared"), std::string::npos);
}

TEST_F(PlyParserTest, PlyMissingEndHeader) {
  const std::string_view plyContent = R"(ply
format ascii 1.0
element vertex 3
property float x
property float y
property float z
0.0 0.0 0.0
)";
  createTestFile("test_data/noend.ply", plyContent);

  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/noend.ply");
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Failed to parse header, 'end_header' not found"), std::string::npos);
}

TEST_F(PlyParserTest, PlyNonexistentFile) {
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/nonexistent.ply");
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Failed to open file: test_data/nonexistent.ply"), std::string::npos);
}

TEST_F(PlyParserTest, PlyFewerVerticesThanDeclared) {
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

  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/fewer_verts.ply");
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Failed to parse vertex data"), std::string::npos);
}

TEST_F(PlyParserTest, PlyFewerFacesThanDeclared) {
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

  //testing::internal::CaptureStderr();
  expectInvalidParse("test_data/fewer_faces.ply");
  //const std::string output = testing::internal::GetCapturedStderr();
  //EXPECT_NE(output.find("Failed to parse face vertex count at triangle 1"), std::string::npos);
  //EXPECT_NE(output.find("Failed to parse face data"), std::string::npos);
}

TEST_F(PlyParserTest, PlyInvalidVertexIndex) {
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

  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/invalid_index.ply");
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Index out of bounds at triangle 0"), std::string::npos);
  EXPECT_NE(output.find("Failed to parse face data"), std::string::npos);
}

TEST_F(PlyParserTest, RejectNonTriangleFace) {
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

  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/quad_face.ply");
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Non-triangle face detected (vertex count: 4) at triangle 0"), std::string::npos);
  EXPECT_NE(output.find("Failed to parse face data"), std::string::npos);
}

TEST_F(PlyParserTest, RejectFaceWithZeroIndices) {
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

  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/zero_face.ply");
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Non-triangle face detected (vertex count: 0) at triangle 0"), std::string::npos);
  EXPECT_NE(output.find("Failed to parse face data"), std::string::npos);
}

TEST_F(PlyParserTest, RejectFaceWithOneIndex) {
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

  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/one_face.ply");
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Non-triangle face detected (vertex count: 1) at triangle 0"), std::string::npos);
  EXPECT_NE(output.find("Failed to parse face data"), std::string::npos);
}

TEST_F(PlyParserTest, RejectIncompleteVertexData) {
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

  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/incomplete_vertex.ply");
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Failed to parse position Z at vertex 1"), std::string::npos);
  EXPECT_NE(output.find("Failed to parse vertex data"), std::string::npos);
}