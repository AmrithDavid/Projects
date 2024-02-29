# Barometer
**Overview**

This is an Arduino-based project designed to monitor and analyze the altitude of rockets in real-time. Utilizing the robust Adafruit BMP280 barometer sensor, this project captures precise atmospheric pressure readings, which are then converted to altitude measurements. These measurements are crucial for determining the rocket's launch state, monitoring its flight path, and confirming its landing. This is developed using C++ and embodies the principles of Object-Oriented Programming (OOP), ensuring a modular, reusable, and maintainable codebase.

**Features**

Real-Time Altitude Monitoring: Leverages the Adafruit BMP280 sensor to provide accurate altitude data by converting atmospheric pressure readings.
Launch Detection: Automatically detects the rocket's launch using altitude changes, transitioning the system's state accordingly.
Flight Path Analysis: Continuously monitors the rocket during flight, providing valuable data on its ascent and descent.
Landing Confirmation: Identifies the rocket's landing through altitude stabilization, marking the end of its journey.
Data Logging: Utilizes SPI Flash memory for efficient data storage and retrieval, enabling post-flight analysis.
OOP Design: Employs Object-Oriented Programming principles for a structured and scalable software architecture.

**Technical Highlights**

C++ and Arduino Framework: Developed in C++ with the Arduino framework, offering a powerful platform for embedded programming.
Object-Oriented Approach: Features classes and objects for managing the barometer sensor, data filtering, and state transitions, promoting code reusability and encapsulation.
Modular Design: The codebase is divided into logical modules, each responsible for a specific aspect of the project, facilitating easy updates and maintenance.
SPI Flash Memory Integration: Incorporates SPI Flash memory for robust data logging capabilities, allowing for extensive data collection and analysis.

**Getting Started**

To get started with RocketBarometer, clone this repository and open the project in the Arduino IDE or a compatible editor. Ensure you have the Adafruit BMP280 and SPIMemory libraries installed. Upload the code to your Arduino board connected to the BMP280 sensor and SPI Flash memory module. Detailed setup instructions and wiring diagrams are provided in the repository.
