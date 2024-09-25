cmake_minimum_required(VERSION 3.10)
project(mprpc)

# 项目可执行文件输出地址
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
# 项目库文件输出地址
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# 设置项目编译头文件搜索路径
include_directories(${PROJECT_SOURCE_DIR}/src/include)
include_directories(${PROJECT_SOURCE_DIR}/example)

# 设置项目库文件搜索路径
link_directories(${PROJECT_SOURCE_DIR}/lib)


# 框架源代码
add_subdirectory(${PROJECT_SOURCE_DIR}/src)
# 框架实例代码
add_subdirectory(${PROJECT_SOURCE_DIR}/example)