set(Sources
main.cpp ipv4.cpp
)

set(TestName hello-test)

add_executable(${TestName} ${Sources})

target_link_libraries(${TestName} PUBLIC
    gtest_main
    iplib
)

add_test(
    NAME ${TestName}
    COMMAND ${TestName}
)

add_dependencies(coverage ${TestName})
