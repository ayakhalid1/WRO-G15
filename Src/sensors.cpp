#include <Arduino.h>
#include <math.h>
#include "config.h"
#include "sensors.h"

void sensorsInit() {
  // Ultrasonic
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // TCS3200
  pinMode(S0, OUTPUT); pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT); pinMode(S3, OUTPUT);
  pinMode(OUT_PIN, INPUT);

  // 20% scaling
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
}

long readUltrasonicCM() {
  digitalWrite(TRIG_PIN, LOW); delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return -1;
  long distance = (long)(duration * 0.034f / 2.0f);
  return distance;
}

int readColor(bool s2State, bool s3State) {
  digitalWrite(S2, s2State);
  digitalWrite(S3, s3State);
  delay(5);
  int duration = pulseIn(OUT_PIN, HIGH, 50000); // 50ms
  if (duration == 0) duration = 50000;
  int frequency = 1000000 / duration;
  return frequency;
}

int readChannelMedian(bool s2, bool s3){
  int a = readColor(s2,s3);
  int b = readColor(s2,s3);
  int c = readColor(s2,s3);
  if (a>b){int t=a;a=b;b=t;}
  if (b>c){int t=b;b=c;c=t;}
  if (a>b){int t=a;a=b;b=t;}
  return b; // الوسيط
}

Col readMatColorNorm(){
  int R = max(readChannelMedian(LOW,LOW),  1); // R
  int G = max(readChannelMedian(LOW,HIGH), 1); // G
  int B = max(readChannelMedian(HIGH,HIGH),1); // B
  float S = (float)R + G + B;
  float Rn = R/S, Gn = G/S, Bn = B/S;

  bool isBlue   = (Bn > 0.50f) && (Rn < 0.30f) && (Gn < 0.40f);
  bool isOrange = (Rn > 0.50f) && (Bn < 0.25f) && (Gn > 0.20f);
  bool nearWhite= (fabs(Rn-Gn) < 0.10f) && (fabs(Gn-Bn) < 0.10f);

  if (isBlue)    return C_BLUE;
  if (isOrange)  return C_ORANGE;
  if (nearWhite) return C_WHITE;
  return C_OTHER;
}
