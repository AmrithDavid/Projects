# Mechatronics Platform Controller

## Overview

This folder contains a complete C++ program that controls Mechatronic platforms using object-oriented paradigms, multiple threads, data synchronization, and time synchronization. The program effectively coordinates the actions of different platforms to achieve specified goals within a simulated environment.

## Project Features

- **Multithreading**: Efficiently manage multiple operations in parallel to ensure real-time performance and responsiveness.
- **Data Synchronization**: Ensure data integrity between threads with synchronized operations, crucial for consistent platform behavior.
- **Time Synchronization and Interpolation**: Utilize time-stamped data for accurate prediction and control over platform movements.
- **Optimal Path Finding**: Implemented using graph data structures and search algorithms to determine efficient paths through goals.


## Platforms Controlled

- **Quadcopter (Drone)**: Manages aerial navigation through specified waypoints.
- **Audi R8 (Vehicle with Ackerman Steering)**: Handles ground navigation with precision steering control.

### Technical Environment

- **ROS 2 (Robot Operating System)**: This project utilizes ROS for managing communication between the different components of the system. 
- **Gazebo Simulator**: All simulation tasks are performed within the Gazebo environment, a robust simulator for robotic systems that provides realistic scenarios and physics calculations. This allows for accurate testing of the navigation algorithms and platform controls before actual deployment.
- **Linux**: Given the robust support for ROS and Gazebo, this project is designed to run on Linux operating systems, which provides the necessary tools and performance efficiencies required for real-time simulation and control tasks.
  
