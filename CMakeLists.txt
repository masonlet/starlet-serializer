cmake_minimum_required(VERSION 3.20)
project(starlet_serializer VERSION 1.0.0)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(PROJECT_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(PROJECT_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/inc")

if(NOT TARGET ${PROJECT_NAME})
  if(NOT BUILD_LOCAL)
    include(FetchContent)

    if(NOT TARGET starlet_math)
      FetchContent_Declare(starlet_math
        GIT_REPOSITORY https://github.com/masonlet/starlet-math.git
        GIT_TAG main
      )
    endif()
    if(NOT TARGET starlet_logger)
      FetchContent_Declare(starlet_logger
        GIT_REPOSITORY https://github.com/masonlet/starlet-logger.git 
        GIT_TAG feature/unit-tests
      )
    endif()

    FetchContent_MakeAvailable(starlet_math starlet_logger)
  endif()

  add_library(${PROJECT_NAME} STATIC)

  file(GLOB_RECURSE PROJECT_SRC CONFIGURE_DEPENDS
    ${PROJECT_SRC_DIR}/*.cpp
  )
  file(GLOB_RECURSE PROJECT_HEADERS CONFIGURE_DEPENDS 
    ${PROJECT_INC_DIR}/starlet-serializer/*.hpp
  )

  target_sources(${PROJECT_NAME} 
    PRIVATE ${PROJECT_SRC} ${PROJECT_HEADERS})

  target_include_directories(${PROJECT_NAME} 
    PUBLIC 
      $<BUILD_INTERFACE:${PROJECT_INC_DIR}> 
      $<INSTALL_INTERFACE:include>
  )

  target_link_libraries(${PROJECT_NAME} 
    PUBLIC 
      starlet_math 
      starlet_logger
  )

  target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

  # IDE organization
  source_group(TREE ${PROJECT_SRC_DIR} PREFIX "Source Files" FILES ${PROJECT_SRC})
  source_group(TREE ${PROJECT_INC_DIR} PREFIX "Header Files" FILES ${PROJECT_HEADERS})
endif()

option(BUILD_TESTS "Build units tests" OFF)

if(BUILD_TESTS)
  enable_testing()

  include(FetchContent)
  FetchContent_Declare(
    googletet
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
  )

  # For Windows: Prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

  FetchContent_MakeAvailable(googletest)

  set_target_properties(gtest gtest_main gmock gmock_main PROPERTIES
    FOLDER "Tests/GoogleTest"
  )

  add_subdirectory(tests)
endif()
