# Smart Pet Feeder based on Raspberry Pi 5B 

## Project Overview
 
 This project is a **real-time embedded system** for a smart pet feeder, built on the Raspberry Pi 5B platform and developed primarily in C++. The system aims to provide **inexpensive, safe, and intelligent feeding solutions for pets**, allowing remote feeding, interaction, and health monitoring via an easy-to-use web interface. The design integrates hardware modules (motors, sensors, camera), real-time control software, and web technologies, showcasing the application of embedded programming in IoT and smart home scenarios.

## Key Features

- **Automated Feeding**: Dispenses pet food and water at scheduled times or via remote control.
- **Remote Monitoring and Control**: Web-based UI allows users to monitor device status and manually trigger feeding from anywhere.
- **Sensing and Feedback**: Integrates weight, IR, and environmental sensors for real-time status and safety monitoring.
- **Video Streaming**: Onboard camera module enables live video feed to check on pets remotely.
- **Real-time Embedded Control**: Deterministic task scheduling for sensor polling, actuator control, and event response.

## System Architecture

![image](https://github.com/user-attachments/assets/a5740e59-f767-4d95-acde-e5efa4959d23)

Raspberry Pi 5B 

├─ Motor Driver (Feeder, Water Pump)

├─ Weight Sensor Module

├─ IR Sensor (Food/Water Detection)

├─ Camera Module (Video Stream)

└─ Web Server (Remote Control UI)

- **Main Control**: C++ application running on Raspberry Pi, handles sensor polling, actuator control, safety checks, and data transmission.
- **Web Frontend**: Simple control panel for users to view status and operate the feeder remotely.
- **Modular Hardware Design**: All key components are modular and easy to extend.

## Hardware Requirements

- Raspberry Pi 5B
- Motor driver (for dry food dispenser & water pump)
- Weight sensor module (HX711)
- IR obstacle sensors (food, water presence detection)
- Pi Camera Module (for live video)
- External power supply for actuators

## Project Structure

├─ src/ # Core C++ source code

├─ include/ # Header files

├─ web/ # Web UI files

├─ libhv-1.3.3/ # Third-party networking library

├─ LICENSE

└─ README.md

## Operating Principle
1. System Power On
   
   Device is powered up (battery or power supply).

   Mainboard (e.g., Raspberry Pi) boots and loads all modules.

2. Initialize Devices
   
   Initialize:

   Servo motor/Feeder; Water pump; IR sensor; Voice module; Camera module (e.g., USB / PiCam); Network module (Wi-Fi/4G + MQTT/HTTP); Real-Time Clock (RTC)
   
3. Connect to Server
   
   Attempt to connect to cloud platform or custom server.

   If connection fails: Enter offline mode with local logging.
   
   If connection succeeds: Sync time and configuration. Enable remote commands
   
4. Enter Main Loop
   
   System continuously monitors environment and schedules.
   
5. IR Sensor Check: Is Pet Nearby?
   
   If No → return to top of loop.
   
   If Yes → proceed with interaction and validation.
   
6. Capture Image via Camera
   Take a snapshot or stream video.
   
   Upload image to server for logging or user view
   
7. Check Feeding Schedule
   
   Compare current time to feeding schedule (local or server-configured).
   
   If time is right → proceed
   
   If not yet → optionally notify server or return to loop
   
8. Voice Prompt + Interaction

   Play voice line (e.g., “Welcome, want some food?”)

   Optionally detect voice input from user/pet
   
9. Feeding and Watering Action

   Activate feeder and water pump

   Use delay-based timing for control

10. Log + Upload Feeding Event

    Record: Timestamp; Pet presence; Snapshot; Feeding status (success/fail)

    Upload to server: Cloud dashboard; Push notification
   
11. Return to Loop

    Short wait or idle state

    Go back to IR detection

## Main Modules

- **main.cpp**: System entry point and global initialization. Initializes all subsystems, manages the main event loop and operational modes.
- **Core/AutoMode.cpp**: Implements autonomous feeding logic based on schedule and sensor data.
- **Core/RemoteMode.cpp**: Handles remote commands and manual operation from the web interface.
- **Core/StateManager.cpp**: Manages device state, transitions, and provides status information to other modules.
- **Communication/WebSocketClient.cpp**: Real-time WebSocket communication for remote control and monitoring.
- **Data/Database.cpp**: Persistent storage for feeding events, logs, and device settings.
- **Data/DataApi.cpp**: Data access abstraction, unifies queries and data operations.
- **Hardware/ServoMotor.cpp**: Controls the feeder's servo motor.
- **Hardware/WaterPump.cpp**: Controls the water dispensing module.
- **Hardware/WeightSensor.cpp**: Reads and processes food/water weight measurements.
- **Hardware/GPIOChip.cpp**: Abstracts GPIO operations (sensors, actuators, indicators).
- **Hardware/CameraModule.cpp**: Handles camera operations and video streaming.

## Highlights & Technical Challenges

- **Real-Time Response**: Achieved via polling and prioritized event handling, ensuring timely actuator and safety actions.
- **Concurrency**: Utilizes multithreading (std::thread, mutex) for parallel processing of sensors, actuators, and web server.
- **Fault Tolerance**: Built-in error detection for sensor failure, food/water blockages, and system recovery.
- **Modularity**: Hardware abstraction allows for easy component replacement or system expansion.

## Demo

- https://www.instagram.com/p/DIn8_9MM1OK/?utm_source=qr

- <img src="https://github.com/user-attachments/assets/56eea94b-a509-4db9-9c5a-6ab82ff19774" alt="Web UI" width="350"/>

## Team & Contribution

- **Hongji Zhang (2961176Z)**:

  Tasked with establishing the overall system framework. Responsible for GPIO hardware abstraction, providing unified and safe control of all sensor and actuator interfaces.

- **Jingjia Zhang (2944713Z)**:

  Responsible for water pump control, camera integration and streaming module, and remote/manual operation mode.
  
- **Runze Zhang (2960782Z)**:

  Designed and implemented the servo motor control logic for food dispensing, and developed the autonomous feeding mode.
  
- **Minghao Jiang (2961294J)**:

  Developed the weight sensing and measurement module for food/water detection, and managed the system state logic and transitions.
  
- **Hongzhan Li (2962160L)**:

  Developed the WebSocket communication module for real-time remote control and data synchronization. Implemented the data API layer and local database management for persistent data storage and query operations.

## License

MIT License

Copyright (c) 2025 Team36-Smart-Pet-Feeder

Permission is hereby granted, free of charge, to any person obtaining a copy  
of this software and associated documentation files (the "Software"), to deal  
in the Software without restriction, including without limitation the rights  
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell  
copies of the Software, and to permit persons to whom the Software is  
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all  
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE  
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER  
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  
SOFTWARE.

---

This project includes open-source components under separate licenses:

- Eclipse Paho MQTT C Client: Eclipse Public License 2.0
  https://github.com/eclipse/paho.mqtt.c

- libhv: MIT License
  https://github.com/ithewei/libhv

- Apache ECharts: Apache License 2.0
  https://github.com/apache/echarts

- rpi-webrtc-streamer: BSD 3-Clause
  https://github.com/kclyu/rpi-webrtc-streamer

- json.hpp (nlohmann/json): MIT License
  https://github.com/nlohmann/json

All rights and licenses for third-party libraries are retained by their original authors.
