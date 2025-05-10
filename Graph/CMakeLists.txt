cmake_minimum_required(VERSION 3.10)
project(Graph)

# 使用 C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 生成调试信息并关闭优化，方便 GDB 调试
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -pedantic")

# 导出编译命令数据库，供 clangd 使用
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 包含头文件目录
include_directories(include)

# 添加可执行文件
add_executable(a.out src/main.cc)
