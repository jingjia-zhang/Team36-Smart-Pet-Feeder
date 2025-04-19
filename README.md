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
#### 6. Mobile Application
- The Pictures of Our Mobile Application
- How to Use Our Mobile Application
#### 7. Repository Structure
#### 8. Team Member Introduction & Task Allocation
#### 9. License


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

5.Convenient remote control: Support mobile APP or web access, remote video viewing, manual feeding, data analysis and other functions, so that pet feeding is more intelligent. 

- #### Technical Realization

Embedded platform: adopts Raspberry Pi 5B as the core processing unit, supporting multi-thread processing to improve system response speed. 

Sensor system: Infrared/ultrasonic sensors are used to detect the proximity of pets and combined with weight sensors to monitor the remaining amount of food and water. 

IoT technology: Based on MQTT or WebSocket, real-time communication between devices and the cloud, supporting remote control and data synchronization. 

Camera and Remote Video: Integrated HD camera supports video streaming transmission, allowing owners to check the status of their pets at any time. 

Voice Interaction System: Embedded with AI voice module, it supports voice synthesis and command recognition to achieve remote voice communication. 

This project is dedicated to creating a pet feeder with comprehensive functions and a high degree of intelligence to help pet owners efficiently manage their pets' diets and improve their pets' quality of life. 

 

 

 

### 4. **Design and Implementation of Our Smart Pet Feeder Using Raspberry Pi** 

The Altium Designer project can be found in the folder Circuit Schematic. 

{circuit picture}

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

- **Raspberry Pi 5B** with camera module

  

#### 2. Sensors & Modules

- Infrared sensor ×1

  

- Load cell with **HX711** module

  

- Water pump ×1

  

- Servo motor **SG90** ×1

  

- Voice module **SU-03T** ×1

  

- Several Dupont wires

  

### 6. Mobile Application

- **The Pictures of Our Mobile Application**

- **How to Use Our Mobile Application**

### 7. Repository Structure

SmartPetFeeder/
├── README.md                  # Project description and usage instructions
├── LICENSE                          # Open source license file
├── web/                               # Web frontend for remote control and visualization
│   ├── index.html                # Main UI page
│   ├── mqtt.js                      # MQTT over WebSocket communication
│   ├── echarts.min.js         # (3rd-party) Chart rendering library
│   └── chart.js                     # Weight and status display
├── Project/                         # External libraries, build scripts, and binaries
│   ├── 3rd/                          # Precompiled libraries (e.g., hv, mqtt)
│   ├── bin/                          # Output binaries and calibration logs
│   └── build/                       # CMake build files (ignored by Git)
├── RaspberryPi-WebRTC-main/  # Video and audio streaming module using WebRTC
│   ├── src/                          # WebRTC backend logic
│   └── doc/                        # Setup and architecture documentation
├── test/                              # Unit test files and validation scripts
├── images/                       # (Optional) Project photos or diagrams
├── video/                          # (Optional) Demo or test videos
└── docs/                           # (Optional) Technical or team documentation

### 8. Team Member Introduction & Task Allocation

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

### 9. License
