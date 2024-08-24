#最小版本要求
cmake_minimum_required(VERSION 3.22.0)

#工程名称
project(chatClient)

#生成可执行文件的源文件
aux_source_directory(${PROJECT_SOURCE_DIR} SRC_LIST)

#生成可执行文件
add_executable(chatClient ${SRC_LIST} ${PROJECT_SOURCE_DIR}/../server/model/User.cpp ${PROJECT_SOURCE_DIR}/../server/model/Group.cpp)

target_link_libraries(chatClient pthread)



