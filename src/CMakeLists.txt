# 将本目录下的所有源文件放入SRC_LIST变量中
aux_source_directory(. SRC_LIST)

# 声明利用这些源文件生成动态库mprpc
add_library(mprpc SHARED ${SRC_LIST})