file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_executable(${PROJECT_NAME}_tests ${TEST_SOURCES})


target_link_libraries(${PROJECT_NAME}_tests
  PRIVATE
    ${PROJECT_NAME}
    GTest::gtest_main
)


source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Test Files" 
  FILES ${TEST_SOURCES}
)
set_target_properties(${PROJECT_NAME}_tests PROPERTIES
  FOLDER "Tests"
)


include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}_tests)
