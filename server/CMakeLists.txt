cmake_minimum_required(VERSION 3.14)
project(Pet_Server)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 添加 SQLiteCpp
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/SQLiteCpp)

# 添加可执行文件
add_executable(server
        ${CMAKE_SOURCE_DIR}/src/main.cpp
        ${CMAKE_SOURCE_DIR}/src/Server.cpp
        ${CMAKE_SOURCE_DIR}/src/DeviceMessageHandler.cpp
        ${CMAKE_SOURCE_DIR}/src/FrontendCommandHandler.cpp
)

# 公共头文件路径
target_include_directories(server PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/lib/websocketpp
        ${CMAKE_SOURCE_DIR}/lib/nlohmann_json/include
        ${CMAKE_SOURCE_DIR}/lib/SQLiteCpp/include
)

if(WIN32)
    # Windows 下手动指定 Boost 路径
    set(BOOST_ROOT "D:/local/boost_1_69_0")
    set(BOOST_INCLUDE_DIR "${BOOST_ROOT}")
    set(BOOST_LIB_DIR "${BOOST_ROOT}/stage/lib")

    target_include_directories(server PRIVATE ${BOOST_INCLUDE_DIR})

    target_link_libraries(server PRIVATE
            SQLiteCpp
            pthread
            "${BOOST_LIB_DIR}/libboost_system-vc140-mt-x64-1_69.lib"
            "${BOOST_LIB_DIR}/libboost_thread-vc140-mt-x64-1_69.lib"
            ws2_32
            Mswsock
    )
else()
    # Linux/树莓派 用 find_package 查找 Boost
    find_package(Boost 1.69 REQUIRED COMPONENTS system thread)

    if(Boost_FOUND)
        message(STATUS "Found Boost: ${Boost_VERSION}")
        target_include_directories(server PRIVATE ${Boost_INCLUDE_DIRS})
        target_link_libraries(server PRIVATE ${Boost_LIBRARIES} pthread)
    else()
        message(FATAL_ERROR "Boost not found!")
    endif()

    # 额外链接库（Linux 通常不需要 ws2_32 和 Mswsock）
endif()

install(TARGETS server DESTINATION bin)