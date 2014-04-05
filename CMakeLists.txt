cmake_minimum_required(VERSION 2.8)

project(textip)

add_definitions(--std=c++11 -Wall -Werror -Wextra -pedantic-errors)

enable_testing()

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS unit_test_framework)

add_subdirectory(trie)
add_subdirectory(test)

add_subdirectory(utils)