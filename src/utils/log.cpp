#include "StarletSerializer/utils/log.hpp"

#include <cstdio>

namespace Starlet::Serializer {
	bool error(const char* caller, const char* function, const std::string& msg) {
		fprintf(stderr, "[%s %s ERROR] %s\n", caller, function, msg.c_str());
		return false;
	}
	bool debugLog(const char* caller, const char* function, const std::string& msg, const bool returnValue) {
#ifndef NDEBUG
		printf("[%s %s LOG] %s\n", caller, function, msg.c_str());
#endif
		return returnValue;
	}
}

