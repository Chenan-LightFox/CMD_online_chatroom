cmake_minimum_required(VERSION 3.15)
#生成的子项目名称
project (CMDoc_server) 
 
#搜索对应的头文件 并存储在 HEADER_FILES变量中
file(GLOB HEADER_FILES ./header_files/*.h)
#搜索对应的CPP文件 并存储在 SRC_FILES_FILES变量中
file(GLOB_RECURSE SRC_FILES ./source_file/*.cpp)
 
#将之前找到的文件导入到项目中，指定生成的文件类型 (.exe)
add_executable (${PROJECT_NAME} ${HEADER_FILES} ${SRC_FILES})