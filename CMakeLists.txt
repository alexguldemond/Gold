cmake_minimum_required(VERSION 3.7)

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
                 ${CMAKE_BINARY_DIR}/googletest-build)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()

project(Gold)

enable_testing()
set(GTEST_libs gtest gtest_main)
set(GOLD_CPLUSPLUS_HOME ${PROJECT_SOURCE_DIR}/cplusplus)
set(NEW_MATH_HOME ${GOLD_CPLUSPLUS_HOME}/Gold/new_math)
set(SOURCES ${NEW_MATH_HOME}/utils/utils.cpp ${NEW_MATH_HOME}/simplify/simplify.cpp)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

set(CMAKE_CXX_FLAGS "-Wall -Werror -std=c++0x")

include_directories(${GOLD_CPLUSPLUS_HOME})

add_library(gold_math SHARED ${SOURCES})

# math/utils/utils_test
add_executable(math_utils_test ${NEW_MATH_HOME}/utils/utils_test.cxx)
target_link_libraries(math_utils_test gtest_main gold_math)
add_test(NAME math_utils_test COMMAND math_utils_test)

# math/simplify/simplify_test
add_executable(math_simplify_test ${NEW_MATH_HOME}/simplify/simplify_test.cxx)
target_link_libraries(math_simplify_test gtest_main gold_math)
add_test(NAME math_simplify_test COMMAND math_simplify_test)
