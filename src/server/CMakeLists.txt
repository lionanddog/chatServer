#最小版本要求
cmake_minimum_required(VERSION 3.22.0)

#工程名称
project(chatServer)

#生成可执行文件的源文件
aux_source_directory(${PROJECT_SOURCE_DIR} SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/db DB_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/model MODEL_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/redis REDIS_LIST)

#生成可执行文件
add_executable(chatServer ${SRC_LIST} ${DB_LIST} ${MODEL_LIST} ${REDIS_LIST})

target_link_libraries(chatServer muduo_net muduo_base mysqlclient hiredis pthread)



