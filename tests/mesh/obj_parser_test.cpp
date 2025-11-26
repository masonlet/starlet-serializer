#include "../test_helpers.hpp"

class ObjParserTest : public MeshParserTest {};


TEST_F(ObjParserTest, CommentOnly) {
  createTestFile("test_data/comments.obj", "# Comment\n# Another comment\n");
  EXPECT_TRUE(parser.parse("test_data/comments.obj", out));
}


// Error tests
TEST_F(ObjParserTest, EmptyFile) {
  createTestFile("test_data/empty.obj", "");

  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/empty.obj", out));
  std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("File is empty"), std::string::npos);
}