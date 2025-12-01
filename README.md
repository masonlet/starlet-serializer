# Starlet Serializer

![Tests](https://github.com/masonlet/starlet-serializer/actions/workflows/test.yml/badge.svg)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://isocpp.org/std/the-standard)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](./LICENSE)

A lightweight serialization library for **Starlet** projects to handle both data reading and writing.

## Features
### File Format Support
- **Images**:
  - BMP (24-bit)
  - TGA (24/32-bit uncompressed)
- **Meshes**:
  - PLY (ASCII w/ positions, normals, colors, texture coordinates)
  - OBJ (positions, normals, texture coordinates, vertex colors, n-gon triangulation)
- **Scenes**: Custom text-based scene format with models, lights, cameras, textures, primitives

### Core Utilities
- **File I/O**: Binary and text file loading
- **Parsing Primitives**: 
  - Type-safe parsers: `parseBool`, `parseUInt`, `parseFloat`, `parseVec2f/3f/4f`
  - Token extraction with `parseToken`
  - Whitespace handling: `skipWhitespace`, `skipToNextLine`, `trimEOL`
  - Error-safe macros: `STARLET_PARSE_OR`, `STARLET_PARSE_STRING_OR`

<br/>


## Prerequisites
- C++20 or later
- CMake 3.20+
- **Dependencies**: 
  - [starlet-math](https://github.com/masonlet/starlet-math) (auto-fetched)
  - [starlet-logger](https://github.com/masonlet/starlet-logger) (auto-fetched)

## Installation

### Using as a Dependency
```cmake
include(FetchContent)

FetchContent_Declare(starlet_serializer
  GIT_REPOSITORY https://github.com/masonlet/starlet-serializer.git 
  GIT_TAG main
)
FetchContent_MakeAvailable(starlet_serializer)

target_link_libraries(app_name PRIVATE starlet_serializer)
```

### Building from Source
```bash
# Clone the repository
git clone https://github.com/masonlet/starlet-serializer.git
cd starlet-serializer

# Configure and build
cmake -B build
cmake --build build
```

<br/>


## Testing
```bash
# Configure with tests enabled
cmake -B build -DBUILD_TESTS=ON

# Build and run tests
cmake --build build
ctest --test-dir build --output-on-failure
```

<br/>


## License
MIT License - see [LICENSE](./LICENSE) for details.
