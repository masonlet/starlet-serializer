#include <gtest/gtest.h>

#include "starlet-serializer/parser/parser.hpp"
#include "test_helpers.hpp"

#include "starlet-math/vec2.hpp"
#include "starlet-math/vec3.hpp"
#include "starlet-math/vec4.hpp"

#include <cmath>
#include <fstream>
#include <filesystem>

namespace SSerializer = Starlet::Serializer;

class ParserTest : public ::testing::Test {
protected:
  SSerializer::Parser parser;
  std::string out;
	std::vector<unsigned char> binaryOut;
};

// File Loading Tests
TEST_F(ParserTest, LoadFileSuccess) {
  createTestFile("test_data/valid.txt", "test content");
  EXPECT_TRUE(parser.loadFile(out, "test_data/valid.txt"));
  EXPECT_EQ(out, "test content");
}

TEST_F(ParserTest, LoadFileEmpty) {
  createTestFile("test_data/empty.txt", "");
  EXPECT_TRUE(parser.loadFile(out, "test_data/empty.txt"));
  EXPECT_TRUE(out.empty());
}

TEST_F(ParserTest, LoadFileNonexistent) {
  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.loadFile(out, "test_data/nonexistent.txt"));
  expectStderrContains({ "Failed to open file: test_data/nonexistent.txt" });
}

TEST_F(ParserTest, LoadBinaryFileSuccess) {
  std::vector<unsigned char> testData = { 0x00, 0x01, 0x02, 0xFF };
  createBinaryFile("test_data/binary.bin", testData);
  EXPECT_TRUE(parser.loadBinaryFile(binaryOut, "test_data/binary.bin"));
  binaryOut.pop_back(); // Remove null terminator for comparison
  EXPECT_EQ(binaryOut, testData);
}

TEST_F(ParserTest, LoadBinaryFileEmpty) {
  createBinaryFile("test_data/empty.bin", {});
  EXPECT_TRUE(parser.loadBinaryFile(binaryOut, "test_data/empty.bin"));
  binaryOut.pop_back(); // Remove null terminator for comparison
  EXPECT_TRUE(binaryOut.empty());
}

TEST_F(ParserTest, LoadBinaryFileNonexistent) {
  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.loadBinaryFile(binaryOut, "test_data/nonexistent.bin"));
  expectStderrContains({ "Failed to open file: test_data/nonexistent.bin" });
}


// parseBool Tests
TEST_F(ParserTest, ParseBoolTrue) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("true");
  bool out{};
  EXPECT_TRUE(parser.parseBool(p, out));
  EXPECT_TRUE(out);
}

TEST_F(ParserTest, ParseBoolFalse) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("false");
  bool out{};
  EXPECT_TRUE(parser.parseBool(p, out));
  EXPECT_FALSE(out);
}

TEST_F(ParserTest, ParseBoolOn) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("on");
  bool out{};
  EXPECT_TRUE(parser.parseBool(p, out));
  EXPECT_TRUE(out);
}

TEST_F(ParserTest, ParseBoolOff) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("off");
  bool out{};
  EXPECT_TRUE(parser.parseBool(p, out));
  EXPECT_FALSE(out);
}

TEST_F(ParserTest, ParseBoolOne) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1");
  bool out{};
  EXPECT_TRUE(parser.parseBool(p, out));
  EXPECT_TRUE(out);
}

TEST_F(ParserTest, ParseBoolZero) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("0");
  bool out{};
  EXPECT_TRUE(parser.parseBool(p, out));
  EXPECT_FALSE(out);
}

TEST_F(ParserTest, ParseBoolWithLeadingWhitespace) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("  true");
  bool out{};
  EXPECT_TRUE(parser.parseBool(p, out));
  EXPECT_TRUE(out);
}

TEST_F(ParserTest, ParseBoolInvalid) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("notabool");
  bool out{};
  EXPECT_FALSE(parser.parseBool(p, out));
}

TEST_F(ParserTest, ParseBoolCaseInsensitiveTrue) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("TRUE");
  bool out{};
  EXPECT_TRUE(parser.parseBool(p, out));
  EXPECT_TRUE(out);
}

TEST_F(ParserTest, ParseBoolCaseInsensitiveFalse) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("False");
  bool out{};
  EXPECT_TRUE(parser.parseBool(p, out));
  EXPECT_FALSE(out);
}

TEST_F(ParserTest, ParseBoolCaseInsensitiveOn) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("ON");
  bool out{};
  EXPECT_TRUE(parser.parseBool(p, out));
  EXPECT_TRUE(out);
}

TEST_F(ParserTest, ParseBoolNullptr) {
  const unsigned char* p = nullptr;
  bool out{};
  EXPECT_FALSE(parser.parseBool(p, out));
}


// parseUInt Tests
TEST_F(ParserTest, ParseUIntValid) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("12345");
  unsigned int out;
  EXPECT_TRUE(parser.parseUInt(p, out));
  EXPECT_EQ(out, 12345u);
}

TEST_F(ParserTest, ParseUIntZero) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("0");
  unsigned int out{};
  EXPECT_TRUE(parser.parseUInt(p, out));
  EXPECT_EQ(out, 0u);
}

TEST_F(ParserTest, ParseUIntInvalid) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("notanumber");
  unsigned int out{};
  EXPECT_FALSE(parser.parseUInt(p, out));
}

TEST_F(ParserTest, ParseUIntNegative) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("-123");
  unsigned int out{};
  EXPECT_FALSE(parser.parseUInt(p, out));
}

TEST_F(ParserTest, ParseUIntMax) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("4294967295");
  unsigned int out{};
  EXPECT_TRUE(parser.parseUInt(p, out));
  EXPECT_EQ(out, 4294967295u);
}

TEST_F(ParserTest, ParseUIntLeadingZeros) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("00123");
  unsigned int out{};
  EXPECT_TRUE(parser.parseUInt(p, out));
  EXPECT_EQ(out, 123u);
}

TEST_F(ParserTest, ParseUIntTrailingNonNumeric) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("123abc");
  unsigned int out{};
  EXPECT_TRUE(parser.parseUInt(p, out));
  EXPECT_EQ(out, 123u);
  EXPECT_EQ(*p, 'a');
}

TEST_F(ParserTest, ParseUIntWithLeadingWhitespace) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("  456");
  unsigned int out{};
  EXPECT_TRUE(parser.parseUInt(p, out));
  EXPECT_EQ(out, 456u);
}

TEST_F(ParserTest, ParseUIntOverflow) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("4294967296"); // UINT_MAX (4294967295) + 1
  unsigned int out{};
  EXPECT_TRUE(parser.parseUInt(p, out));
  EXPECT_EQ(out, 0u);
}

TEST_F(ParserTest, ParseUIntLargeOverflow) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("99999999999");
  unsigned int out{};
  EXPECT_TRUE(parser.parseUInt(p, out));
}

TEST_F(ParserTest, ParseUIntNullptr) {
  const unsigned char* p = nullptr;
  unsigned int out{};
  EXPECT_FALSE(parser.parseUInt(p, out));
}



// parseFloat Tests
TEST_F(ParserTest, ParseFloatValid) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("3.14159");
  float out{};
  EXPECT_TRUE(parser.parseFloat(p, out));
  EXPECT_FLOAT_EQ(out, 3.14159f);
}

TEST_F(ParserTest, ParseFloatPositiveSign) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("+3.14");
  float out{};
  EXPECT_TRUE(parser.parseFloat(p, out));
  EXPECT_FLOAT_EQ(out, 3.14f);
}

TEST_F(ParserTest, ParseFloatNegative) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("-2.5");
  float out{};
  EXPECT_TRUE(parser.parseFloat(p, out));
  EXPECT_FLOAT_EQ(out, -2.5f);
}

TEST_F(ParserTest, ParseFloatScientific) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1.5e-3");
  float out{};
  EXPECT_TRUE(parser.parseFloat(p, out));
  EXPECT_FLOAT_EQ(out, 0.0015f);
}

TEST_F(ParserTest, ParseFloatLargePositiveExponent) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1.0e100");
  float out{};
  EXPECT_TRUE(parser.parseFloat(p, out));
  EXPECT_TRUE(std::isinf(out) || out > 1e38f);
}

TEST_F(ParserTest, ParseFloatVerySmallNegative) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("-1.0e-50");
  float out{};
  EXPECT_TRUE(parser.parseFloat(p, out));
  EXPECT_FLOAT_EQ(out, 0.0f);
}

TEST_F(ParserTest, ParseFloatLargeNegativeExponent) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1.0e-100");
  float out{};
  EXPECT_TRUE(parser.parseFloat(p, out));
  EXPECT_FLOAT_EQ(out, 0.0f); 
}

TEST_F(ParserTest, ParseFloatExponentNoDigits) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1.5e");
  float out{};
  EXPECT_FALSE(parser.parseFloat(p, out));
}

TEST_F(ParserTest, ParseFloatExponentSignNoDigits) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1.5e+");
  float out{};
  EXPECT_FALSE(parser.parseFloat(p, out));
}

TEST_F(ParserTest, ParseFloatNaN) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("nan");
  float out{};
  EXPECT_FALSE(parser.parseFloat(p, out)); 
}

TEST_F(ParserTest, ParseFloatResultingInInfinity) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1e500");
  float out{};
  EXPECT_TRUE(parser.parseFloat(p, out));
  EXPECT_TRUE(std::isinf(out) && out > 0); 
}

TEST_F(ParserTest, ParseFloatResultingInNegativeInfinity) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("-1e500");
  float out{};
  EXPECT_TRUE(parser.parseFloat(p, out));
  EXPECT_TRUE(std::isinf(out) && out < 0); 
}

TEST_F(ParserTest, ParseFloatTrailingGarbage) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1.23abc");
  float out{};
  EXPECT_TRUE(parser.parseFloat(p, out));
  EXPECT_FLOAT_EQ(out, 1.23f);
  EXPECT_EQ(*p, 'a');
}

TEST_F(ParserTest, ParseFloatWithLeadingWhitespace) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("  2.718");
  float out{};
  EXPECT_TRUE(parser.parseFloat(p, out));
  EXPECT_FLOAT_EQ(out, 2.718f);
}

TEST_F(ParserTest, ParseFloatOnlyDecimalPoint) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>(".");
  float out{};
  EXPECT_FALSE(parser.parseFloat(p, out));
}

TEST_F(ParserTest, ParseFloatOnlyMinusSign) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("-");
  float out{};
  EXPECT_FALSE(parser.parseFloat(p, out));
}

TEST_F(ParserTest, ParseFloatOnlyPlusSign) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("+");
  float out{};
  EXPECT_FALSE(parser.parseFloat(p, out));
}

TEST_F(ParserTest, ParseFloatInvalid) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("notafloat");
  float out{};
  EXPECT_FALSE(parser.parseFloat(p, out));
}

TEST_F(ParserTest, ParseFloatNullptr) {
  const unsigned char* p = nullptr;
  float out{};
  EXPECT_FALSE(parser.parseFloat(p, out));
}



// parseToken Tests
TEST_F(ParserTest, ParseTokenValid) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("token");
  unsigned char out[64] = {};
  EXPECT_TRUE(parser.parseToken(p, out, 64));
  EXPECT_STREQ(reinterpret_cast<char*>(out), "token");
}

TEST_F(ParserTest, ParseTokenWithWhitespace) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("  token  ");
  unsigned char out[64] = {};
  EXPECT_TRUE(parser.parseToken(p, out, 64));
  EXPECT_STREQ(reinterpret_cast<char*>(out), "token");
}

TEST_F(ParserTest, ParseTokenEmpty) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("");
  unsigned char out[64] = {};
  EXPECT_FALSE(parser.parseToken(p, out, 64));
}

TEST_F(ParserTest, ParseTokenExactMaxLength) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1234567");
  unsigned char out[8] = {}; 
  EXPECT_TRUE(parser.parseToken(p, out, 8));
  EXPECT_STREQ(reinterpret_cast<char*>(out), "1234567");
}

TEST_F(ParserTest, ParseTokenZeroMaxLength) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("token");
  unsigned char out[64] = {};
  EXPECT_FALSE(parser.parseToken(p, out, 0));
}

TEST_F(ParserTest, ParseTokenTruncation) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("verylongtoken");
  unsigned char out[8] = {};
  EXPECT_TRUE(parser.parseToken(p, out, 8));
  EXPECT_EQ(strlen(reinterpret_cast<char*>(out)), 7u); 
}

TEST_F(ParserTest, ParseTokenWithCommaDelimiter) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("token,rest");
  unsigned char out[64] = {};
  EXPECT_TRUE(parser.parseToken(p, out, 64));
  EXPECT_STREQ(reinterpret_cast<char*>(out), "token");
}

TEST_F(ParserTest, ParseTokenNullOutput) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("token");
  EXPECT_FALSE(parser.parseToken(p, nullptr, 64));
}

TEST_F(ParserTest, ParseTokenNullInput) {
  const unsigned char* p = nullptr;
  unsigned char out[64] = {};
  EXPECT_FALSE(parser.parseToken(p, out, 64));
}



// parseVec2f Tests
TEST_F(ParserTest, ParseVec2fValid) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1.0 2.0");
  Starlet::Math::Vec2<float> out;
  EXPECT_TRUE(parser.parseVec2f(p, out));
  EXPECT_FLOAT_EQ(out.x, 1.0f);
  EXPECT_FLOAT_EQ(out.y, 2.0f);
}

TEST_F(ParserTest, ParseVec2fExtraNumbers) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1.0 2.0 3.0");
  Starlet::Math::Vec2<float> out;
  EXPECT_TRUE(parser.parseVec2f(p, out));
  EXPECT_FLOAT_EQ(out.x, 1.0f);
  EXPECT_FLOAT_EQ(out.y, 2.0f);
  EXPECT_STREQ(reinterpret_cast<const char*>(p), " 3.0");
}

TEST_F(ParserTest, ParseVec2fTrailingNonNumeric) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1.0 2.0abc");
  Starlet::Math::Vec2<float> out;
  EXPECT_TRUE(parser.parseVec2f(p, out));
  EXPECT_FLOAT_EQ(out.x, 1.0f);
  EXPECT_FLOAT_EQ(out.y, 2.0f);
  EXPECT_EQ(*p, 'a'); 
}

TEST_F(ParserTest, ParseVec2fInvalid) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1.0");
  Starlet::Math::Vec2<float> out;
  EXPECT_FALSE(parser.parseVec2f(p, out));
}

TEST_F(ParserTest, ParseVec2fNullptr) {
  const unsigned char* p = nullptr;
  Starlet::Math::Vec2<float> out;
  EXPECT_FALSE(parser.parseVec2f(p, out));
}



// parseVec3f Tests
TEST_F(ParserTest, ParseVec3fValid) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1.0 2.0 3.0");
  Starlet::Math::Vec3<float> out;
  EXPECT_TRUE(parser.parseVec3f(p, out));
  EXPECT_FLOAT_EQ(out.x, 1.0f);
  EXPECT_FLOAT_EQ(out.y, 2.0f);
  EXPECT_FLOAT_EQ(out.z, 3.0f);
}

TEST_F(ParserTest, ParseVec3fInvalid) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1.0 2.0");
  Starlet::Math::Vec3<float> out;
  EXPECT_FALSE(parser.parseVec3f(p, out));
}

TEST_F(ParserTest, ParseVec3fNullptr) {
  const unsigned char* p = nullptr;
  Starlet::Math::Vec3<float> out;
  EXPECT_FALSE(parser.parseVec3f(p, out));
}

TEST_F(ParserTest, ParseVec3fWithLeadingWhitespace) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("  1.0  2.0  3.0");
  Starlet::Math::Vec3<float> out;
  EXPECT_TRUE(parser.parseVec3f(p, out));
  EXPECT_FLOAT_EQ(out.x, 1.0f);
  EXPECT_FLOAT_EQ(out.y, 2.0f);
  EXPECT_FLOAT_EQ(out.z, 3.0f);
}

TEST_F(ParserTest, ParseVec3fTrailingNonNumeric) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1.0 2.0 3.0abc");
  Starlet::Math::Vec3<float> out;
  EXPECT_TRUE(parser.parseVec3f(p, out));
  EXPECT_FLOAT_EQ(out.x, 1.0f);
  EXPECT_FLOAT_EQ(out.y, 2.0f);
  EXPECT_FLOAT_EQ(out.z, 3.0f);
  EXPECT_EQ(*p, 'a');
}

// parseVec4f Tests
TEST_F(ParserTest, ParseVec4fValid) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1.0 2.0 3.0 4.0");
  Starlet::Math::Vec4<float> out;
  EXPECT_TRUE(parser.parseVec4f(p, out));
  EXPECT_FLOAT_EQ(out.x, 1.0f);
  EXPECT_FLOAT_EQ(out.y, 2.0f);
  EXPECT_FLOAT_EQ(out.z, 3.0f);
  EXPECT_FLOAT_EQ(out.w, 4.0f);
}

TEST_F(ParserTest, ParseVec4fInvalid) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1.0 2.0 3.0");
  Starlet::Math::Vec4<float> out;
  EXPECT_FALSE(parser.parseVec4f(p, out));
}

TEST_F(ParserTest, ParseVec4fInvalidCharacter) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1.0 2.0 abc 4.0");
  Starlet::Math::Vec4<float> out;
  EXPECT_FALSE(parser.parseVec4f(p, out));
}

TEST_F(ParserTest, ParseVec4fNullptr) {
  const unsigned char* p = nullptr;
  Starlet::Math::Vec4<float> out;
  EXPECT_FALSE(parser.parseVec4f(p, out));
}

TEST_F(ParserTest, ParseVec4fTrailingNonNumeric) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1.0 2.0 3.0 4.0 extra");
  Starlet::Math::Vec4<float> out;
  EXPECT_TRUE(parser.parseVec4f(p, out));
  EXPECT_FLOAT_EQ(out.x, 1.0f);
  EXPECT_FLOAT_EQ(out.y, 2.0f);
  EXPECT_FLOAT_EQ(out.z, 3.0f);
  EXPECT_FLOAT_EQ(out.w, 4.0f);
  EXPECT_EQ(*p, ' '); 
}

// skipWhitespace Tests
TEST_F(ParserTest, SkipWhitespaceSpaces) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("   text");
  const unsigned char* result = parser.skipWhitespace(p);
  EXPECT_EQ(*result, 't');
}

TEST_F(ParserTest, SkipWhitespaceTabs) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("\t\ttext");
  const unsigned char* result = parser.skipWhitespace(p);
  EXPECT_EQ(*result, 't');
}

TEST_F(ParserTest, SkipWhitespaceComma) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>(", text");
  const unsigned char* result = parser.skipWhitespace(p, true);
  EXPECT_EQ(*result, 't');
}

TEST_F(ParserTest, SkipWhitespaceNoComma) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>(", text");
  const unsigned char* result = parser.skipWhitespace(p, false);
  EXPECT_EQ(*result, ',');
}

TEST_F(ParserTest, SkipWhitespaceAtEnd) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("   ");
  const unsigned char* result = parser.skipWhitespace(p);
  EXPECT_EQ(*result, '\0');
}

TEST_F(ParserTest, SkipWhitespaceMixed) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>(" \t,\n text");
  const unsigned char* result = parser.skipWhitespace(p);
  EXPECT_EQ(*result, 't');
}

TEST_F(ParserTest, SkipWhitespaceNullptr) {
  const unsigned char* p = nullptr;
  const unsigned char* result = parser.skipWhitespace(p);
  EXPECT_EQ(result, nullptr);
}

// skipToNextLine Tests
TEST_F(ParserTest, SkipToNextLineUnix) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("line1\nline2");
  const unsigned char* result = parser.skipToNextLine(p);
  EXPECT_EQ(*result, 'l');
  EXPECT_STREQ(reinterpret_cast<const char*>(result), "line2");
}

TEST_F(ParserTest, SkipToNextLineWindows) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("line1\r\nline2");
  const unsigned char* result = parser.skipToNextLine(p);
  EXPECT_EQ(*result, 'l');
  EXPECT_STREQ(reinterpret_cast<const char*>(result), "line2");
}

TEST_F(ParserTest, SkipToNextLineNoNewline) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("singleline");
  const unsigned char* result = parser.skipToNextLine(p);
  EXPECT_EQ(*result, '\0');
}

TEST_F(ParserTest, SkipToNextLineEmpty) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("");
  const unsigned char* result = parser.skipToNextLine(p);
  EXPECT_EQ(*result, '\0');
}

TEST_F(ParserTest, SkipToNextLineStartsWithNewline) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>("\nrest");
  const unsigned char* result = parser.skipToNextLine(p);
  EXPECT_EQ(*result, 'r');
  EXPECT_STREQ(reinterpret_cast<const char*>(result), "rest");
}

TEST_F(ParserTest, SkipToNextLineNullptr) {
  const unsigned char* p = nullptr;
  const unsigned char* result = parser.skipToNextLine(p);
  EXPECT_EQ(result, nullptr);
}

// trimEOL Tests
TEST_F(ParserTest, TrimEOLUnix) {
  const unsigned char data[] = "text\n";
  const unsigned char* result = parser.trimEOL(data, data + 5);
  EXPECT_EQ(result - data, 4);
}

TEST_F(ParserTest, TrimEOLWindows) {
  const unsigned char data[] = "text\r\n";
  const unsigned char* result = parser.trimEOL(data, data + 6);
  EXPECT_EQ(result - data, 4);
}

TEST_F(ParserTest, TrimEOLNoNewline) {
  const unsigned char data[] = "text";
  const unsigned char* result = parser.trimEOL(data, data + 4);
  EXPECT_EQ(result - data, 4);
}

TEST_F(ParserTest, TrimEOLTrailingComma) {
  const unsigned char data[] = "text,";
  const unsigned char* result = parser.trimEOL(data, data + 5);
  EXPECT_EQ(result - data, 4);
}

TEST_F(ParserTest, TrimEOLOnlyNewlines) {
  const unsigned char data[] = "\r\n";
  const unsigned char* result = parser.trimEOL(data, data + 2);
  EXPECT_EQ(result, data); 
}

TEST_F(ParserTest, TrimEOLEmpty) {
  const unsigned char data[] = "";
  const unsigned char* result = parser.trimEOL(data, data);
  EXPECT_EQ(result, data);
}

TEST_F(ParserTest, TrimEOLMixedLineEndings) {
  const unsigned char data[] = "text\r\n,\n";
  const unsigned char* result = parser.trimEOL(data, data + 8);
  EXPECT_EQ(result - data, 4);
}