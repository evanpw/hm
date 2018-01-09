cmake_minimum_required(VERSION 2.8)
project("hm")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -g -Wall -Wfatal-errors -std=c++17")

# Ragel, for building the lexer
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
find_package(RAGEL 6.6 REQUIRED)
RAGEL_TARGET(lexer lexer.rl ${CMAKE_BINARY_DIR}/lexer.cpp)

# Google test, for unit / integration tests
include(ExternalProject)
ExternalProject_Add(
    gtest
    URL https://github.com/google/googletest/archive/master.zip
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
    INSTALL_COMMAND ""
)
set(GTEST_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/gtest/src/gtest/googletest/include)
set(GTEST_LIBS_DIR ${CMAKE_CURRENT_BINARY_DIR}/gtest/src/gtest-build/googlemock/gtest)

## Library
set(SOURCES
    parser.cpp
    semantic.cpp
    type_env.cpp
    types.cpp
    ${RAGEL_lexer_OUTPUTS}
)

include_directories(${CMAKE_SOURCE_DIR})
add_library(hm ${SOURCES})

## Main executable
add_executable(hmc main.cpp)
target_link_libraries(hmc hm)

## Tests
enable_testing()
set(PROJECT_TEST_NAME run-tests)
include_directories(${GTEST_INCLUDE_DIRS})

file(GLOB TEST_SRC_FILES ${PROJECT_SOURCE_DIR}/test/*.cpp)
add_executable(${PROJECT_TEST_NAME} ${TEST_SRC_FILES})
target_link_libraries(${PROJECT_TEST_NAME} hm)
add_dependencies(${PROJECT_TEST_NAME} gtest)

target_link_libraries(${PROJECT_TEST_NAME}
    ${GTEST_LIBS_DIR}/libgtest.a
    ${GTEST_LIBS_DIR}/libgtest_main.a)
