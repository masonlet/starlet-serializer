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