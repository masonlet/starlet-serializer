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

// File Loading Tests
TEST(ParserTest, LoadFileSuccess) {
  createTestFile("test_data/valid.txt", "test content");
  SSerializer::Parser parser;
  std::string out;
  EXPECT_TRUE(parser.loadFile(out, "test_data/valid.txt"));
  EXPECT_EQ(out, "test content");
}

TEST(ParserTest, LoadFileEmpty) {
  createTestFile("test_data/empty.txt", "");
  SSerializer::Parser parser;
  std::string out;
  EXPECT_TRUE(parser.loadFile(out, "test_data/empty.txt"));
  EXPECT_TRUE(out.empty());
}

TEST(ParserTest, LoadFileNonexistent) {
  SSerializer::Parser parser;
  std::string out;
  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.loadFile(out, "test_data/nonexistent.txt"));
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Failed to open file: test_data/nonexistent.txt"), std::string::npos);
}

TEST(ParserTest, LoadBinaryFileSuccess) {
  std::vector<unsigned char> testData = { 0x00, 0x01, 0x02, 0xFF };
  createBinaryFile("test_data/binary.bin", testData);
  SSerializer::Parser parser;
  std::vector<unsigned char> out;
  EXPECT_TRUE(parser.loadBinaryFile(out, "test_data/binary.bin"));
  EXPECT_EQ(out, testData);
}

TEST(ParserTest, LoadBinaryFileEmpty) {
  createBinaryFile("test_data/empty.bin", {});
  SSerializer::Parser parser;
  std::vector<unsigned char> out;
  EXPECT_TRUE(parser.loadBinaryFile(out, "test_data/empty.bin"));
  EXPECT_TRUE(out.empty());
}

TEST(ParserTest, LoadBinaryFileNonexistent) {
  SSerializer::Parser parser;
  std::vector<unsigned char> out;
  testing::internal::CaptureStderr();
  EXPECT_FALSE(parser.loadBinaryFile(out, "test_data/nonexistent.bin"));
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Failed to open file: test_data/nonexistent.bin"), std::string::npos);
}


// parseBool Tests
TEST(ParserTest, ParseBoolTrue) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("true");
  bool out{};
  EXPECT_TRUE(parser.parseBool(p, out));
  EXPECT_TRUE(out);
}

TEST(ParserTest, ParseBoolFalse) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("false");
  bool out{};
  EXPECT_TRUE(parser.parseBool(p, out));
  EXPECT_FALSE(out);
}

TEST(ParserTest, ParseBoolOn) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("on");
  bool out{};
  EXPECT_TRUE(parser.parseBool(p, out));
  EXPECT_TRUE(out);
}

TEST(ParserTest, ParseBoolOff) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("off");
  bool out{};
  EXPECT_TRUE(parser.parseBool(p, out));
  EXPECT_FALSE(out);
}

TEST(ParserTest, ParseBoolOne) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1");
  bool out{};
  EXPECT_TRUE(parser.parseBool(p, out));
  EXPECT_TRUE(out);
}

TEST(ParserTest, ParseBoolZero) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("0");
  bool out{};
  EXPECT_TRUE(parser.parseBool(p, out));
  EXPECT_FALSE(out);
}

TEST(ParserTest, ParseBoolWithLeadingWhitespace) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("  true");
  bool out{};
  EXPECT_TRUE(parser.parseBool(p, out));
  EXPECT_TRUE(out);
}

TEST(ParserTest, ParseBoolInvalid) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("notabool");
  bool out{};
  EXPECT_FALSE(parser.parseBool(p, out));
}

TEST(ParserTest, ParseBoolCaseInsensitiveTrue) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("TRUE");
  bool out{};
  EXPECT_TRUE(parser.parseBool(p, out));
  EXPECT_TRUE(out);
}

TEST(ParserTest, ParseBoolCaseInsensitiveFalse) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("False");
  bool out{};
  EXPECT_TRUE(parser.parseBool(p, out));
  EXPECT_FALSE(out);
}

TEST(ParserTest, ParseBoolCaseInsensitiveOn) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("ON");
  bool out{};
  EXPECT_TRUE(parser.parseBool(p, out));
  EXPECT_TRUE(out);
}

TEST(ParserTest, ParseBoolNullptr) {
  SSerializer::Parser parser;
  const unsigned char* p = nullptr;
  bool out{};
  EXPECT_FALSE(parser.parseBool(p, out));
}


// parseUInt Tests
TEST(ParserTest, ParseUIntValid) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("12345");
  unsigned int out;
  EXPECT_TRUE(parser.parseUInt(p, out));
  EXPECT_EQ(out, 12345u);
}

TEST(ParserTest, ParseUIntZero) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("0");
  unsigned int out{};
  EXPECT_TRUE(parser.parseUInt(p, out));
  EXPECT_EQ(out, 0u);
}

TEST(ParserTest, ParseUIntInvalid) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("notanumber");
  unsigned int out{};
  EXPECT_FALSE(parser.parseUInt(p, out));
}

TEST(ParserTest, ParseUIntNegative) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("-123");
  unsigned int out{};
  EXPECT_FALSE(parser.parseUInt(p, out));
}

TEST(ParserTest, ParseUIntMax) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("4294967295");
  unsigned int out{};
  EXPECT_TRUE(parser.parseUInt(p, out));
  EXPECT_EQ(out, 4294967295u);
}

TEST(ParserTest, ParseUIntLeadingZeros) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("00123");
  unsigned int out{};
  EXPECT_TRUE(parser.parseUInt(p, out));
  EXPECT_EQ(out, 123u);
}

TEST(ParserTest, ParseUIntTrailingNonNumeric) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("123abc");
  unsigned int out{};
  EXPECT_TRUE(parser.parseUInt(p, out));
  EXPECT_EQ(out, 123u);
  EXPECT_EQ(*p, 'a');
}

TEST(ParserTest, ParseUIntWithLeadingWhitespace) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("  456");
  unsigned int out{};
  EXPECT_TRUE(parser.parseUInt(p, out));
  EXPECT_EQ(out, 456u);
}

TEST(ParserTest, ParseUIntOverflow) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("4294967296"); // UINT_MAX (4294967295) + 1
  unsigned int out{};
  EXPECT_TRUE(parser.parseUInt(p, out));
  EXPECT_EQ(out, 0u);
}

TEST(ParserTest, ParseUIntLargeOverflow) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("99999999999");
  unsigned int out{};
  EXPECT_TRUE(parser.parseUInt(p, out));
}

TEST(ParserTest, ParseUIntNullptr) {
  SSerializer::Parser parser;
  const unsigned char* p = nullptr;
  unsigned int out{};
  EXPECT_FALSE(parser.parseUInt(p, out));
}



// parseFloat Tests
TEST(ParserTest, ParseFloatValid) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("3.14159");
  float out{};
  EXPECT_TRUE(parser.parseFloat(p, out));
  EXPECT_FLOAT_EQ(out, 3.14159f);
}

TEST(ParserTest, ParseFloatPositiveSign) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("+3.14");
  float out{};
  EXPECT_TRUE(parser.parseFloat(p, out));
  EXPECT_FLOAT_EQ(out, 3.14f);
}

TEST(ParserTest, ParseFloatNegative) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("-2.5");
  float out{};
  EXPECT_TRUE(parser.parseFloat(p, out));
  EXPECT_FLOAT_EQ(out, -2.5f);
}

TEST(ParserTest, ParseFloatScientific) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1.5e-3");
  float out{};
  EXPECT_TRUE(parser.parseFloat(p, out));
  EXPECT_FLOAT_EQ(out, 0.0015f);
}

TEST(ParserTest, ParseFloatLargePositiveExponent) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1.0e100");
  float out{};
  EXPECT_TRUE(parser.parseFloat(p, out));
  EXPECT_TRUE(std::isinf(out) || out > 1e38f);
}

TEST(ParserTest, ParseFloatVerySmallNegative) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("-1.0e-50");
  float out{};
  EXPECT_TRUE(parser.parseFloat(p, out));
  EXPECT_FLOAT_EQ(out, 0.0f);
}

TEST(ParserTest, ParseFloatLargeNegativeExponent) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1.0e-100");
  float out{};
  EXPECT_TRUE(parser.parseFloat(p, out));
  EXPECT_FLOAT_EQ(out, 0.0f); 
}

TEST(ParserTest, ParseFloatExponentNoDigits) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1.5e");
  float out{};
  EXPECT_FALSE(parser.parseFloat(p, out));
}

TEST(ParserTest, ParseFloatExponentSignNoDigits) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1.5e+");
  float out{};
  EXPECT_FALSE(parser.parseFloat(p, out));
}

TEST(ParserTest, ParseFloatNaN) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("nan");
  float out{};
  EXPECT_FALSE(parser.parseFloat(p, out)); 
}

TEST(ParserTest, ParseFloatResultingInInfinity) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1e500");
  float out{};
  EXPECT_TRUE(parser.parseFloat(p, out));
  EXPECT_TRUE(std::isinf(out) && out > 0); 
}

TEST(ParserTest, ParseFloatResultingInNegativeInfinity) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("-1e500");
  float out{};
  EXPECT_TRUE(parser.parseFloat(p, out));
  EXPECT_TRUE(std::isinf(out) && out < 0); 
}

TEST(ParserTest, ParseFloatTrailingGarbage) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1.23abc");
  float out{};
  EXPECT_TRUE(parser.parseFloat(p, out));
  EXPECT_FLOAT_EQ(out, 1.23f);
  EXPECT_EQ(*p, 'a');
}

TEST(ParserTest, ParseFloatWithLeadingWhitespace) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("  2.718");
  float out{};
  EXPECT_TRUE(parser.parseFloat(p, out));
  EXPECT_FLOAT_EQ(out, 2.718f);
}

TEST(ParserTest, ParseFloatOnlyDecimalPoint) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>(".");
  float out{};
  EXPECT_FALSE(parser.parseFloat(p, out));
}

TEST(ParserTest, ParseFloatOnlyMinusSign) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("-");
  float out{};
  EXPECT_FALSE(parser.parseFloat(p, out));
}

TEST(ParserTest, ParseFloatOnlyPlusSign) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("+");
  float out{};
  EXPECT_FALSE(parser.parseFloat(p, out));
}

TEST(ParserTest, ParseFloatInvalid) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("notafloat");
  float out{};
  EXPECT_FALSE(parser.parseFloat(p, out));
}

TEST(ParserTest, ParseFloatNullptr) {
  SSerializer::Parser parser;
  const unsigned char* p = nullptr;
  float out{};
  EXPECT_FALSE(parser.parseFloat(p, out));
}



// parseToken Tests
TEST(ParserTest, ParseTokenValid) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("token");
  unsigned char out[64] = {};
  EXPECT_TRUE(parser.parseToken(p, out, 64));
  EXPECT_STREQ(reinterpret_cast<char*>(out), "token");
}

TEST(ParserTest, ParseTokenWithWhitespace) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("  token  ");
  unsigned char out[64] = {};
  EXPECT_TRUE(parser.parseToken(p, out, 64));
  EXPECT_STREQ(reinterpret_cast<char*>(out), "token");
}

TEST(ParserTest, ParseTokenEmpty) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("");
  unsigned char out[64] = {};
  EXPECT_FALSE(parser.parseToken(p, out, 64));
}

TEST(ParserTest, ParseTokenExactMaxLength) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1234567");
  unsigned char out[8] = {}; 
  EXPECT_TRUE(parser.parseToken(p, out, 8));
  EXPECT_STREQ(reinterpret_cast<char*>(out), "1234567");
}

TEST(ParserTest, ParseTokenZeroMaxLength) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("token");
  unsigned char out[64] = {};
  EXPECT_FALSE(parser.parseToken(p, out, 0));
}

TEST(ParserTest, ParseTokenTruncation) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("verylongtoken");
  unsigned char out[8] = {};
  EXPECT_TRUE(parser.parseToken(p, out, 8));
  EXPECT_EQ(strlen(reinterpret_cast<char*>(out)), 7u); 
}

TEST(ParserTest, ParseTokenWithCommaDelimiter) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("token,rest");
  unsigned char out[64] = {};
  EXPECT_TRUE(parser.parseToken(p, out, 64));
  EXPECT_STREQ(reinterpret_cast<char*>(out), "token");
}

TEST(ParserTest, ParseTokenNullOutput) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("token");
  EXPECT_FALSE(parser.parseToken(p, nullptr, 64));
}

TEST(ParserTest, ParseTokenNullInput) {
  SSerializer::Parser parser;
  const unsigned char* p = nullptr;
  unsigned char out[64] = {};
  EXPECT_FALSE(parser.parseToken(p, out, 64));
}



// parseVec2f Tests
TEST(ParserTest, ParseVec2fValid) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1.0 2.0");
  Starlet::Math::Vec2<float> out;
  EXPECT_TRUE(parser.parseVec2f(p, out));
  EXPECT_FLOAT_EQ(out.x, 1.0f);
  EXPECT_FLOAT_EQ(out.y, 2.0f);
}

TEST(ParserTest, ParseVec2fExtraNumbers) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1.0 2.0 3.0");
  Starlet::Math::Vec2<float> out;
  EXPECT_TRUE(parser.parseVec2f(p, out));
  EXPECT_FLOAT_EQ(out.x, 1.0f);
  EXPECT_FLOAT_EQ(out.y, 2.0f);
  EXPECT_STREQ(reinterpret_cast<const char*>(p), " 3.0");
}

TEST(ParserTest, ParseVec2fTrailingNonNumeric) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1.0 2.0abc");
  Starlet::Math::Vec2<float> out;
  EXPECT_TRUE(parser.parseVec2f(p, out));
  EXPECT_FLOAT_EQ(out.x, 1.0f);
  EXPECT_FLOAT_EQ(out.y, 2.0f);
  EXPECT_EQ(*p, 'a'); 
}

TEST(ParserTest, ParseVec2fInvalid) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1.0");
  Starlet::Math::Vec2<float> out;
  EXPECT_FALSE(parser.parseVec2f(p, out));
}

TEST(ParserTest, ParseVec2fNullptr) {
  SSerializer::Parser parser;
  const unsigned char* p = nullptr;
  Starlet::Math::Vec2<float> out;
  EXPECT_FALSE(parser.parseVec2f(p, out));
}



// parseVec3f Tests
TEST(ParserTest, ParseVec3fValid) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1.0 2.0 3.0");
  Starlet::Math::Vec3<float> out;
  EXPECT_TRUE(parser.parseVec3f(p, out));
  EXPECT_FLOAT_EQ(out.x, 1.0f);
  EXPECT_FLOAT_EQ(out.y, 2.0f);
  EXPECT_FLOAT_EQ(out.z, 3.0f);
}

TEST(ParserTest, ParseVec3fInvalid) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1.0 2.0");
  Starlet::Math::Vec3<float> out;
  EXPECT_FALSE(parser.parseVec3f(p, out));
}

TEST(ParserTest, ParseVec3fNullptr) {
  SSerializer::Parser parser;
  const unsigned char* p = nullptr;
  Starlet::Math::Vec3<float> out;
  EXPECT_FALSE(parser.parseVec3f(p, out));
}

TEST(ParserTest, ParseVec3fWithLeadingWhitespace) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("  1.0  2.0  3.0");
  Starlet::Math::Vec3<float> out;
  EXPECT_TRUE(parser.parseVec3f(p, out));
  EXPECT_FLOAT_EQ(out.x, 1.0f);
  EXPECT_FLOAT_EQ(out.y, 2.0f);
  EXPECT_FLOAT_EQ(out.z, 3.0f);
}

TEST(ParserTest, ParseVec3fTrailingNonNumeric) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1.0 2.0 3.0abc");
  Starlet::Math::Vec3<float> out;
  EXPECT_TRUE(parser.parseVec3f(p, out));
  EXPECT_FLOAT_EQ(out.x, 1.0f);
  EXPECT_FLOAT_EQ(out.y, 2.0f);
  EXPECT_FLOAT_EQ(out.z, 3.0f);
  EXPECT_EQ(*p, 'a');
}

// parseVec4f Tests
TEST(ParserTest, ParseVec4fValid) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1.0 2.0 3.0 4.0");
  Starlet::Math::Vec4<float> out;
  EXPECT_TRUE(parser.parseVec4f(p, out));
  EXPECT_FLOAT_EQ(out.x, 1.0f);
  EXPECT_FLOAT_EQ(out.y, 2.0f);
  EXPECT_FLOAT_EQ(out.z, 3.0f);
  EXPECT_FLOAT_EQ(out.w, 4.0f);
}

TEST(ParserTest, ParseVec4fInvalid) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1.0 2.0 3.0");
  Starlet::Math::Vec4<float> out;
  EXPECT_FALSE(parser.parseVec4f(p, out));
}

TEST(ParserTest, ParseVec4fInvalidCharacter) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("1.0 2.0 abc 4.0");
  Starlet::Math::Vec4<float> out;
  EXPECT_FALSE(parser.parseVec4f(p, out));
}

TEST(ParserTest, ParseVec4fNullptr) {
  SSerializer::Parser parser;
  const unsigned char* p = nullptr;
  Starlet::Math::Vec4<float> out;
  EXPECT_FALSE(parser.parseVec4f(p, out));
}

TEST(ParserTest, ParseVec4fTrailingNonNumeric) {
  SSerializer::Parser parser;
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
TEST(ParserTest, SkipWhitespaceSpaces) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("   text");
  const unsigned char* result = parser.skipWhitespace(p);
  EXPECT_EQ(*result, 't');
}

TEST(ParserTest, SkipWhitespaceTabs) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("\t\ttext");
  const unsigned char* result = parser.skipWhitespace(p);
  EXPECT_EQ(*result, 't');
}

TEST(ParserTest, SkipWhitespaceComma) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>(", text");
  const unsigned char* result = parser.skipWhitespace(p, true);
  EXPECT_EQ(*result, 't');
}

TEST(ParserTest, SkipWhitespaceNoComma) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>(", text");
  const unsigned char* result = parser.skipWhitespace(p, false);
  EXPECT_EQ(*result, ',');
}

TEST(ParserTest, SkipWhitespaceAtEnd) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("   ");
  const unsigned char* result = parser.skipWhitespace(p);
  EXPECT_EQ(*result, '\0');
}

TEST(ParserTest, SkipWhitespaceMixed) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>(" \t,\n text");
  const unsigned char* result = parser.skipWhitespace(p);
  EXPECT_EQ(*result, 't');
}

TEST(ParserTest, SkipWhitespaceNullptr) {
  SSerializer::Parser parser;
  const unsigned char* p = nullptr;
  const unsigned char* result = parser.skipWhitespace(p);
  EXPECT_EQ(result, nullptr);
}

// skipToNextLine Tests
TEST(ParserTest, SkipToNextLineUnix) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("line1\nline2");
  const unsigned char* result = parser.skipToNextLine(p);
  EXPECT_EQ(*result, 'l');
  EXPECT_STREQ(reinterpret_cast<const char*>(result), "line2");
}

TEST(ParserTest, SkipToNextLineWindows) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("line1\r\nline2");
  const unsigned char* result = parser.skipToNextLine(p);
  EXPECT_EQ(*result, 'l');
  EXPECT_STREQ(reinterpret_cast<const char*>(result), "line2");
}

TEST(ParserTest, SkipToNextLineNoNewline) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("singleline");
  const unsigned char* result = parser.skipToNextLine(p);
  EXPECT_EQ(*result, '\0');
}

TEST(ParserTest, SkipToNextLineEmpty) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("");
  const unsigned char* result = parser.skipToNextLine(p);
  EXPECT_EQ(*result, '\0');
}

TEST(ParserTest, SkipToNextLineStartsWithNewline) {
  SSerializer::Parser parser;
  const unsigned char* p = reinterpret_cast<const unsigned char*>("\nrest");
  const unsigned char* result = parser.skipToNextLine(p);
  EXPECT_EQ(*result, 'r');
  EXPECT_STREQ(reinterpret_cast<const char*>(result), "rest");
}

TEST(ParserTest, SkipToNextLineNullptr) {
  SSerializer::Parser parser;
  const unsigned char* p = nullptr;
  const unsigned char* result = parser.skipToNextLine(p);
  EXPECT_EQ(result, nullptr);
}

// trimEOL Tests
TEST(ParserTest, TrimEOLUnix) {
  SSerializer::Parser parser;
  const unsigned char data[] = "text\n";
  const unsigned char* result = parser.trimEOL(data, data + 5);
  EXPECT_EQ(result - data, 4);
}

TEST(ParserTest, TrimEOLWindows) {
  SSerializer::Parser parser;
  const unsigned char data[] = "text\r\n";
  const unsigned char* result = parser.trimEOL(data, data + 6);
  EXPECT_EQ(result - data, 4);
}

TEST(ParserTest, TrimEOLNoNewline) {
  SSerializer::Parser parser;
  const unsigned char data[] = "text";
  const unsigned char* result = parser.trimEOL(data, data + 4);
  EXPECT_EQ(result - data, 4);
}

TEST(ParserTest, TrimEOLTrailingComma) {
  SSerializer::Parser parser;
  const unsigned char data[] = "text,";
  const unsigned char* result = parser.trimEOL(data, data + 5);
  EXPECT_EQ(result - data, 4);
}

TEST(ParserTest, TrimEOLOnlyNewlines) {
  SSerializer::Parser parser;
  const unsigned char data[] = "\r\n";
  const unsigned char* result = parser.trimEOL(data, data + 2);
  EXPECT_EQ(result, data); 
}

TEST(ParserTest, TrimEOLEmpty) {
  SSerializer::Parser parser;
  const unsigned char data[] = "";
  const unsigned char* result = parser.trimEOL(data, data);
  EXPECT_EQ(result, data);
}

TEST(ParserTest, TrimEOLMixedLineEndings) {
  SSerializer::Parser parser;
  const unsigned char data[] = "text\r\n,\n";
  const unsigned char* result = parser.trimEOL(data, data + 8);
  EXPECT_EQ(result - data, 4);
}