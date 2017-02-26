cmake_minimum_required(VERSION 3.7)

project(Gold)

set(GTEST_libs gtest gtest_main)
set(GOLD_CPLUSPLUS_HOME ${PROJECT_SOURCE_DIR}/cplusplus/Gold)
set(NEW_MATH_HOME ${GOLD_CPLUSPLUS_HOME}/new_math)
set(SOURCES ${NEW_MATH_HOME}/utils/utils.cpp)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

set(CMAKE_CXX_FLAGS "-Wall -Werror -std=c++0x")

include_directories(${NEW_MATH_HOME}/utils)

add_library(gold_math SHARED ${SOURCES})
add_executable(utils_test GLOB ${NEW_MATH_HOME}/utils/*_test.cxx
