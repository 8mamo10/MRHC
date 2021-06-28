cmake_minimum_required(VERSION 2.8)

project(test_cmake CXX)

#add_library(greetings STATIC hello.cpp good_morning.cpp)
#add_library(greetings SHARED hello.cpp good_morning.cpp)

option(GREETINGS_BUILD_SHARED_LIBS "build greetings as a shared library" OFF)

if (GREETINGS_BUILD_SHARED_LIBS)
  add_library(greetings SHARED hello.cpp good_morning.cpp)
else()
  add_library(greetings STATIC hello.cpp good_morning.cpp)
endif()

add_executable(a.out main.cpp)

target_link_libraries(a.out greetings)
