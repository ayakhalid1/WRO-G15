#ifndef MOTORS_H
#define MOTORS_H
#include <Arduino.h>
#include "config.h"

void motorsInit();
void setLR(int l, int r);      // signed PWM: +forward / -backward
void coast();
void brake();
void spinLeft(uint16_t ms, int sp);
void spinRight(uint16_t ms, int sp);

// Servo steering (نفس ما في السكetch)
void steeringInit();
void steerCenter();
void steerRightSoft();
void steerRightHard();
void steerLeftSoft();
void steerLeftHard();

// للمشي للأمام بسرعتين مستقلتين (إن احتجته)
void motorsForward(uint8_t speedA, uint8_t speedB);

#endif
