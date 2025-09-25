#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

/*** Motor Pins (L298N) ***/
#define IN1 14
#define IN2 12
#define IN3 27
#define IN4 25
#define ENA 26    // enable left motor
#define ENB 17    // enable right motor

/*** Servo (steering) ***/
#define SERVO_PIN 13

/*** Ultrasonic ***/
#define TRIG_PIN 33
#define ECHO_PIN 32

/*** TCS3200 Color Sensor (floor) ***/
#define S0  18
#define S1  19
#define S2  23
#define S3  5
#define OUT_PIN 4

/*** Start Button ***/
#define START_BTN 0   // زر BOOT على ESP32

/*** Parameters ***/
#define OBSTACLE_DISTANCE 20   // cm
#define TURN_DELAY 600         // ms for avoidance burst
#define SERVO_LEFT   45
#define SERVO_CENTER 90
#define SERVO_RIGHT  135

// لوجيك الزوايا/اللفات
enum Col { C_WHITE, C_BLUE, C_ORANGE, C_OTHER };

#endif
