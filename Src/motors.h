#ifndef MOTORS_H
#define MOTORS_H

void motorsInit();                 // تهيئة المحركات + السيرفو
void motorsForward();              //  للامام
void motorsStop();                 // إيقاف
void motorsSteerCenter();          //  السيرفو بالوسط
void motorsAvoidLeftBurst();       // انعطاف يسار
void motorsAvoidRightBurst();      // انعطاف يمين

#endif
