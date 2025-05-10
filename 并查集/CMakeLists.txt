cmake_minimum_required(VERSION 3.10)
project(DisjointSet)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 开启调试信息
set(CMAKE_BUILD_TYPE Debug)

# 添加调试标志（即使没手动指定构建类型）
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -pedantic")

# 包含头文件路径
include_directories(include)

# 添加可执行文件
add_executable(a.out src/main.cc)
