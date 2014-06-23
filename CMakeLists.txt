cmake_minimum_required(VERSION 2.8)

project(textip)

if (${CMAKE_BUILD_TYPE} STREQUAL Release)
  add_definitions(-DRELEASE)
endif()

set(CMAKE_CXX_COMPILER g++-4.9)

add_definitions(--std=c++1y -Wall -Werror -Wextra -pedantic-errors)

enable_testing()

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS unit_test_framework)

add_subdirectory(test)
