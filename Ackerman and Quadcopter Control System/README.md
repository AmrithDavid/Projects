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

## Execution Flow

- **Non-Blocking Execution**: The `run()` function is non-blocking and initiates goal navigation instantly.
- **Simultaneous Operation**: Platforms operate on separate threads, navigating goals simultaneously for optimal efficiency.
- **Distance Computation**: At mission start, each platform calculates the total distance to all goals, aiding in progress tracking.
- **Status Monitoring**: Platforms report `IDLE` when stationary and `RUNNING` during goal navigation.

## Mission Status

- **Progress Tracking**: Mission status, reflecting the percentage of completion, is available on demand. It's based on the distance traveled relative to the total planned.
- **Dynamic Recalibration**: Adjust the distance estimates as goals are approached to refine progress accuracy, ensuring the mission completion percentage reaches 100% only when all goals are achieved.

## Technical Environment

- **ROS 2 (Robot Operating System)**: This project utilizes ROS for managing communication between the different components of the system. 
- **Gazebo Simulator**: All simulation tasks are performed within the Gazebo environment, a robust simulator for robotic systems that provides realistic scenarios and physics calculations. This allows for accurate testing of the navigation algorithms and platform controls before actual deployment.
- **Linux**: Given the robust support for ROS and Gazebo, this project is designed to run on Linux operating systems, which provides the necessary tools and performance efficiencies required for real-time simulation and control tasks.
  
