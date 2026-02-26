## tests/units

find_package(gtest REQUIRED)

enable_testing()

add_executable(tests_units main.cc)

# target_sources(tests_units PRIVATE
# )

target_link_libraries(tests_units PRIVATE wndx::attila::dev)

if(TARGET code_coverage)
  target_link_libraries(tests_units PRIVATE code_coverage)
endif()

target_link_libraries(tests_units PRIVATE gtest::gtest)

## enable CMake’s test runner to discover the tests included in the binary (for ctest)
include(GoogleTest)
gtest_discover_tests(tests_units
  ## explicitly state where should lay Testing/Temporary/ etc.
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)
