add_executable(${PROJECT_NAME}_tests
  serializer_test.cpp
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
