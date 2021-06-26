cmake_minimum_required(VERSION 2.8)

project(test_cmake CXX)

add_library(greetings STATIC hello.cpp good_morning.cpp)

add_executable(a.out main.cpp)

target_link_libraries(a.out greetings)
