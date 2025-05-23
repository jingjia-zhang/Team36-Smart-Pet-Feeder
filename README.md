# Smart Pet Feeder based on Raspberry Pi 5B 

\---------------------------------------------------------------------------------------------------------------------------------
 
 Our real-time Embedded system code is 88% developed in C++. 
 
 Our motto is, " Making Inexpensive , Safe and Smart Pet Feeder Products for Customers. " 

## Document Introduction
####  1. The Link to our Instagram
- The Social Media Link
#### 2. The Project Introduction
#### 3. Functions of Our Automatic Pet Feeder
- Project Objectives
- Technical Realization
#### 4. The Circuit Diagram Introduction and Description of Core Functions of Each Module
#### 5. Component Introduction 
#### 6. Web Page for Remote Control and Status Display
- The Picture of Our Web Page
#### 7. Repository Structure
#### 8. Operating Principle
#### 9. Team Member Introduction & Task Allocation
#### 10. License


\---------------------------------------------------------------------------------------------------------------------------------

### 1. **The Social Media Link** 

Please watch our demonstration video here: https://www.instagram.com/p/DIn8_9MM1OK/?utm_source=qr

![b8654549c2ea8379312fa44fedd8a92](https://github.com/user-attachments/assets/f7985afb-f975-4c9c-8b28-0a059acd0d07)

### 2. **The Project Introduction** 

With the accelerated pace of modern life, many pet owners are unable to ensure that their pets eat and drink on time due to busy work schedules or short-term trips. To improve the quality of life of pets, some smart feeding devices have appeared on the market, but most of them have a single function and are difficult to meet individual needs. The aim of this project is to develop a Raspberry Pi 5B-based smart pet feeder that integrates automatic feeding, automatic water feeding, pet sensing, camera visualization and voice dialogue, allowing pet owners to remotely manage their pets' diet and health even when they are not at home. 

 

### 3. **Functions of Our Smart pet feeder** 

- #### Project Objectives 

1.Remote automated feeding: ensure pets eat and drink on time through a timed feeding mechanism or remote control and support intelligent adjustment of feeding schedules. 

2.Enhance pet interaction experience:  Integrated camera and voice interaction functions allow owners to check the status of their pets at any time and communicate remotely by voice to improve the sense of companionship. 

3.Intelligent Sensing &amp; Optimized Feeding:  Uses infrared/ultrasonic sensors to detect if the pet is within the feeding area to prevent food waste and optimize feeding strategies. 

4.Data management and health monitoring: Record pet's eating time, intake and other data to provide owner with more scientific feeding suggestions. 

5.Convenient remote control: Support web access, remote video viewing, manual feeding, data analysis and other functions, so that pet feeding is more intelligent. 

- #### Technical Realization

Embedded platform: adopts Raspberry Pi 5B as the core processing unit, supporting multi-thread processing to improve system response speed. 

Sensor system: Infrared/ultrasonic sensors are used to detect the proximity of pets and combined with weight sensors to monitor the remaining amount of food and water. 

IoT technology: Based on MQTT or WebSocket, real-time communication between devices and the cloud, supporting remote control and data synchronization. 

Camera and Remote Video: Integrated HD camera supports video streaming transmission, allowing owners to check the status of their pets at any time. 

Voice Interaction System: Embedded with AI voice module, it supports voice synthesis and command recognition to achieve remote voice communication. 

This project is dedicated to creating a pet feeder with comprehensive functions and a high degree of intelligence to help pet owners efficiently manage their pets' diets and improve their pets' quality of life. 

 

 

 

### 4. **Design and Implementation of Our Smart Pet Feeder Using Raspberry Pi** 

The Altium Designer project can be found in the folder Circuit Schematic. 

![image](https://github.com/user-attachments/assets/a5740e59-f767-4d95-acde-e5efa4959d23)


This schematic represents a Smart Pet Feeder System based on the Raspberry Pi 5 platform. The design integrates multiple functional modules to enable automated and intelligent management of pet feeding routines. The system adopts a modular architecture, facilitating scalability and maintenance. Its core functionalities are described as follows: 

#### 1. Main Control Unit – Raspberry Pi 5 (U1) 

The Raspberry Pi 5 serves as the central processing unit, responsible for data handling, control logic execution, and coordination among peripheral modules. It communicates with external devices via multiple GPIO pins to perform tasks such as timed feeding, sensor monitoring, and voice output. 

#### 2. Infrared Sensor Module (U2) 

This module detects the presence of a pet near the feeder. It outputs a digital HIGH/LOW signal through the OUT pin to GPIO22 on the Raspberry Pi, providing real-time proximity detection. The output signal can trigger feeding or voice notification procedures. 

#### 3. Water Pump Control Module (U5) 

The LR7843 driver chip controls a miniature water pump. PWM signals from GPIO26 are used to turn the pump on or off, enabling automatic water dispensing. The load and control signals are electrically isolated to enhance system stability and noise immunity. 

#### 4. Servo Control Module (U3) 

This module operates mechanical gates or rotating components to dispense food in precise quantities. The servo receives PWM control signals via the SIG pin connected to GPIO17, enabling accurate actuation of food dispensing mechanisms. 

#### 5. Weighing Module – HX711 (M1) 

This module measures the weight of food in the bowl to determine if replenishment is needed. The HX711 is a 24-bit high-precision ADC that communicates with the Raspberry Pi via DT (GPIO24) and SCK (GPIO25) pins, ensuring accurate and real-time weight data acquisition. 

#### 6. Voice Interaction Module – SU-03T (U13) 

This module supports voice playback functions, providing system status updates, user interaction, or pet-friendly prompts. It communicates via serial signals connected to GPIO12 and supports speaker outputs (SPK+/-), enabling customized audio playback. 

 

The system runs custom control software on an embedded Linux platform, integrating feeding, hydration, detection, and voice functions. It features intelligent sensing, active response, and remote expansion capabilities, making it well-suited for modern smart pet care applications. 



### 5. Component Introduction

#### 1. Main Controller

- **Raspberry Pi 5B** 

  ![image](https://github.com/user-attachments/assets/8391e5c1-71b5-4f02-b63f-dea7aff676ee)

#### 2. Sensors & Modules

- **Infrared sensor ×1**

  ![f30df0e18c183e1698f7bc9c0960bd2](https://github.com/user-attachments/assets/e5f10256-509d-42df-a6e6-e7677dd97fb1)

- **Load cell with HX711 module ×1**

  ![7b18a948369ae4907e2354afa1dc9f1](https://github.com/user-attachments/assets/0a36d36b-6a27-4890-a000-0958b54f400e)

- **Camera module ×1**

  ![5d37d4e8bc5ede85e4603ac3d32e43f](https://github.com/user-attachments/assets/57b6393a-3fc3-4ea3-a63a-53e16c30bce2)

- **Water pump ×1**

  ![cb95196e7711e2ee8bec04b675bc5a1](https://github.com/user-attachments/assets/9e378500-ca54-4691-b148-17a60a9af517)


- **Servo motor SG90 ×1**

  ![151d789f5a284a76aeb17477e7be648](https://github.com/user-attachments/assets/64e7ebc7-deb1-4cfb-b001-c8f5e07263b2)

- **Voice module SU-03T ×1**

  ![2c72e6d1f41c826a3636111453e05af](https://github.com/user-attachments/assets/822d5844-1415-48a4-92da-38263ff61717)

- **Several Dupont wires**

  ![9be274519104183d3cd9c4a5afa6786](https://github.com/user-attachments/assets/14596334-7731-4725-a1cd-f91db53cd1ba)

### 6. Web Page for Remote Control and Status Display

- **The Picture of Our Web Page**

  ![df3575f7e980e73728eeedf582265e5](https://github.com/user-attachments/assets/f499831f-282b-45f2-bfeb-8169adc42f07)

### 7. Repository Structure

![image](https://github.com/user-attachments/assets/672bff98-1d38-463d-907b-e89a8d9f7ed4)

### 8. Operating Principle

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

### 9. Team Member Introduction & Task Allocation

- Jingjia Zhang (2944713Z)

  Project Development. Responsible for Raspberry Pi control code with sensors, motors, pumps, voice modules and other hardware.
  
- Runze Zhang (2960782Z)

  Project Development. Responsible for Raspberry Pi control code with sensors, motors, pumps, voice modules and other hardware.
  
- Hongji Zhang (2961176Z)

  Project Development. Responsible for Raspberry Pi control code with sensors, motors, pumps, voice modules and other hardware.
  
- Minghao Jiang (2961294J)

  Hardware Engineering (PCB Design). Design PCB schematics and board layouts to integrate Raspberry Pi interfaces, sensor wiring, power management, and module packaging.
  
- Hongzhan Li (2962160L)

  Project Maintenance and Problem Handling. Responsible for testing and debugging, locating and fixing bugs, and coordinating with other team members to solve problems.

### 10. License

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
