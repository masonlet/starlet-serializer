cmake_minimum_required(VERSION 3.20)

set(SER_NAME StarletSerializer)
set(SER_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(SER_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/inc")

include(FetchContent)
if(NOT TARGET StarletMath)
  FetchContent_Declare(StarletMath 
    GIT_REPOSITORY https://github.com/masonlet/starletmath.git 
    GIT_TAG main
  )
  FetchContent_MakeAvailable(StarletMath)
endif()
if(NOT TARGET StarletLogger)
  FetchContent_Declare(StarletLogger
    GIT_REPOSITORY https://github.com/masonlet/starletlogger.git 
    GIT_TAG main
  )
  FetchContent_MakeAvailable(StarletLogger)
endif()

if(NOT TARGET ${SER_NAME})
  add_library(${SER_NAME} STATIC)

  file(GLOB_RECURSE SER_SRC CONFIGURE_DEPENDS ${SER_SRC_DIR}/*.cpp)
  file(GLOB_RECURSE SER_HEADERS CONFIGURE_DEPENDS ${SER_INC_DIR}/${SER_NAME}/*.hpp)

  source_group(TREE ${SER_SRC_DIR} PREFIX "Source Files" FILES ${SER_SRC})
  source_group(TREE ${SER_INC_DIR} PREFIX "Header Files" FILES ${SER_HEADERS})

  target_sources(${SER_NAME} PRIVATE ${SER_SRC} ${SER_HEADERS})
  target_include_directories(${SER_NAME} PUBLIC $<BUILD_INTERFACE:${SER_INC_DIR}> $<INSTALL_INTERFACE:include>)
  target_link_libraries(${SER_NAME} PUBLIC StarletMath StarletLogger)
endif()
