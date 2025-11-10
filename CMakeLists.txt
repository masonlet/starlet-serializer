cmake_minimum_required(VERSION 3.20)

# Config
set(SER_NAME starlet_serializer)
set(SER_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(SER_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/inc")

if(NOT TARGET ${SER_NAME})
  # Fetch dependencies if not building locally
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
        GIT_TAG main
      )
    endif()

    FetchContent_MakeAvailable(starlet_math starlet_logger)
  endif()

  add_library(${SER_NAME} STATIC)

  file(GLOB_RECURSE SER_SRC CONFIGURE_DEPENDS ${SER_SRC_DIR}/*.cpp)
  file(GLOB_RECURSE SER_HEADERS CONFIGURE_DEPENDS ${SER_INC_DIR}/starlet-serializer/*.hpp)

  target_sources(${SER_NAME} PRIVATE ${SER_SRC} ${SER_HEADERS})

  target_include_directories(${SER_NAME} 
    PUBLIC 
      $<BUILD_INTERFACE:${SER_INC_DIR}> 
      $<INSTALL_INTERFACE:include>
  )

  target_link_libraries(${SER_NAME} 
    PUBLIC 
      starlet_math 
      starlet_logger
  )

  # IDE organization
  source_group(TREE ${SER_SRC_DIR} PREFIX "Source Files" FILES ${SER_SRC})
  source_group(TREE ${SER_INC_DIR} PREFIX "Header Files" FILES ${SER_HEADERS})
endif()
