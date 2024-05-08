# Projects

# University Engineering Projects Repository

This repository is dedicated to showcasing key engineering projects. These projects encompass diverse aspects of engineering from telemetry systems in rocketry to sophisticated control systems and embedded hardware applications.

## Table of Contents
- [Rocket Telemetry and Environmental Sensing Project](#rocket-telemetry-and-environmental-sensing-project)
- [Quadcopter Control System](#quadcopter-control-system)
- [Step Tracker System](#step-tracker-system)

## Rocket Telemetry and Environmental Sensing Project
This Arduino-based project captures and analyzes telemetry and environmental data during rocket launches, using various sensors and data handling techniques.

### Key Features
- **Barometric Altitude Measurement**: Utilizes the Adafruit BMP280 sensor for accurate altitude tracking.
- **SPI Flash Memory Integration**: Uses the SPIMemory library for data storage to facilitate post-flight analysis.
- **Sound Tones for Event Notification**: Implements sound notifications for critical launch phases using a tones library.
- **Telemetry Data Filtering**: Features a custom filter class to refine altitude data and reduce sensor noise.

### Programming Concepts
- **Object-Oriented Programming**: Structured around classes and objects to manage system behaviors.
- **Memory Management**: Demonstrates effective use of pointers for handling complex hardware interactions.
- **Serial Communication**: Employs serial outputs for real-time telemetry and diagnostic outputs.
- **Control Structures and Data Structures**: Utilizes advanced programming constructs to manage data flow and sensor integration.

### Hardware Requirements
- Arduino compatible microcontroller board
- Adafruit BMP280 barometric sensor
- SPI flash memory module
- Optional: Buzzer or speaker for sound notifications
