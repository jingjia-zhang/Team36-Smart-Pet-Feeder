cmake_minimum_required(VERSION 3.12)
project(PetFeederSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output construction information
message(STATUS "Building ${PROJECT_NAME} with C++17")

find_package(gpiod REQUIRED)
find_package(Threads REQUIRED)
find_package(Websocketpp REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(nlohmann_json 3.4.0 REQUIRED)
find_package(OpenCV REQUIRED COMPONENTS core imgproc highgui)
if(OpenCV_FOUND)
    message(STATUS "OpenCV include dirs: ${OpenCV_INCLUDE_DIRS}")
    message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")
    include_directories(${OpenCV_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "OpenCV not found. Please install OpenCV first.")
endif()

# Output SQLite related information for easy debugging
message(STATUS "SQLite3 include dirs: ${SQLite3_INCLUDE_DIRS}")
message(STATUS "SQLite3 libraries: ${SQLite3_LIBRARIES}")

include_directories(
        ${CMAKE_SOURCE_DIR}/include
        ${SQLite3_INCLUDE_DIRS}
)


set(SOURCES
        src/main.cpp
        src/Hardware/GPIOChip.cpp
        src/Hardware/ServoMotor.cpp
        src/Hardware/WaterPump.cpp
        src/Hardware/WeightSensor.cpp
        src/Hardware/CameraModule.cpp
        src/Communication/WebSocketClient.cpp
        src/Core/AutoMode.cpp
        src/Core/RemoteMode.cpp
        src/Core/StateManager.cpp
        src/Data/Database.cpp
        src/Data/DataAPI.cpp
)


add_executable(pet_feeder ${SOURCES})


target_link_libraries(pet_feeder
        PRIVATE
        gpiod::gpiod
        Websocketpp::websocketpp
        SQLite::SQLite3
        nlohmann_json::nlohmann_json
        ${CMAKE_THREAD_LIBS_INIT}
        ${OpenCV_LIBS}
        v4l2
)


install(TARGETS pet_feeder DESTINATION bin)
