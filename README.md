# Starlet Serializer
A lightweight serialization library for **Starlet** projects to handle both data reading and writing.

## Features
- **Logging Utils**
    - `error` and `debug` for console logging

- **File I/O**
    - **Parser** - `loadFile`, `loadBinaryFile`, both with basic safety checks
    - **Writer** - `saveScene`, for serializing scenes and components

- **Parser Utils**
    - Whitespace & line skipping helpers: `skipWhitespace`, `skipToNextLine`, `trimEOL`
    - Typed parsers: `parseUINT`, `parseBool`, `parseFloat`, `parseVec2f`, `parseVec3`, `parseVec3`
    - Token parsing with `parseToken`
    - Error-safe macros `PARSE_OR` and `PARSE_STRING_OR`

## Using as a Dependency
```cmake
include(FetchContent)

FetchContent_Declare(starlet_serializer
  GIT_REPOSITORY https://github.com/masonlet/starlet-serializer.git 
  GIT_TAG main
)
FetchContent_MakeAvailable(starlet_serializer)

target_link_libraries(app_name PRIVATE starlet_serializer)
```
