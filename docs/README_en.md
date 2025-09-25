# WRO-G15 Project (English)

This documentation is aimed at judges and mentors to evaluate our project.

## Objective
- Build an autonomous robot for the WRO 2025 Future Engineers category.
- The robot must navigate, avoid obstacles, and count laps using sensors.

## Hardware
- ESP32 DevKit V1
- L298N Motor Driver + 2 DC Motors
- SG90 Servo for steering
- HC-SR04 Ultrasonic sensor
- TCS3200 Color sensor
- 18650 batteries + holder

## Software
- C++ with Arduino IDE
- Modular code structure:
  - `motors.cpp/h` → Motor & servo functions
  - `sensors.cpp/h` → Ultrasonic + color detection
  - `config.h` → Pins and constants
  - `WRO-G15.ino` → Main program

## Key Features
- Obstacle avoidance
- Color-based lap counting
- Simple finite state machine (FSM) for tracking corners/laps
