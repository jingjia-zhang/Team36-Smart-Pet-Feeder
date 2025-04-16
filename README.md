# Pet Automatic Feeder based on Raspberry Pi 5B 

\---------------------------------------           
 Our real-time Embedded system code is 88% developed in C++. 
 \--------------------------------------- 
 Our motto is, " Making inexpensive and safe feeder products for customers. " 

 

Document Introduction


- The Social Media Link 

​	1. The link to our instagram 

- The project Introduction: 

​	1. The functions the auto feed machine has 

​	2. The Threads we use and introduction for them 

- Function

- The circuit diagram introduction

- Component Introduction

- Team member introduction & Task allocation 

  

- Mobile App 

​	1. The pics of our mobile application 

​	2. How to use our mobile application 



### 1.**The Social Media Link** 

Please watch our demonstration video here: https://www.instagram.com/zhj_001108?igsh=eDBwM3BmcXl2MnZz 

[ins picture]

### 2.**The Project Introduction** 

With the accelerated pace of modern life, many pet owners are unable to ensure that their pets eat and drink on time due to busy work schedules or short-term trips. To improve the quality of life of pets, some smart feeding devices have appeared on the market, but most of them have a single function and are difficult to meet individual needs. The aim of this project is to develop a Raspberry Pi 5B-based smart pet feeder that integrates automatic feeding, automatic water feeding, pet sensing, camera visualization and voice dialogue, allowing pet owners to remotely manage their pets' diet and health even when they are not at home. 

 

### 3.**Functions of our automatic pet feeder** 

#### Project Objectives 

1.Remote automated feeding: ensure pets eat and drink on time through a timed feeding mechanism or remote control and support intelligent adjustment of feeding schedules. 

2.Enhance pet interaction experience:  Integrated camera and voice interaction functions allow owners to check the status of their pets at any time and communicate remotely by voice to improve the sense of companionship. 

3.Intelligent Sensing &amp; Optimized Feeding:  Uses infrared/ultrasonic sensors to detect if the pet is within the feeding area to prevent food waste and optimize feeding strategies. 

4.Data management and health monitoring: Record pet's eating time, intake and other data to provide owner with more scientific feeding suggestions. 

5.Convenient remote control: Support mobile APP or web access, remote video viewing, manual feeding, data analysis and other functions, so that pet feeding is more intelligent. 

Technical realization 

Embedded platform: adopts Raspberry Pi 5B as the core processing unit, supporting multi-thread processing to improve system response speed. 

Sensor system: Infrared/ultrasonic sensors are used to detect the proximity of pets and combined with weight sensors to monitor the remaining amount of food and water. 

IoT technology: Based on MQTT or WebSocket, real-time communication between devices and the cloud, supporting remote control and data synchronization. 

Camera and Remote Video: Integrated HD camera supports video streaming transmission, allowing owners to check the status of their pets at any time. 

Voice Interaction System: Embedded with AI voice module, it supports voice synthesis and command recognition to achieve remote voice communication. 

This project is dedicated to creating a pet feeder with comprehensive functions and a high degree of intelligence to help pet owners efficiently manage their pets' diets and improve their pets' quality of life. 

 

 

 

### 4.**The Circuit Diagram** 

The Altium Designer project can be found in the folder Circuit Schematic. 

{circuit picture}

This schematic represents a Smart Pet Feeder System based on the Raspberry Pi 5 platform. The design integrates multiple functional modules to enable automated and intelligent management of pet feeding routines. The system adopts a modular architecture, facilitating scalability and maintenance. Its core functionalities are described as follows: 

1. Main Control Unit – Raspberry Pi 5 (U1) 

The Raspberry Pi 5 serves as the central processing unit, responsible for data handling, control logic execution, and coordination among peripheral modules. It communicates with external devices via multiple GPIO pins to perform tasks such as timed feeding, sensor monitoring, and voice output. 

2. Infrared Sensor Module (U2) 

This module detects the presence of a pet near the feeder. It outputs a digital HIGH/LOW signal through the OUT pin to GPIO22 on the Raspberry Pi, providing real-time proximity detection. The output signal can trigger feeding or voice notification procedures. 

3. Water Pump Control Module (U5) 

The LR7843 driver chip controls a miniature water pump. PWM signals from GPIO26 are used to turn the pump on or off, enabling automatic water dispensing. The load and control signals are electrically isolated to enhance system stability and noise immunity. 

4. Servo Control Module (U3) 

This module operates mechanical gates or rotating components to dispense food in precise quantities. The servo receives PWM control signals via the SIG pin connected to GPIO17, enabling accurate actuation of food dispensing mechanisms. 

5. Weighing Module – HX711 (M1) 

This module measures the weight of food in the bowl to determine if replenishment is needed. The HX711 is a 24-bit high-precision ADC that communicates with the Raspberry Pi via DT (GPIO24) and SCK (GPIO25) pins, ensuring accurate and real-time weight data acquisition. 

6. Voice Interaction Module – SU-03T (U13) 

This module supports voice playback functions, providing system status updates, user interaction, or pet-friendly prompts. It communicates via serial signals connected to GPIO12 and supports speaker outputs (SPK+/-), enabling customized audio playback. 

 

The system runs custom control software on an embedded Linux platform, integrating feeding, hydration, detection, and voice functions. It features intelligent sensing, active response, and remote expansion capabilities, making it well-suited for modern smart pet care applications. 



### 5.Component Introduction

### 1. Main Controller

- **Raspberry Pi 5B** with camera module

  

### 2. Sensors & Modules

- **2.1** Infrared sensor ×1

  

- **2.2** Load cell with **HX711** module

  

- **2.3** Water pump ×1

  

- **2.4** Servo motor **SG90** ×1

  

- **2.5** Voice module **SU-03T** ×1

  

- **2.6** Several Dupont wires

  