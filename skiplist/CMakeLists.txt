cmake_minimum_required(VERSION 3.10)
project(skiplistProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

include_directories(${PROJECT_SOURCE_DIR}/include)

# 加入 AddressSanitizer（仅在 Debug 模式）
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
endif()

add_executable(skiplist
    src/main.cc
)