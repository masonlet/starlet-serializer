#include "../test_helpers.hpp"

class ObjParserTest : public MeshParserTest {};


TEST_F(ObjParserTest, CommentOnly) {
  createTestFile("test_data/comments.obj", "# Comment\n# Another comment\n");
  EXPECT_TRUE(parser.parse("test_data/comments.obj", out));
}

TEST_F(ObjParserTest, SingleVertex) {
  createTestFile("test_data/single_vertex.obj", "v 1.0 2.0 3.0\n");
  EXPECT_TRUE(parser.parse("test_data/single_vertex.obj", out));
  EXPECT_EQ(out.numVertices, 1);
  EXPECT_FLOAT_EQ(out.vertices[0].pos.x, 1.0f);
  EXPECT_FLOAT_EQ(out.vertices[0].pos.y, 2.0f);
  EXPECT_FLOAT_EQ(out.vertices[0].pos.z, 3.0f);
}

TEST_F(ObjParserTest, MultipleVertices) {
  createTestFile("test_data/vertices.obj", "v 0.0 0.0 0.0\nv 1.0 0.0 0.0\nv 0.0 1.0 0.0\n");
  EXPECT_TRUE(parser.parse("test_data/vertices.obj", out));
  EXPECT_EQ(out.numVertices, 3);
  EXPECT_FLOAT_EQ(out.vertices[2].pos.y, 1.0f);
}

TEST_F(ObjParserTest, VertexWithW) {
  createTestFile("test_data/vertex_w.obj", "v 1.0 2.0 3.0 0.5\n");
  EXPECT_TRUE(parser.parse("test_data/vertex_w.obj", out));
  EXPECT_EQ(out.numVertices, 1);
  EXPECT_FLOAT_EQ(out.vertices[0].pos.z, 3.0f);
}

TEST_F(ObjParserTest, VerticesWithComments) {
  createTestFile("test_data/vert_comments.obj", "# Header\nv 1.0 0.0 0.0\n# Mid comment\nv 0.0 1.0 0.0\n");
  EXPECT_TRUE(parser.parse("test_data/vert_comments.obj", out));
  EXPECT_EQ(out.numVertices, 2);
}

TEST_F(ObjParserTest, SingleTriangle) {
  createTestFile("test_data/triangle.obj", "v 0 0 0\nv 1 0 0\nv 0 1 0\nf 1 2 3\n");
  expectValidParse("test_data/triangle.obj", 3, 1);
  EXPECT_EQ(out.indices[0], 0);
  EXPECT_EQ(out.indices[1], 1);
  EXPECT_EQ(out.indices[2], 2);
}

TEST_F(ObjParserTest, QuadTriangulation) {
  createTestFile("test_data/quad.obj", "v 0 0 0\nv 1 0 0\nv 1 1 0\nv 0 1 0\nf 1 2 3 4\n");
  expectValidParse("test_data/quad.obj", 4, 2);
  EXPECT_EQ(out.indices[0], 0);
  EXPECT_EQ(out.indices[1], 1);
  EXPECT_EQ(out.indices[2], 2);
  EXPECT_EQ(out.indices[3], 0);
  EXPECT_EQ(out.indices[4], 2);
  EXPECT_EQ(out.indices[5], 3);
}

TEST_F(ObjParserTest, NegativeIndex) {
  createTestFile("test_data/negative.obj", "v 0 0 0\nv 1 0 0\nv 0 1 0\nf -3 -2 -1\n");
  expectValidParse("test_data/negative.obj", 3, 1);
}

TEST_F(ObjParserTest, TextureCoordinate) {
  createTestFile("test_data/texcoord.obj", "vt 0.5 0.5\n");
  EXPECT_TRUE(parser.parse("test_data/texcoord.obj", out));
  //TODO: Verify correct
}

TEST_F(ObjParserTest, TextureCoordinateWithW) {
  createTestFile("test_data/texcoord_w.obj", "vt 0.5 0.5 1.0\n");
  EXPECT_TRUE(parser.parse("test_data/texcoord_w.obj", out));
  //TODO: Verify correct
}

TEST_F(ObjParserTest, Normal) {
  createTestFile("test_data/normal.obj", "vn 0.0 1.0 0.0\n");
  EXPECT_TRUE(parser.parse("test_data/normal.obj", out));
  //TODO: Verify correct
}

TEST_F(ObjParserTest, MixedVertexData) {
  createTestFile("test_data/mixed.obj", "v 0 0 0\nvt 0.0 0.0\nvn 0 1 0\nv 1 0 0\n");
  EXPECT_TRUE(parser.parse("test_data/mixed.obj", out));
  EXPECT_EQ(out.numVertices, 2);
  //TODO: Verify correct
}



// Error tests
TEST_F(ObjParserTest, EmptyFile) {
  createTestFile("test_data/empty.obj", "");

  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/empty.obj", out));
  std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("File is empty"), std::string::npos);
}

TEST_F(ObjParserTest, VertexMissingZ) {
  createTestFile("test_data/missing_z.obj", "v 1.0 2.0\n");
  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/missing_z.obj", out));
  std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Failed to parse vertex position at vertex 0"), std::string::npos);
}

TEST_F(ObjParserTest, VertexInvalidFloat) {
  createTestFile("test_data/invalid_float.obj", "v 1.0 abc 3.0\n");
  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/invalid_float.obj", out));
  std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Failed to parse vertex position"), std::string::npos);
}

TEST_F(ObjParserTest, FaceIndexZero) {
  createTestFile("test_data/zero_index.obj", "v 0 0 0\nf 0 1 2\n");
  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/zero_index.obj", out));
  std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Face index cannot be 0"), std::string::npos);
}

TEST_F(ObjParserTest, FaceIndexOutOfBounds) {
  createTestFile("test_data/bounds.obj", "v 0 0 0\nf 1 2 3\n");
  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/bounds.obj", out));
  std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Face index out of bounds"), std::string::npos);
}

TEST_F(ObjParserTest, FaceTooFewVertices) {
  createTestFile("test_data/few_verts.obj", "v 0 0 0\nv 1 0 0\nf 1 2\n");
  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/few_verts.obj", out));
  std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Face has fewer than 3 vertices"), std::string::npos);
}

TEST_F(ObjParserTest, TextureCoordMissingV) {
  createTestFile("test_data/tc_missing.obj", "vt 0.5\n");
  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/tc_missing.obj", out));
  std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Failed to parse texture coordinate at texCoord 0"), std::string::npos);
}

TEST_F(ObjParserTest, TextureCoordInvalid) {
  createTestFile("test_data/tc_invalid.obj", "vt abc 0.5\n");
  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/tc_invalid.obj", out));
  std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Failed to parse texture coordinate"), std::string::npos);
}

TEST_F(ObjParserTest, NormalMissingZ) {
  createTestFile("test_data/norm_missing.obj", "vn 0.0 1.0\n");
  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/norm_missing.obj", out));
  std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Failed to parse normal at normal 0"), std::string::npos);
}

TEST_F(ObjParserTest, NormalInvalid) {
  createTestFile("test_data/norm_invalid.obj", "vn 0.0 abc 1.0\n");
  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/norm_invalid.obj", out));
  std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Failed to parse normal"), std::string::npos);
}