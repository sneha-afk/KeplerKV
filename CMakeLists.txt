cmake_minimum_required(VERSION 3.10)

project(KeplerKV VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/syntax_tree.cpp
    src/lexer.cpp
    src/parser.cpp
    src/handler.cpp
    src/store_value.cpp
    src/store.cpp
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -pedantic
)
