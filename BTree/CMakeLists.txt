cmake_minimum_required(VERSION 3.10)
project(BTreeProject)

# 使用 C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置调试构建类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Debug 模式下开启调试信息，关闭优化
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

# 包含头文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)

# 添加可执行文件
add_executable(BTree
    src/main.cc
)