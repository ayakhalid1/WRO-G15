#ifndef SENSORS_H
#define SENSORS_H
#include <Arduino.h>
#include "config.h"

void sensorsInit();

long readUltrasonicCM(uint8_t trigPin, uint8_t echoPin);

unsigned long readTCSFrequency(uint8_t s2, uint8_t s3);
void readColorRG(unsigned long &fR, unsigned long &fG);
DetectedColor detectColor();

#endif
