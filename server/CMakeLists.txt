cmake_minimum_required(VERSION 3.29)
project(Pet_Server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS thread system)

# Adding SQLiteCpp Library
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/SQLiteCpp)

# Adding header file directories
include_directories(
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/lib/websocketpp
        ${CMAKE_SOURCE_DIR}/lib/nlohmann_json/include
        ${CMAKE_SOURCE_DIR}/lib/SQLiteCpp/include
        ${Boost_INCLUDE_DIRS}
)

# Adding Executable Files
add_executable(server
        ${CMAKE_SOURCE_DIR}/src/main.cpp
        ${CMAKE_SOURCE_DIR}/src/Server.cpp
)

# Linking dependent libraries
target_link_libraries(server
        PRIVATE
        SQLiteCpp
        pthread
        ${Boost_LIBRARIES}
)

install(TARGETS server DESTINATION bin)