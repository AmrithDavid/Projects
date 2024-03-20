# Rocket Telemetry and Environmental Sensing Project


This repository contains the source code for an Arduino-based project aimed at capturing and analyzing telemetry and environmental data for rocket launches. The project utilizes various hardware components and programming concepts to achieve a comprehensive monitoring solution.


**Key Features**


*Barometric Altitude Measurement*: Utilizes the Adafruit BMP280 sensor to measure barometric pressure, from which altitude data is derived. This is crucial for determining the rocket's altitude during its flight.

**SPI Flash Memory Integration**: Incorporates the SPIMemory library for SPI flash memory interaction, enabling the storage of telemetry data that can be analyzed post-flight.

**Sound Tones for Event Notification**: Employs a tones library to generate sound tones, which are used to indicate different stages of the rocket's journey, such as launch readiness, launch detection, and landing.

**Telemetry Data Filtering**: Implements a custom filtering class to calculate rolling averages of altitude measurements, ensuring more stable and accurate altitude data by mitigating sensor noise.


**Programming Concepts**


**Object-Oriented Programming (OOP)**: The code extensively uses classes and objects (e.g., the Filter class for altitude filtering and instances of sensor objects) to encapsulate behaviors and manage state.

**Memory Management**: Demonstrates dynamic memory allocation with pointers for managing hardware component interfaces, such as flash memory (SPIFlash* flash) and the barometric sensor (Adafruit_BMP280* barometer).

**Serial Communication**: Utilizes Arduino's serial communication capabilities to output telemetry data and debugging information, enhancing the visibility of the rocket's state and sensor readings.

**Control Structures**: Employs control structures such as loops, conditional statements, and switch cases (e.g., for handling different launch states) to control the flow of the program based on sensor inputs and system state.

**Data Structures**: Makes use of arrays for data storage (e.g., rolling average calculations), structures for organizing related data (struct RocketState), and enums for defining state constants (enum LaunchState).


**Hardware Requirements**


- Arduino compatible microcontroller board
- Adafruit BMP280 barometric sensor
- SPI flash memory module
- Optional: Buzzer or speaker for sound notification
