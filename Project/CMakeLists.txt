cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

project(project LANGUAGES CXX C)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)  # 设置这一项可以替代set(CMAKE_CXX_FLAGS "-std=c++20")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_BUILD_TYPE "Debug")
find_package(eclipse-paho-mqtt-c REQUIRED)
find_package(OpenSSL REQUIRED)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)  # 设置可执行文件的输出目录
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)	   # 设置库文件的输出目录

aux_source_directory(${PROJECT_SOURCE_DIR}/src DIR_SRC)

# 设定头文件查找路径，可以将所有头文件路径都添加到这里面
include_directories(${PROJECT_SOURCE_DIR}/include/
                    ${PROJECT_SOURCE_DIR}/3rd/include
                    /usr/local/include)
link_directories(${PROJECT_SOURCE_DIR}/3rd/lib
                    /usr/local/lib/)

add_executable( project ${DIR_SRC} )
target_link_libraries(project wiringPi paho-mqtt3cs -lhx711 -lsqlite3 -llgpio -lpthread -lssl -lcrypto -lm)
