cmake_minimum_required(VERSION 3.10)

project(KeplerKV VERSION 1.3.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/util.cpp
    src/store_value.cpp
    src/store.cpp
    src/syntax_tree.cpp
    src/command_ast_nodes.cpp
    src/lexer.cpp
    src/parser.cpp
    src/handler.cpp
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -pedantic
)
