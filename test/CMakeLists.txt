set(Sources
main.cpp ip.cpp
)

set(TestName hello-test)

add_executable(${TestName} ${Sources})

target_link_libraries(${TestName} PUBLIC
    gtest_main
    # Add your library to test here
    # ExampleNestedTopLib
    iplib
)

add_test(
    NAME ${TestName}
    COMMAND ${TestName}
)

add_dependencies(coverage ${TestName})
