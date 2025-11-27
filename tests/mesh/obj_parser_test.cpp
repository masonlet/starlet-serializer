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

TEST_F(ObjParserTest, VertexWithColour) {
  createTestFile("test_data/vertex_color.obj",
    "v 1.0 2.0 3.0 0.1 0.2 0.3 0.4\n");
  EXPECT_TRUE(parser.parse("test_data/vertex_color.obj", out));
  EXPECT_EQ(out.numVertices, 1);
  EXPECT_FLOAT_EQ(out.vertices[0].pos.x, 1.0f);
  EXPECT_FLOAT_EQ(out.vertices[0].pos.y, 2.0f);
  EXPECT_FLOAT_EQ(out.vertices[0].pos.z, 3.0f);
  EXPECT_FLOAT_EQ(out.vertices[0].col.x, 0.1f);
  EXPECT_FLOAT_EQ(out.vertices[0].col.y, 0.2f);
  EXPECT_FLOAT_EQ(out.vertices[0].col.z, 0.3f);
  EXPECT_FLOAT_EQ(out.vertices[0].col.w, 0.4f);
}

TEST_F(ObjParserTest, VertexColourOptionalAlpha) {
  createTestFile("test_data/vertex_color_alpha.obj",
    "v 1.0 2.0 3.0 0.5 0.5 0.5\n"); 
  EXPECT_TRUE(parser.parse("test_data/vertex_color_alpha.obj", out));
  EXPECT_EQ(out.numVertices, 1);
  EXPECT_FLOAT_EQ(out.vertices[0].col.w, 1.0f);
}

TEST_F(ObjParserTest, VertexColourAndFace) {
  createTestFile("test_data/color_face.obj",
    "v 0 0 0 1 0 0\nv 1 0 0 0 1 0\nv 0 1 0 0 0 1\n"
    "f 1 2 3\n");
  EXPECT_TRUE(parser.parse("test_data/color_face.obj", out));
  EXPECT_EQ(out.numTriangles, 1);
  EXPECT_FLOAT_EQ(out.vertices[0].col.x, 1.0f);
  EXPECT_FLOAT_EQ(out.vertices[1].col.y, 1.0f);
  EXPECT_FLOAT_EQ(out.vertices[2].col.z, 1.0f);
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

TEST_F(ObjParserTest, Pentagon) {
  createTestFile("test_data/pentagon.obj",
    "v 0 0 0\nv 1 0 0\nv 1.5 0.5 0\nv 1 1 0\nv 0 1 0\nf 1 2 3 4 5\n");
  expectValidParse("test_data/pentagon.obj", 5, 3);
  EXPECT_EQ(out.indices.size(), 9);
}

TEST_F(ObjParserTest, Hexagon) {
  createTestFile("test_data/hexagon.obj",
    "v 0 0 0\nv 1 0 0\nv 1.5 0.5 0\nv 1.5 1.5 0\nv 1 2 0\nv 0 2 0\nf 1 2 3 4 5 6\n");
  expectValidParse("test_data/hexagon.obj", 6, 4);
  EXPECT_EQ(out.indices.size(), 12);
}

TEST_F(ObjParserTest, LargePolygon) {
  std::string content;
  for (int i = 0; i < 10; i++)
    content += "v " + std::to_string(i) + " 0 0\n";
  content += "f 1 2 3 4 5 6 7 8 9 10\n";
  createTestFile("test_data/large_poly.obj", content);
  expectValidParse("test_data/large_poly.obj", 10, 8);
}

TEST_F(ObjParserTest, DegenerateFace) {
  createTestFile("test_data/degen.obj", "v 0 0 0\nv 1 0 0\nf 1 1 2\n");
  expectValidParse("test_data/degen.obj", 2, 1);
}



TEST_F(ObjParserTest, TextureCoordinate) {
  createTestFile("test_data/texcoord.obj", "v 0 0 0\nv 1 0 0\nv 0 1 0\nvt 0.5 0.5\nf 1/1 2/1 3/1\n");
  EXPECT_TRUE(parser.parse("test_data/texcoord.obj", out));
  EXPECT_TRUE(out.hasTexCoords); 
  EXPECT_EQ(out.numVertices, 3);
}

TEST_F(ObjParserTest, TextureCoordinateWithW) {
  createTestFile("test_data/texcoord_w.obj", "v 0 0 0\nv 1 0 0\nv 0 1 0\nvt 0.5 0.5 1.0\nf 1/1 2/1 3/1\n\n");
  EXPECT_TRUE(parser.parse("test_data/texcoord_w.obj", out));
  EXPECT_TRUE(out.hasTexCoords);
  EXPECT_EQ(out.numVertices, 3);
}

TEST_F(ObjParserTest, TriangleWithTexCoords) {
  createTestFile("test_data/tri_tc.obj",
    "v 0 0 0\nv 1 0 0\nv 0 1 0\nvt 0 0\nvt 1 0\nvt 0 1\nf 1/1 2/2 3/3\n");
  expectValidParse("test_data/tri_tc.obj", 3, 1);
  EXPECT_TRUE(out.hasTexCoords);
  EXPECT_FALSE(out.hasNormals);
  EXPECT_FLOAT_EQ(out.vertices[0].texCoord.x, 0.0f);
  EXPECT_FLOAT_EQ(out.vertices[0].texCoord.y, 0.0f);
  EXPECT_FLOAT_EQ(out.vertices[2].texCoord.x, 0.0f);
  EXPECT_FLOAT_EQ(out.vertices[2].texCoord.y, 1.0f);
}



TEST_F(ObjParserTest, Normal) {
  createTestFile("test_data/normal.obj", "v 0 0 0\nv 1 0 0\nv 0 1 0\nvn 0.0 1.0 0.0\nf 1//1 2//1 3//1\n");
  EXPECT_TRUE(parser.parse("test_data/normal.obj", out));
  EXPECT_TRUE(out.hasNormals);
  EXPECT_EQ(out.numVertices, 3);
}

TEST_F(ObjParserTest, TriangleWithNormals) {
  createTestFile("test_data/tri_norm.obj",
    "v 0 0 0\nv 1 0 0\nv 0 1 0\nvn 0 0 1\nf 1//1 2//1 3//1\n");
  expectValidParse("test_data/tri_norm.obj", 3, 1);
  EXPECT_FALSE(out.hasTexCoords);
  EXPECT_TRUE(out.hasNormals);
  EXPECT_FLOAT_EQ(out.vertices[0].norm.z, 1.0f);
  EXPECT_FLOAT_EQ(out.vertices[2].norm.z, 1.0f);
}

TEST_F(ObjParserTest, FaceMissingTexCoordIndex) {
  createTestFile("test_data/missing_tc_idx.obj",
    "v 0 0 0\nv 1 0 0\nv 0 1 0\nvn 0 0 1\nf 1//1 2//1 3//1\n");
  expectValidParse("test_data/missing_tc_idx.obj", 3, 1);
  EXPECT_FALSE(out.hasTexCoords);
  EXPECT_TRUE(out.hasNormals);
}



TEST_F(ObjParserTest, TriangleWithTexCoordsAndNormals) {
  createTestFile("test_data/tri_full.obj",
    "v 0 0 0\nv 1 0 0\nv 0 1 0\nvt 0 0\nvt 1 0\nvt 0 1\nvn 0 0 1\nf 1/1/1 2/2/1 3/3/1\n");
  expectValidParse("test_data/tri_full.obj", 3, 1);
  EXPECT_TRUE(out.hasTexCoords);
  EXPECT_TRUE(out.hasNormals);
  EXPECT_FLOAT_EQ(out.vertices[1].texCoord.x, 1.0f);
  EXPECT_FLOAT_EQ(out.vertices[1].norm.z, 1.0f);
}

TEST_F(ObjParserTest, MixedVertexData) {
  createTestFile("test_data/mixed.obj", "v 0 0 0\nvt 0.0 0.0\nvn 0 1 0\nv 1 0 0\nv 0 1 0\nf 1/1/1 2/1/1 3/1/1\n");
  EXPECT_TRUE(parser.parse("test_data/mixed.obj", out));
  EXPECT_EQ(out.numVertices, 3);
  EXPECT_TRUE(out.hasTexCoords);
  EXPECT_TRUE(out.hasNormals);
}

TEST_F(ObjParserTest, MixedFaceSeparators) {
  createTestFile("test_data/mixed_sep.obj",
    "v 0 0 0\nv 1 0 0\nv 0 1 0\nvt 0 0\nvt 1 0\nvn 0 0 1\nf 1//1 2/2 3\n");
  expectValidParse("test_data/mixed_sep.obj", 3, 1);
  EXPECT_TRUE(out.hasNormals);
  EXPECT_TRUE(out.hasTexCoords);
}

TEST_F(ObjParserTest, FaceWithOnlyPositions) {
  createTestFile("test_data/pos_only.obj",
    "v 0 0 0\nv 1 0 0\nv 0 1 0\nvt 0.5 0.5\nvn 0 0 1\nf 1 2 3\n");
  expectValidParse("test_data/pos_only.obj", 3, 1);
  EXPECT_FALSE(out.hasTexCoords);
  EXPECT_FALSE(out.hasNormals);
}

TEST_F(ObjParserTest, FaceUsingOnlySubsetOfDefinedData) {
  createTestFile("test_data/subset_data.obj",
    "v 0 0 0\nv 1 0 0\nv 0 1 0\nvt 0 0\nvn 0 0 1\nf 1 2 3\n");
  expectValidParse("test_data/subset_data.obj", 3, 1);
  EXPECT_FALSE(out.hasTexCoords);
  EXPECT_FALSE(out.hasNormals);
}



TEST_F(ObjParserTest, NegativeIndex) {
  createTestFile("test_data/negative.obj", "v 0 0 0\nv 1 0 0\nv 0 1 0\nf -3 -2 -1\n");
  expectValidParse("test_data/negative.obj", 3, 1);
}

TEST_F(ObjParserTest, NegativeIndexForPositionOnlyFace) {
  createTestFile("test_data/neg_pos_only.obj",
    "v 0 0 0\nv 1 0 0\nv 0 1 0\nf 1 2 -1\n");
  expectValidParse("test_data/neg_pos_only.obj", 3, 1);
  EXPECT_EQ(out.indices[2], 2);
}

TEST_F(ObjParserTest, NegativeTexCoordIndex) {
  createTestFile("test_data/neg_tc.obj",
    "v 0 0 0\nv 1 0 0\nv 0 1 0\nvt 0 0\nvt 1 0\nf 1/-2 1/-1 1/-2\n");
  expectValidParse("test_data/neg_tc.obj", 2, 1);
  EXPECT_TRUE(out.hasTexCoords);
}

TEST_F(ObjParserTest, NegativeNormalIndex) {
  createTestFile("test_data/neg_norm.obj",
    "v 0 0 0\nv 1 0 0\nv 0 1 0\nvn 0 0 1\nf 1//-1 2//-1 3//-1\n");
  expectValidParse("test_data/neg_norm.obj", 3, 1);
  EXPECT_TRUE(out.hasNormals);
}

TEST_F(ObjParserTest, FaceNegativeIndexMixedRefs) {
  createTestFile("test_data/mixed_neg_idx.obj",
    "v 0 0 0\nv 1 0 0\nv 0 1 0\nvt 0 0\nvt 1 0\nf 1/1 2/-2 3/1\n");
  expectValidParse("test_data/mixed_neg_idx.obj", 3, 1);
  EXPECT_TRUE(out.hasTexCoords);
}



TEST_F(ObjParserTest, VertexDeduplication) {
  createTestFile("test_data/dedup.obj",
    "v 0 0 0\nvt 0 0\nvt 1 0\nf 1/1 1/2 1/1\n");
  EXPECT_TRUE(parser.parse("test_data/dedup.obj", out));
  EXPECT_EQ(out.numVertices, 2);
  EXPECT_EQ(out.numTriangles, 1);
  EXPECT_EQ(out.indices[0], 0);
  EXPECT_EQ(out.indices[1], 1);
  EXPECT_EQ(out.indices[2], 0); 
}

TEST_F(ObjParserTest, DeduplicationWithDifferentNormals) {
  createTestFile("test_data/dedup_diff_norms.obj",
    "v 0 0 0\nvt 0 0\nvn 0 0 1\nvn 0 1 0\nf 1/1/1 1/1/2 1/1/1\n");
  EXPECT_TRUE(parser.parse("test_data/dedup_diff_norms.obj", out));
  EXPECT_EQ(out.numVertices, 2); 
  EXPECT_EQ(out.numTriangles, 1);
  EXPECT_EQ(out.indices[0], 0);
  EXPECT_EQ(out.indices[1], 1);
  EXPECT_EQ(out.indices[2], 0);
}

TEST_F(ObjParserTest, DeduplicationWithDifferentTexCoords) {
  createTestFile("test_data/dedup_diff_tex.obj",
    "v 0 0 0\nvt 0 0\nvt 1 1\nvn 0 0 1\nf 1/1/1 1/2/1 1/1/1\n");
  EXPECT_TRUE(parser.parse("test_data/dedup_diff_tex.obj", out));
  EXPECT_EQ(out.numVertices, 2); 
  EXPECT_EQ(out.numTriangles, 1);
  EXPECT_EQ(out.indices[0], 0);
  EXPECT_EQ(out.indices[1], 1);
  EXPECT_EQ(out.indices[2], 0);
}



TEST_F(ObjParserTest, ExtraWhitespace) {
  createTestFile("test_data/whitespace.obj", "v    1.0    2.0   3.0\nvt\t0.5\t0.5\nf   1   1   1\n");
  EXPECT_TRUE(parser.parse("test_data/whitespace.obj", out));
  EXPECT_EQ(out.numVertices, 1);
}

TEST_F(ObjParserTest, WindowsCRLF) {
  createTestFile("test_data/crlf.obj", "v 1 2 3\r\nvt 0.5 0.5\r\nf 1/1 1/1 1/1\r\n");
  EXPECT_TRUE(parser.parse("test_data/crlf.obj", out));
  EXPECT_EQ(out.numVertices, 1);
  EXPECT_TRUE(out.hasTexCoords);
}

TEST_F(ObjParserTest, LeadingTrailingBlankLines) {
  createTestFile("test_data/blanks.obj", "\n\nv 0 0 0\nv 1 0 0\nv 0 1 0\n\nf 1 2 3\n\n");
  expectValidParse("test_data/blanks.obj", 3, 1);
}

TEST_F(ObjParserTest, MultipleSpacesInFace) {
  createTestFile("test_data/multi_space.obj",
    "v 0 0 0\nv 1 0 0\nv 0 1 0\nvn 0 0 1\nf    1//1     2//1     3//1\n");
  expectValidParse("test_data/multi_space.obj", 3, 1);
}

TEST_F(ObjParserTest, CommentAfterFace) {
  createTestFile("test_data/face_comment.obj",
    "v 0 0 0\nv 1 0 0\nv 0 1 0\nvt 0 0\nvt 1 0\nvt 0 1\nvn 0 0 1\nf 1/1/1 2/2/1 3/3/1 # triangle\n");
  expectValidParse("test_data/face_comment.obj", 3, 1);
}

TEST_F(ObjParserTest, ScientificNotation) {
  createTestFile("test_data/scientific.obj", "v 1e-3 2E+2 -3e0\nvt 5e-1 5e-1\nf 1/1 1/1 1/1\n");
  EXPECT_TRUE(parser.parse("test_data/scientific.obj", out));
  EXPECT_FLOAT_EQ(out.vertices[0].pos.x, 0.001f);
  EXPECT_FLOAT_EQ(out.vertices[0].pos.y, 200.0f);
  EXPECT_FLOAT_EQ(out.vertices[0].pos.z, -3.0f);
}

TEST_F(ObjParserTest, TexCoordExtraComponents) {
  createTestFile("test_data/tc_extra.obj", "v 0 0 0\nvt 0.5 0.5 1.0 extra junk\nf 1/1 1/1 1/1\n");
  EXPECT_TRUE(parser.parse("test_data/tc_extra.obj", out));
  EXPECT_EQ(out.numVertices, 1);
}

TEST_F(ObjParserTest, InterleavedCommands) {
  createTestFile("test_data/interleaved.obj",
    "v 0 0 0\nvt 0 0\nv 1 0 0\nvn 0 0 1\nv 0 1 0\nf 1/1/1 2/1/1 3/1/1\n");
  expectValidParse("test_data/interleaved.obj", 3, 1);
  EXPECT_TRUE(out.hasTexCoords);
  EXPECT_TRUE(out.hasNormals);
}

TEST_F(ObjParserTest, UnknownCommands) {
  createTestFile("test_data/unknown.obj",
    "usemtl Material\ns off\ng groupName\nv 0 0 0\nv 1 0 0\nv 0 1 0\nf 1 2 3\n");
  expectValidParse("test_data/unknown.obj", 3, 1);
}

TEST_F(ObjParserTest, ObjectCommand) {
  createTestFile("test_data/object.obj", "o Cube\nv 0 0 0\nv 1 0 0\nv 0 1 0\nf 1 2 3\n");
  expectValidParse("test_data/object.obj", 3, 1);
}

TEST_F(ObjParserTest, LargeFile) {
  std::string content;
  for (int i = 0; i < 1000; i++)
    content += "v " + std::to_string(i) + " 0 0\n";

  for (int i = 0; i < 300; i++) {
    int v1 = i * 3 + 1;
    int v2 = i * 3 + 2;
    int v3 = i * 3 + 3;
    if (v3 <= 1000) 
      content += "f " + std::to_string(v1) + " " + std::to_string(v2) + " " + std::to_string(v3) + "\n";
  }

  createTestFile("test_data/large.obj", content);
  EXPECT_TRUE(parser.parse("test_data/large.obj", out));
  EXPECT_EQ(out.numVertices, 900);  
  EXPECT_EQ(out.numTriangles, 300);
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

TEST_F(ObjParserTest, VeryLargeIndex) {
  createTestFile("test_data/huge_idx.obj", "v 0 0 0\nf 9999999 1 2\n");
  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/huge_idx.obj", out));
  std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("out of bounds"), std::string::npos);
}

TEST_F(ObjParserTest, FaceTooFewVertices) {
  createTestFile("test_data/few_verts.obj", "v 0 0 0\nv 1 0 0\nf 1 2\n");
  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/few_verts.obj", out));
  std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Face has fewer than 3 vertices"), std::string::npos);
}



TEST_F(ObjParserTest, TrailingSlashInFace) {
  createTestFile("test_data/trail_slash.obj",
    "v 0 0 0\nv 1 0 0\nvt 0.5 0.5\nf 1/ 2/1\n");

  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/trail_slash.obj", out));
  std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("index"), std::string::npos);
}

TEST_F(ObjParserTest, FaceMissingNormalIndex) {
  createTestFile("test_data/missing_norm_idx.obj",
    "v 0 0 0\nv 1 0 0\nv 0 1 0\nvt 0 0\nvt 1 0\nf 1/1 2/2 3/\n");

  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/missing_norm_idx.obj", out));
  std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Expected texture coordinate index"), std::string::npos);
}
 
TEST_F(ObjParserTest, BareSlashMalformed) {
  createTestFile("test_data/bare_slash.obj",
    "v 0 0 0\nv 1 0 0\nv 0 1 0\nvn 0 0 1\nf 1// 2/3/ 3//\n");

  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/bare_slash.obj", out));
  std::string output = testing::internal::GetCapturedStderr();
  EXPECT_FALSE(output.empty());
}

TEST_F(ObjParserTest, FaceIndexMissingIndexAfterFirstSlash) {
  createTestFile("test_data/empty_after_slash.obj",
    "v 0 0 0\nvn 0 0 1\nf 1/ /1\n");

  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.parse("test_data/empty_after_slash.obj", out));
  std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Expected texture coordinate index"), std::string::npos);
}