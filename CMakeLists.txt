cmake_minimum_required(VERSION 2.8)
project("hm")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wfatal-errors -std=c++17")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
find_package(RAGEL 6.6 REQUIRED)
RAGEL_TARGET(lexer lexer.rl ${CMAKE_BINARY_DIR}/lexer.cpp)

set(SOURCES
    main.cpp
    parser.cpp
    semantic.cpp
    type_env.cpp
    types.cpp
    ${RAGEL_lexer_OUTPUTS}
)

include_directories(${CMAKE_SOURCE_DIR})
add_executable(hm ${SOURCES})
