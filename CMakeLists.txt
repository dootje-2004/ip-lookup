cmake_minimum_required(VERSION 3.20)

project(ip-lookup C CXX)

option(BUILD_TESTS OFF)
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

add_compile_options(-Wall -Wextra -Wpedantic)
include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/test/data")

enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)

add_subdirectory(src)
add_subdirectory(test)




# Test options
if(BUILD_TESTS)
  set(MEMORYCHECK_COMMAND_OPTIONS "--show-leak-kinds=all --leak-check=full --error-exitcode=1")
  add_compile_options(-Wall -Wextra -Wpedantic -fprofile-arcs -ftest-coverage)
  link_libraries(gcov gtest gtest_main)
endif()

# Code coverage
set(COVERAGE_REPORT_DIR ${CMAKE_SOURCE_DIR}/report)
file(MAKE_DIRECTORY ${COVERAGE_REPORT_DIR})
add_custom_target(coverage
  COMMAND lcov --capture --directory ./src/ --output-file ${COVERAGE_REPORT_DIR}/coverage.info --gcov-tool gcov
  COMMAND lcov --list ${COVERAGE_REPORT_DIR}/coverage.info
  COMMAND genhtml ${COVERAGE_REPORT_DIR}/coverage.info --output-directory ${COVERAGE_REPORT_DIR}/html
)



enable_coverage(iplib)
enable_coverage(btreelib)
