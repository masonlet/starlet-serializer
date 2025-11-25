#include "../test_helpers.hpp"

namespace {
  class BmpBuilder {
  public:
    BmpBuilder(uint32_t width, int32_t height) : width(width), height(height) {
      const size_t rowStride = (width * 3 + 3) & ~3;
      const uint32_t absHeight = (height > 0) ? height : -height;
      const size_t pixelDataSize = rowStride * absHeight;
      data.resize(54 + pixelDataSize, 0);
      writeHeader();
    }

    BmpBuilder& setTopDown() {
      height = (height > 0) ? -height : height; 
      writeHeader(); 
      return *this; 
    }
    BmpBuilder& setPixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b) {
      const size_t rowStride = (width * 3 + 3) & ~3;
      size_t offset = 54 + y * rowStride + x * 3;
      data[offset] = b; data[offset + 1] = g; data[offset + 2] = r;
      return *this;
    }

    BmpBuilder& setFileSize(uint32_t size)      { writeU32(2, size);    return *this; }
    BmpBuilder& setDataOffset(uint32_t offset)  { writeU32(10, offset); return *this; }
    BmpBuilder& setDibHeaderSize(uint32_t size) { writeU32(14, size);   return *this; }
    BmpBuilder& setPlanes(uint16_t planes)      { writeU16(26, planes); return *this; }
    BmpBuilder& setBpp(uint16_t bpp)            { writeU16(28, bpp);    return *this; }
    BmpBuilder& setCompression(uint32_t comp)   { writeU32(30, comp);   return *this; }

    BmpBuilder& corruptSignature() { data[0] = 'X'; data[1] = 'Y'; return *this; }

    std::string build() const { return data; }

  private:
    void writeHeader() {
      data[0] = 'B'; data[1] = 'M';
      writeU32(2, static_cast<uint32_t>(data.size()));
      writeU32(10, 54);
      writeU32(14, 40);
      writeU32(18, width);
      writeI32(22, height);
      writeU16(26, 1);
      writeU16(28, 24);
      writeU32(30, 0);
    }

    void writeU32(size_t offset, uint32_t val) {
      *reinterpret_cast<uint32_t*>(&data[offset]) = val;
    }
    void writeI32(size_t offset, int32_t val) {
      *reinterpret_cast<int32_t*>(&data[offset]) = val;
    }
    void writeU16(size_t offset, uint16_t val) {
      *reinterpret_cast<uint16_t*>(&data[offset]) = val;
    }

    std::string data;
    uint32_t width;
    int32_t height;
  };
}

class BmpParserTest : public ImageParserTest {};

TEST_F(BmpParserTest, ValidMinimal) {
  std::string bmp = BmpBuilder(2, 2)
    .setPixel(0, 0, 0, 0, 255)      // Bottom-left:  Blue
    .setPixel(1, 0, 0, 255, 0)      // Bottom-right: Green
    .setPixel(0, 1, 255, 0, 0)      // Top-left:     Red
    .setPixel(1, 1, 255, 255, 255)  // Top-right:    White
    .build();

  createTestFile("test_data/minimal.bmp", bmp);
  expectValidParse("test_data/minimal.bmp", 2, 2);

  EXPECT_EQ(out.pixels[0], 255);
}

TEST_F(BmpParserTest, TopDownOrientation) {
  std::string bmp = BmpBuilder(2, 2)
    .setPixel(0, 0, 0, 0, 255)      // Bottom-left:  Blue
    .setPixel(1, 0, 0, 255, 0)      // Bottom-right: Green  
    .setPixel(0, 1, 255, 0, 0)      // Top-left:     Red
    .setPixel(1, 1, 255, 255, 255)  // Top-right:    White
    .setTopDown()
    .build();

  createTestFile("test_data/topdown.bmp", bmp);
  expectValidParse("test_data/topdown.bmp", 2, 2);

  EXPECT_EQ(out.pixelSize, 3);
  EXPECT_EQ(out.byteSize, 12);
  EXPECT_EQ(out.pixels[0], 0);   // R
  EXPECT_EQ(out.pixels[1], 0);   // G
  EXPECT_EQ(out.pixels[2], 255); // B
}

TEST_F(BmpParserTest, CorrectPaddingWithGarbage) {
  std::string bmp = BmpBuilder(2, 2)
    .setPixel(0, 0, 0, 0, 255)
    .setPixel(1, 0, 0, 255, 0)
    .setPixel(0, 1, 255, 0, 0)
    .setPixel(1, 1, 255, 255, 255)
    .build();
  bmp[60] = 0xFF; bmp[61] = 0xAA; // Padding after first row
  bmp[68] = 0xFF; bmp[69] = 0xBB; // Padding after second row

  createTestFile("test_data/padding_garbage.bmp", bmp);
  expectValidParse("test_data/padding_garbage.bmp", 2, 2);

  // Verify padding doesn't affect pixel data
  EXPECT_EQ(out.pixels[0], 255); // R
  EXPECT_EQ(out.pixels[1], 0);   // G
  EXPECT_EQ(out.pixels[2], 0);   // B
}

TEST_F(BmpParserTest, OddWidthPadding) {
  std::string bmp = BmpBuilder(3, 1)
    .setPixel(0, 0, 255, 0, 0)
    .setPixel(1, 0, 0, 255, 0)
    .setPixel(2, 0, 0, 0, 255)
    .build();

  createTestFile("test_data/odd_width.bmp", bmp);
  expectValidParse("test_data/odd_width.bmp", 3, 1);

  EXPECT_EQ(out.byteSize, 9);
  EXPECT_EQ(out.pixels[0], 255); // R
  EXPECT_EQ(out.pixels[1], 0);   // G
  EXPECT_EQ(out.pixels[2], 0);   // B
  EXPECT_EQ(out.pixels[3], 0);   // R
  EXPECT_EQ(out.pixels[4], 255); // G
  EXPECT_EQ(out.pixels[5], 0);   // B
  EXPECT_EQ(out.pixels[6], 0);   // R
  EXPECT_EQ(out.pixels[7], 0);   // G
  EXPECT_EQ(out.pixels[8], 255); // B
}

TEST_F(BmpParserTest, TopDownFullPixelVerification) {
  std::string bmp = BmpBuilder(2, 2)
    .setPixel(0, 0, 0, 0, 255)
    .setPixel(1, 0, 0, 255, 0)
    .setPixel(0, 1, 255, 0, 0)
    .setPixel(1, 1, 255, 255, 255)
    .setTopDown()
    .build();

  createTestFile("test_data/topdown_full.bmp", bmp);
  expectValidParse("test_data/topdown_full.bmp", 2, 2);

  EXPECT_EQ(out.pixels[0], 0);    // Top-left: Blue
  EXPECT_EQ(out.pixels[1], 0);
  EXPECT_EQ(out.pixels[2], 255);
  EXPECT_EQ(out.pixels[9], 255);  // Bottom-right: White
  EXPECT_EQ(out.pixels[10], 255);
  EXPECT_EQ(out.pixels[11], 255);
}

TEST_F(BmpParserTest, LargeDimensions) {
  constexpr int size{ 101 };
  std::string bmp = BmpBuilder(size, size).build();

  createTestFile("test_data/large_dims.bmp", bmp);
  expectValidParse("test_data/large_dims.bmp", size, size);

  EXPECT_EQ(out.byteSize, size * size * 3);
  EXPECT_EQ(out.pixels.size(), out.width * out.height * out.pixelSize);
}

TEST_F(BmpParserTest, OddWidthLargeDimensions) {
  constexpr int size{ 333 };
  std::string bmp = BmpBuilder(size, size).build();

  createTestFile("test_data/large_odd_dims.bmp", bmp);
  expectValidParse("test_data/large_odd_dims.bmp", size, size);

  EXPECT_EQ(out.byteSize, size * size * 3);
  EXPECT_EQ(out.pixels.size(), out.width * out.height * out.pixelSize);
}

TEST_F(BmpParserTest, OddWidthLargePaddingCheck) {
  constexpr int size{ 333 };
  std::string bmp = BmpBuilder(size, size)
    .setPixel(0, 0, 255, 0, 0)
    .setPixel(0, 1, 0, 255, 0)
    .build();

  createTestFile("test_data/large_odd_padding.bmp", bmp);
  expectValidParse("test_data/large_odd_padding.bmp", size, size);

  size_t secondToLastRowPixelOffset = (size - 2) * size * 3;
  EXPECT_EQ(out.pixels[secondToLastRowPixelOffset], 0);       // R
  EXPECT_EQ(out.pixels[secondToLastRowPixelOffset + 1], 255); // G
  EXPECT_EQ(out.pixels[secondToLastRowPixelOffset + 2], 0);   // B
}

TEST_F(BmpParserTest, HeaderFileSizeTooSmall) {
  std::string bmp = BmpBuilder(2, 2).setFileSize(60).build();
  createTestFile("test_data/header_size_mismatch.bmp", bmp);
  expectValidParse("test_data/header_size_mismatch.bmp", 2, 2); // Should succeed using actual file size
}

TEST_F(BmpParserTest, OneByOneImage) {
  std::string bmp = BmpBuilder(1, 1)
    .setPixel(0, 0, 32, 64, 128)
    .build();

  createTestFile("test_data/one_by_one.bmp", bmp);
  expectValidParse("test_data/one_by_one.bmp", 1, 1);

  EXPECT_EQ(out.byteSize, 3);
  EXPECT_EQ(out.pixels[0], 32);  // R
  EXPECT_EQ(out.pixels[1], 64);  // G
  EXPECT_EQ(out.pixels[2], 128); // B
}


// Error Tests
TEST_F(BmpParserTest, FileTooSmall) {
  std::string bmp(20, '\0'); // Less than 54 bytes
  bmp[0] = 'B'; bmp[1] = 'M';
  createTestFile("test_data/too_small.bmp", bmp);

  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/too_small.bmp");
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("File too small: 21 bytes"), std::string::npos);
}

TEST_F(BmpParserTest, InvalidSignature) {
  std::string bmp = BmpBuilder(2, 2).corruptSignature().build();
  createTestFile("test_data/bad_sig.bmp", bmp);

  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/bad_sig.bmp");
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Bad signature (not BM)"), std::string::npos);
}

TEST_F(BmpParserTest, InvalidDataOffset) {
  std::string bmp = BmpBuilder(2, 2).setDataOffset(10000).build();
  createTestFile("test_data/bad_offset.bmp", bmp);

  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/bad_offset.bmp");
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Invalid data offset: 10000"), std::string::npos);
}

TEST_F(BmpParserTest, UnsupportedDIBHeader) {
  std::string bmp = BmpBuilder(2, 2).setDibHeaderSize(12).build();
  createTestFile("test_data/bad_dib.bmp", bmp);

  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/bad_dib.bmp");
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Unsupported DIB header size: 12"), std::string::npos);
}

TEST_F(BmpParserTest, InvalidColourPlanes) {
  std::string bmp = BmpBuilder(2, 2).setPlanes(2).build();
  createTestFile("test_data/bad_planes.bmp", bmp);

  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/bad_planes.bmp");
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Planes != 1: 2"), std::string::npos);
}

TEST_F(BmpParserTest, UnsupportedBitsPerPixel) {
  std::string bmp = BmpBuilder(2, 2).setBpp(32).build();
  createTestFile("test_data/bad_bpp.bmp", bmp);

  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/bad_bpp.bmp");
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Only 24bpp supported: 32"), std::string::npos);
}

TEST_F(BmpParserTest, CompressedBMP) {
  std::string bmp = BmpBuilder(2, 2).setCompression(1).build();
  createTestFile("test_data/compressed.bmp", bmp);

  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/compressed.bmp");
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Compressed BMP not supported: 1"), std::string::npos);
}

TEST_F(BmpParserTest, InsufficientPixelData) {
  std::string bmp = BmpBuilder(2, 2).build();
  bmp.resize(60);
  createTestFile("test_data/truncated.bmp", bmp);

  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/truncated.bmp");
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("File too small for declared dimensions: 61 bytes, need 70 bytes"), std::string::npos);
}

TEST_F(BmpParserTest, ZeroWidth) {
  std::string bmp = BmpBuilder(2, 2).build();
  *reinterpret_cast<uint32_t*>(&bmp[18]) = 0;
  createTestFile("test_data/zero_width.bmp", bmp);

  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/zero_width.bmp");
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Invalid width: 0"), std::string::npos);
}

TEST_F(BmpParserTest, ZeroHeight) {
  std::string bmp = BmpBuilder(2, 2).build();
  *reinterpret_cast<int32_t*>(&bmp[22]) = 0;
  createTestFile("test_data/zero_height.bmp", bmp);

  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/zero_height.bmp");
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Invalid height: 0"), std::string::npos);
}

TEST_F(BmpParserTest, FileNotFound) {
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/nonexistent.bmp");
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Failed to open file: test_data/nonexistent.bmp"), std::string::npos);
}

TEST_F(BmpParserTest, DataOffsetEqualsFileSize) {
  std::string bmp = BmpBuilder(1, 1).setDataOffset(54).build();
  bmp.resize(54);
  createTestFile("test_data/offset_at_end.bmp", bmp);

  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/offset_at_end.bmp");
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Invalid data offset: 54"), std::string::npos);
}