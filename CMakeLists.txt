cmake_minimum_required(VERSION 3.2)

# Download and unpack googletest at configure time
configure_file(.CMakeLists.txt.in googletest-download/CMakeLists.txt)
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

project(goldjs)

enable_testing()
set(GOLD_CPLUSPLUS_HOME ${PROJECT_SOURCE_DIR}/src/cplusplus)
set(GOLD_INCLUDE ${PROJECT_SOURCE_DIR}/include)
set(MATH_HOME ${GOLD_CPLUSPLUS_HOME}/math)

set(SOURCES 
  ${MATH_HOME}/utils/utils.cpp 
  ${MATH_HOME}/node/node.cpp
  ${MATH_HOME}/expression/expression.cpp 
  ${MATH_HOME}/expression/variable.cpp 
  ${MATH_HOME}/function/function.cpp 
)

set(CMAKE_CXX_FLAGS "-Wall -Werror -std=c++1y")

include_directories(${GOLD_INCLUDE})

add_library(gold_math OBJECT ${SOURCES})

# math/utils/utils_test
add_executable(math_utils_test ${MATH_HOME}/utils/utils_test.cxx $<TARGET_OBJECTS:gold_math>)
target_link_libraries(math_utils_test gtest_main)
add_test(NAME math_utils_test COMMAND math_utils_test)

# math/node/node_test
add_executable(math_node_test ${MATH_HOME}/node/node_test.cxx $<TARGET_OBJECTS:gold_math>)
target_link_libraries(math_node_test gtest_main)
add_test(NAME math_node_test COMMAND math_node_test)

# math/expression/expression_test
add_executable(math_expression_test ${MATH_HOME}/expression/expression_test.cxx $<TARGET_OBJECTS:gold_math>)
target_link_libraries(math_expression_test gtest_main)
add_test(NAME math_expression_test COMMAND math_expression_test)

# math/expression/expression_test
add_executable(math_function_test ${MATH_HOME}/function/function_test.cxx $<TARGET_OBJECTS:gold_math>)
target_link_libraries(math_function_test gtest_main)
add_test(NAME math_function_test COMMAND math_function_test)

# Setup coverage
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)
  include(CodeCoverage)
  APPEND_COVERAGE_COMPILER_FLAGS()
  set(COVERAGE_EXCLUDES '*gtest*' '*boost'* '*c++*' )
  setup_target_for_coverage(NAME coverage EXECUTABLE ctest)
endif()
