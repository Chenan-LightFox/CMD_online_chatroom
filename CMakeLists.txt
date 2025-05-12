
#对CMake版本进行限制
cmake_minimum_required(VERSION 3.15)
 
#设置解决方案名称
project(CMD_ONLINE_CHATROOM)
 
#添加子目录 - 即MyClient和MyServer两个项目录
# - 参数1 是用来指定需要索引的源文件目录 
# - 参数2 用来指定项目的名称
 
add_subdirectory(CMDoc_server CMDoc_server)
add_subdirectory(CMDoc_client CMDoc_client)