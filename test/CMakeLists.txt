set(Sources
main.cpp ipv4.cpp ipv6.cpp btree.cpp
)

set(TestName ip-test)

add_executable(${TestName} ${Sources})

target_link_libraries(${TestName} PUBLIC
    gtest
    gtest_main
    iplib
    btreelib
)

add_test(
    NAME ${TestName}
    COMMAND ${TestName}
    WORKING_DIRECTORY ../build
)

add_dependencies(coverage ${TestName})
