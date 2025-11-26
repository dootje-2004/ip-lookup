cmake_minimum_required(VERSION 3.20)

# TODO: All CMakeLists probably need some cleanup.

project(ip-lookup C CXX)

option(ENABLE_COVERAGE "Enable code coverage reporting" OFF)

function(enable_coverage target)
  if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${target} PRIVATE -fprofile-arcs -ftest-coverage --coverage -O0 -g)
    target_link_options(${target} PRIVATE --coverage)
  endif()
endfunction()

set(CMAKE_C_STANDARD 99) # C standard is C99
set(CMAKE_CXX_STANDARD 20) # C++ standard is C++20
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
# Build the compilation database (https://stackoverflow.com/a/73523313):
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Wpedantic)
include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/test/data")

enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)

add_subdirectory(src)
add_subdirectory(test)

# Code coverage
add_custom_target(coverage
  COMMAND lcov --capture --directory ./src/ --output-file coverage.info --gcov-tool gcov
  COMMAND lcov --list coverage.info
  COMMAND genhtml coverage.info --output-directory html
)
