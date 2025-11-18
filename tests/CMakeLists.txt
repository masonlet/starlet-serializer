add_executable(${PROJECT_NAME}_tests
  image_parser_test.cpp
  mesh_parser_test.cpp
  parser_test.cpp
  scene_parser_test.cpp
)

target_link_libraries(${PROJECT_NAME}_tests
  PRIVATE
    ${PROJECT_NAME}
    GTest::gtest_main
)

set_target_properties(${PROJECT_NAME}_tests PROPERTIES
  FOLDER "Tests"
)


include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}_tests)
