#pragma once

namespace Starlet::Serializer {
	struct BmpData {
		int32_t width{ 0 }, height{ 0 };
		std::vector<uint8_t> pixels;
		uint8_t  pixelSize{ 0 };
		size_t   byteSize{ 0 };
	};
}
