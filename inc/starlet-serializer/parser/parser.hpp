#pragma once

#include <string>
#include <vector>

namespace Starlet {

namespace Math {

template <typename T> struct Vec2;
template <typename T> struct Vec3;
template <typename T> struct Vec4;

}

namespace Serializer {

#ifndef STARLET_PARSE_OR 
#define STARLET_PARSE_OR(onFail, parser, target, errorMsg) \
do { \
  if (!(parser(p, target))) { \
      if((errorMsg) && *(errorMsg) != '\0') fprintf(stderr, "[Parser ERROR]: Failed to parse %s\n", errorMsg); \
      onFail; \
	} \
} while(0)
#endif

#ifndef STARLET_PARSE_STRING_OR
#define STARLET_PARSE_STRING_OR(onFail, p, target, size, label) \
  do {\
    char temp[size]{}; \
    if (!parseToken(p, reinterpret_cast<unsigned char*>(temp), size) || strlen(temp) == 0) { \
      fprintf(stderr, "[Parser ERROR] Failed to parse %s\n", label); \
      onFail; \
    } \
    target = temp; \
  } while (0)
#endif

class Parser {
public:
	static constexpr size_t MAX_SIZE = static_cast<size_t>(200 * 1024) * 1024; //200MB Limit

	bool loadFile(std::string& out, const std::string& path);
	bool loadBinaryFile(std::vector<unsigned char>& dataOut, const std::string& path);

	bool parseBool(const unsigned char*& p, bool& out);
	bool parseUInt(const unsigned char*& p, unsigned int& out);
	bool parseFloat(const unsigned char*& p, float& out);
	bool parseToken(const unsigned char*& p, unsigned char* out, const size_t maxLength);

	bool parseVec2f(const unsigned char*& p, Math::Vec2<float>& out);
	bool parseVec3f(const unsigned char*& p, Math::Vec3<float>& out);
	bool parseVec4f(const unsigned char*& p, Math::Vec4<float>& out);

	const unsigned char* skipToNextLine(const unsigned char* p);
	const unsigned char* skipWhitespace(const unsigned char* p, bool skipComma = true);
	const unsigned char* trimEOL(const unsigned char* p, const unsigned char* end);

private:
	bool getFileSize(FILE* file, size_t& sizeOut) const;

	bool isDelim(unsigned char c, bool comma = true);
};

}

}