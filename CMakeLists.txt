cmake_minimum_required(VERSION 3.16)
project(ip-lookup)
add_executable(hello src/main.cpp)
target_compile_features(hello PUBLIC cxx_std_20)
