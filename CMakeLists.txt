#最小版本要求
cmake_minimum_required(VERSION 3.22.0)

#工程名称
project(chatProject)

#设置输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#设置可调试
set(CMAKE_CXX_FLAGS -g)

#包含头文件
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/server)
include_directories(${PROJECT_SOURCE_DIR}/include/server/model)
include_directories(${PROJECT_SOURCE_DIR}/include/client)
include_directories(${PROJECT_SOURCE_DIR}/thirdpart)
include_directories(${PROJECT_SOURCE_DIR}/include/server/db)
include_directories(${PROJECT_SOURCE_DIR}/include/server/redis)

#添加子文件夹
add_subdirectory(${PROJECT_SOURCE_DIR}/src/server)
add_subdirectory(${PROJECT_SOURCE_DIR}/src/client)


