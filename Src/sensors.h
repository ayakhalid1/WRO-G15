#ifndef SENSORS_H
#define SENSORS_H

#include "config.h"

void sensorsInit();           // تهيئة الآلتراسونك + TCS3200
long readUltrasonicCM();      // المسافة بالسم
int  readColor(bool s2, bool s3);
int  readChannelMedian(bool s2, bool s3);
Col  readMatColorNorm();      // White / Blue / Orange / Other

#endif
