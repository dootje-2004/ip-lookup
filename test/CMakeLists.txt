set(TESTNAME ip-test)

set(SOURCES
    ipv4.cpp 
    ipv6.cpp 
    btree.cpp
)

add_executable(${TESTNAME} ${SOURCES})

target_link_libraries(${TESTNAME} PUBLIC
    GTest::gtest_main
    iplib
    btreelib
)

gtest_discover_tests(${TESTNAME})

enable_coverage(${TESTNAME})
