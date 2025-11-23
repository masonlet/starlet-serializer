#include "../test_helpers.hpp"
#include <cstdint>

namespace {
  class TgaBuilder {
  public:
    TgaBuilder(uint16_t width, uint16_t height, uint8_t bpp = 24)
      : width(width), height(height), bpp(bpp) {
      const size_t pixelSize = bpp / 8;
      const size_t pixelDataSize = static_cast<size_t>(width) * height * pixelSize;
      data.resize(18 + pixelDataSize, 0);
      writeHeader();
    }

    TgaBuilder& setTopDown() {
      data[17] |= 0x20;
      return *this;
    }

    TgaBuilder& setIdField(const std::string& id) {
      data[0] = static_cast<uint8_t>(id.size());
      data.insert(data.begin() + 18, id.begin(), id.end());
      return *this;
    }

    TgaBuilder& setPixel(uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
      const size_t idLen = data[0];
      const size_t pixelSize = bpp / 8;
      const size_t offset = 18 + idLen + (static_cast<size_t>(y) * width + x) * pixelSize;

      data[offset] = b;
      data[offset + 1] = g;
      data[offset + 2] = r;
      if (bpp == 32) data[offset + 3] = a;
      return *this;
    }

    TgaBuilder& setImageType(uint8_t type)    { data[2] = type; return *this; }
    TgaBuilder& setColorMapType(uint8_t type) { data[1] = type; return *this; }
    TgaBuilder& setBpp(uint8_t bpp)           { data[16] = bpp; this->bpp = bpp; return *this; }
    TgaBuilder& setIdLength(uint8_t len)      { data[0] = len; return *this; }

    std::string build() const { return data; }

  private:
    void writeHeader() {
      data[0] = 0;  // ID length
      data[1] = 0;  // Colour map type
      data[2] = 2;  // Image type (uncompressed true-colour)
      *reinterpret_cast<uint16_t*>(&data[12]) = width;
      *reinterpret_cast<uint16_t*>(&data[14]) = height;
      data[16] = bpp;
      data[17] = 0;  // Image descriptor
    }

    std::string data;
    uint16_t width;
    uint16_t height;
    uint8_t bpp;
  };
}

class TgaParserTest : public ImageParserTest {};

TEST_F(TgaParserTest, ValidMinimal) {
  std::string tga = TgaBuilder(2, 2)
    .setPixel(0, 0, 0, 0, 255)
    .setPixel(1, 0, 0, 255, 0)
    .setPixel(0, 1, 255, 0, 0)
    .setPixel(1, 1, 255, 255, 255)
    .build();

  createTestFile("test_data/minimal.tga", tga);
  expectValidParse("test_data/minimal.tga", 2, 2);

  EXPECT_EQ(out.pixels[0], 255);
}

TEST_F(TgaParserTest, TopDownOrientation) {
  std::string tga = TgaBuilder(2, 2)
    .setPixel(0, 0, 0, 0, 255)
    .setPixel(1, 0, 0, 255, 0)
    .setPixel(0, 1, 255, 0, 0)
    .setPixel(1, 1, 255, 255, 255)
    .build();

  tga[17] = 0x20; // Set top-down bit

  createTestFile("test_data/topdown.tga", tga);
  expectValidParse("test_data/topdown.tga", 2, 2);

  EXPECT_EQ(out.pixelSize, 3);
  EXPECT_EQ(out.byteSize, 12);
  EXPECT_EQ(out.pixels[0], 0);
  EXPECT_EQ(out.pixels[1], 0);
  EXPECT_EQ(out.pixels[2], 255);
}

TEST_F(TgaParserTest, Tga32Bit) {
  std::string tga = TgaBuilder(2, 2, 32)
    .setPixel(0, 0, 255, 0, 0, 255)
    .setPixel(1, 0, 0, 255, 0, 255)
    .setPixel(0, 1, 0, 0, 255, 255)
    .setPixel(1, 1, 255, 255, 255, 255)
    .setTopDown()
    .build();

  createTestFile("test_data/tga32.tga", tga);
  expectValidParse("test_data/tga32.tga", 2, 2);

  EXPECT_EQ(out.pixelSize, 3);
  EXPECT_EQ(out.byteSize, 12);
  EXPECT_EQ(out.pixels[0], 255);
  EXPECT_EQ(out.pixels[1], 0);
  EXPECT_EQ(out.pixels[2], 0);
}

TEST_F(TgaParserTest, WithIdField) {
  std::string tga = TgaBuilder(2, 2)
    .setIdField("ABCDEFGH")
    .setPixel(0, 0, 255, 0, 0)
    .setPixel(1, 0, 0, 255, 0)
    .setPixel(0, 1, 0, 0, 255)
    .setPixel(1, 1, 255, 255, 255)
    .build();

  createTestFile("test_data/with_id.tga", tga);
  expectValidParse("test_data/with_id.tga", 2, 2);
}

TEST_F(TgaParserTest, TopDownFullPixelVerification) {
  std::string tga = TgaBuilder(2, 2)
    .setPixel(0, 0, 0, 0, 255)
    .setPixel(1, 0, 0, 255, 0)
    .setPixel(0, 1, 255, 0, 0)
    .setPixel(1, 1, 255, 255, 255)
    .build();

  tga[17] = 0x20;
  createTestFile("test_data/topdown_full.tga", tga);
  expectValidParse("test_data/topdown_full.tga", 2, 2);

  EXPECT_EQ(out.pixels[0], 0);
  EXPECT_EQ(out.pixels[1], 0);
  EXPECT_EQ(out.pixels[2], 255);
  EXPECT_EQ(out.pixels[9], 255);
  EXPECT_EQ(out.pixels[10], 255);
  EXPECT_EQ(out.pixels[11], 255);
}

TEST_F(TgaParserTest, LargeDimensions) {
  const uint32_t size = 101;
  std::string tga = TgaBuilder(size, size).build();

  createTestFile("test_data/large_dims.tga", tga);
  expectValidParse("test_data/large_dims.tga", size, size);

  EXPECT_EQ(out.byteSize, size * size * 3);
  EXPECT_EQ(out.pixels.size(), out.width * out.height * out.pixelSize);
}

TEST_F(TgaParserTest, OneByOneImage) {
  std::string tga = TgaBuilder(1, 1)
    .setPixel(0, 0, 32, 64, 128)
    .build();

  createTestFile("test_data/one_by_one.tga", tga);
  expectValidParse("test_data/one_by_one.tga", 1, 1);

  EXPECT_EQ(out.byteSize, 3);
  EXPECT_EQ(out.pixels[0], 32);
  EXPECT_EQ(out.pixels[1], 64);
  EXPECT_EQ(out.pixels[2], 128);
}


// Error Tests
TEST_F(TgaParserTest, FileTooSmall) {
  std::string tga(10, '\0');
  createTestFile("test_data/too_small.tga", tga);

  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/too_small.tga");
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("File too small: 10 bytes"), std::string::npos);
}

TEST_F(TgaParserTest, UnsupportedImageType) {
  std::string tga = TgaBuilder(2, 2)
    .setPixel(0, 0, 0, 0, 255)
    .setPixel(1, 0, 0, 255, 0)
    .setPixel(0, 1, 255, 0, 0)
    .setPixel(1, 1, 255, 255, 255)
    .build();
  tga[2] = 1; // RLE compressed
  createTestFile("test_data/bad_type.tga", tga);

  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/bad_type.tga");
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Unsupported TGA image type"), std::string::npos);
}

TEST_F(TgaParserTest, WithColorMap) {
  std::string tga = TgaBuilder(2, 2)
    .setPixel(0, 0, 0, 0, 255)
    .setPixel(1, 0, 0, 255, 0)
    .setPixel(0, 1, 255, 0, 0)
    .setPixel(1, 1, 255, 255, 255)
    .build();
  tga[1] = 1; // Has colour map
  createTestFile("test_data/with_colourmap.tga", tga);

  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/with_colourmap.tga");
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Unsupported TGA colour map type"), std::string::npos);
}

TEST_F(TgaParserTest, UnsupportedBitsPerPixel) {
  std::string tga = TgaBuilder(2, 2)
    .setPixel(0, 0, 0, 0, 255)
    .setPixel(1, 0, 0, 255, 0)
    .setPixel(0, 1, 255, 0, 0)
    .setPixel(1, 1, 255, 255, 255)
    .build();
  tga[16] = 16;
  createTestFile("test_data/bad_bpp.tga", tga);

  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/bad_bpp.tga");
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Unsupported TGA bits per pixel"), std::string::npos);
}

TEST_F(TgaParserTest, InvalidDataOffset) {
  std::string tga = TgaBuilder(2, 2)
    .setPixel(0, 0, 0, 0, 255)
    .setPixel(1, 0, 0, 255, 0)
    .setPixel(0, 1, 255, 0, 0)
    .setPixel(1, 1, 255, 255, 255)
    .build();
  tga[0] = 100; // ID length beyond file size
  createTestFile("test_data/bad_offset.tga", tga);

  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/bad_offset.tga");
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Invalid data offset"), std::string::npos);
}

TEST_F(TgaParserTest, InsufficientPixelData) {
  std::string tga = TgaBuilder(2, 2).build();
  tga.resize(21);
  createTestFile("test_data/truncated.tga", tga);

  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/truncated.tga");
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("File too small for declared dimensions: 21 bytes, need 30 bytes"), std::string::npos);
}

TEST_F(TgaParserTest, ZeroWidth) {
  std::string tga = TgaBuilder(2, 2).build();
  *reinterpret_cast<uint16_t*>(&tga[12]) = 0;
  createTestFile("test_data/zero_width.tga", tga);

  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/zero_width.tga");
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Invalid width: 0"), std::string::npos);
}
  
TEST_F(TgaParserTest, ZeroHeight) {
  std::string tga = TgaBuilder(2, 2).build();
  *reinterpret_cast<int16_t*>(&tga[14]) = 0;
  createTestFile("test_data/zero_height.tga", tga);

  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/zero_height.tga");
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Invalid height: 0"), std::string::npos);
}

TEST_F(TgaParserTest, FileNotFound) {
  testing::internal::CaptureStderr();
  expectInvalidParse("test_data/nonexistent.tga");
  const std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("Failed to open file: test_data/nonexistent.tga"), std::string::npos);
}