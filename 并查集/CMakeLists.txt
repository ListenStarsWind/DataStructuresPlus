cmake_minimum_required(VERSION 3.10)
project(DisjointSet)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含头文件路径
include_directories(include)

# 添加可执行文件
add_executable(disjoint_set_example src/main.cc)
