#include "StarletSerializer/parser/parser.hpp"
#include "StarletSerializer/utils/log.hpp"

#include "StarletMath/vec2.hpp"
#include "StarletMath/vec3.hpp"
#include "StarletMath/vec4.hpp"

#include <cstdlib>  
#include <cstring>
#include <cstdio>

bool Parser::loadFile(std::string& out, const std::string& path) {
	FILE* file = fopen(path.c_str(), "rb");
	if (!file) return error("FileParser", "loadFile", "Failed to open file: " + path);

	size_t fileSize;
	if (!getFileSize(file, fileSize)) {
		fclose(file);
		return error("FileParser", "loadFile", "Failed to get file size");
	}

	out.resize(fileSize);

	size_t bytesRead = 0;
	while (bytesRead < fileSize) {
		size_t byteRead = fread(&out[bytesRead], 1, fileSize - bytesRead, file);
		if (byteRead == 0) {
			if (ferror(file)) {
				fclose(file);
				out.clear();
				return error("FileParser", "loadFile", "fread failed at byte " + std::to_string(bytesRead));
			}
			break;
		}
		bytesRead += byteRead;
	}

	fclose(file);

	if (bytesRead != fileSize) {
		out.clear();
		return error("FileParser", "loadFile", "fread failed. Expected " + std::to_string(fileSize) + ", got " + std::to_string(bytesRead));
	}
	return true;
}
bool Parser::loadBinaryFile(std::vector<unsigned char>& dataOut, const std::string& path) {
	FILE* file = fopen(path.c_str(), "rb");
	if (!file) return error("FileParser", "loadBinaryFile", "Failed to open file: " + path);

	size_t fileSize;
	if (!getFileSize(file, fileSize)) {
		fclose(file);
		return error("FileParser", "loadBinaryFile", "Failed to get file size");
	}

	dataOut.resize(fileSize);
	size_t bytesRead = fread(dataOut.data(), 1, fileSize, file);
	fclose(file);

	if (bytesRead != fileSize) {
		dataOut.clear();
		return error("FileParser", "loadBinaryFile", "fread failed. Expected " + std::to_string(fileSize) + ", got " + std::to_string(bytesRead));
	}

	return true;
}

bool Parser::parseBool(const unsigned char*& p, bool& out) {
	p = skipWhitespace(p);
	if (!p || *p == '\0') return false;

	if (*p == '1') { ++p; out = true;  return true; }
	if (*p == '0') { ++p; out = false; return true; }

	unsigned char tok[6]{};
	if (!parseToken(p, tok, sizeof(tok))) return false;

	const char* str = reinterpret_cast<const char*>(tok);
	if (strcmp(str, "true") == 0 || strcmp(str, "on") == 0) { out = true; return true; }
	if (strcmp(str, "false") == 0 || strcmp(str, "off") == 0) { out = false; return true; }

	return false;
}
bool Parser::parseUInt(const unsigned char*& p, unsigned int& out) {
	p = skipWhitespace(p);
	if (!p || *p == '\0' || *p < '0' || *p > '9') return false;

	out = 0;
	while (*p >= '0' && *p <= '9') out = out * 10 + (*p++ - '0');
	return true;
}

static inline int parseFloatSign(const unsigned char*& p) {
	int s = 1;
	if (*p == '+') ++p;
	else if (*p == '-') { ++p; s = -1; }
	return s;
}
static inline int parseFloatDigit(const unsigned char*& p, unsigned long long& u) {
	int n = 0;
	while (*p >= '0' && *p <= '9') {
		u = u * 10ULL + (unsigned long long)(*p - '0');
		++p;
		++n;
	}
	return n;
}
static const double POW10_NEG9[10] = {
	1.0, 1e-1, 1e-2, 1e-3, 1e-4, 1e-5, 1e-6, 1e-7, 1e-8, 1e-9
};
bool Parser::parseFloat(const unsigned char*& p, float& out) {
	p = skipWhitespace(p);
	if (!p || *p == '\0') return false;

	const unsigned char* start = p;
	int sign = parseFloatSign(p);
	unsigned long long i = 0ULL;
	int iCount = parseFloatDigit(p, i);

	double val = static_cast<double>(i);
	bool hasFrac{ 0 };
	if (*p == '.') {
		++p;
		unsigned long long f = 0ULL;
		int kept = 0;

		while (*p >= '0' && *p <= '9') {
			hasFrac = 1;
			if (kept < 9) {
				f = f * 10ULL + static_cast<unsigned long long>(*p - '0');
				++kept;
			}
			++p;
		}
		if (kept > 0) val += static_cast<double>(f) * POW10_NEG9[kept];
	}

	if (iCount == 0 && !hasFrac) { 
		p = start;
		return false;
	}

	if (*p == 'e' || *p == 'E') {
		++p;
		int esign = 1;
		if (*p == '+') ++p;
		else if (*p == '-') { esign = -1; ++p; }

		int any = 0, e = 0;
		while (*p >= '0' && *p <= '9') {
			any = 1;
			if (e < 100000000) e = e * 10 + (*p - '0');
			++p;
		}
		if (!any) { 
			p = start; 
			return false; 
		}

		static const double P10P[39] = {
			1.0,1e1,1e2,1e3,1e4,1e5,1e6,1e7,1e8,1e9,
			1e10,1e11,1e12,1e13,1e14,1e15,1e16,1e17,1e18,1e19,
			1e20,1e21,1e22,1e23,1e24,1e25,1e26,1e27,1e28,1e29,
			1e30,1e31,1e32,1e33,1e34,1e35,1e36,1e37,1e38
		};
		static const double P10N[39] = {
			1.0,1e-1,1e-2,1e-3,1e-4,1e-5,1e-6,1e-7,1e-8,1e-9,
			1e-10,1e-11,1e-12,1e-13,1e-14,1e-15,1e-16,1e-17,1e-18,1e-19,
			1e-20,1e-21,1e-22,1e-23,1e-24,1e-25,1e-26,1e-27,1e-28,1e-29,
			1e-30,1e-31,1e-32,1e-33,1e-34,1e-35,1e-36,1e-37,1e-38
		};

		if (esign >= 0) val *= (e > 38 ? 1e308 : P10P[e]);
		else            val *= (e > 38 ? 0.0   : P10N[e]);
	}

	if (sign < 0) val = -val;
	out = static_cast<float>(val);
	return true;
}

bool Parser::parseToken(const unsigned char*& p, unsigned char* out, const size_t maxLength) {
	if (!p || !out || maxLength == 0) return false;
	p = skipWhitespace(p);
	if (!p || *p == '\0') return false;

	size_t i = 0;
	while (*p && !isDelim(*p) && i + 1 < maxLength) out[i++] = *p++;
	out[i] = '\0';
	return true;
}

bool Parser::parseVec2f(const unsigned char*& p, Vec2<float>& out) {
	return parseFloat(p, out.x) && parseFloat(p, out.y);
}
bool Parser::parseVec3f(const unsigned char*& p, Vec3<float>& out) {
	return parseFloat(p, out.x) && parseFloat(p, out.y) && parseFloat(p, out.z);
}
bool Parser::parseVec4f(const unsigned char*& p, Vec4<float>& out) {
	return parseFloat(p, out.x) && parseFloat(p, out.y) && parseFloat(p, out.z) && parseFloat(p, out.w);
}

const unsigned char* Parser::skipToNextLine(const unsigned char* p) {
	if (!p) return nullptr;
	while (*p && *p != '\n' && *p != '\r') ++p;
	if (*p == '\r') ++p;
	if (*p == '\n') ++p;
	return p;
}
const unsigned char* Parser::skipWhitespace(const unsigned char* p, bool skipComma) {
	while (p && *p && isDelim(*p, skipComma)) ++p;
	return p ? p : nullptr;
}
const unsigned char* Parser::trimEOL(const unsigned char* p, const unsigned char* end) {
	while (end && *end && end > p && (end[-1] == '\n' || end[-1] == '\r' || end[-1] == ',')) --end;
	return end ? end : nullptr;
}

bool Parser::getFileSize(FILE* file, size_t& sizeOut) const {
	if (fseek(file, 0, SEEK_END) != 0) return error("FileParser", "getFileSize", "Failed to seek end of file");

	const long size = ftell(file);
	if (size == -1L) return error("FileParser", "getFileSize", "Invalid file, ftell failed");

	if (size <= 0 || static_cast<size_t>(size) > MAX_SIZE)
		return error("FileParser", "getFileSize", "Invalid file size");

	if (fseek(file, 0, SEEK_SET) != 0) return error("FileParser", "getFileSize", "Failed to rewind file");

	sizeOut = static_cast<size_t>(size);
	return true;
}

bool Parser::isDelim(unsigned char c, bool comma) {
	return c == 0 || c == ' ' || c == '\t' || c == '\n' || c == '\r' || (comma && c == ',');
}