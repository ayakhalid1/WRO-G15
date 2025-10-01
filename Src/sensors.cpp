#include "sensors.h"

void sensorsInit(){
  pinMode(TRIG_FRONT,OUTPUT); pinMode(ECHO_FRONT,INPUT);
  pinMode(TRIG_RIGHT,OUTPUT); pinMode(ECHO_RIGHT,INPUT);
  pinMode(TRIG_LEFT ,OUTPUT); pinMode(ECHO_LEFT ,INPUT);

  pinMode(S0,OUTPUT); pinMode(S1,OUTPUT);
  pinMode(S2,OUTPUT); pinMode(S3,OUTPUT);
  pinMode(OUT_PIN,INPUT);
}

long readUltrasonicCM(uint8_t trigPin, uint8_t echoPin){
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  unsigned long dur = pulseIn(echoPin, HIGH, 30000UL);
  if(dur==0) return -1;
  return (long)(dur/58);
}

unsigned long readTCSFrequency(uint8_t s2, uint8_t s3){
  digitalWrite(S2, s2); digitalWrite(S3, s3);
  delayMicroseconds(200);
  unsigned long total=0;
  for(int i=0;i<COLOR_SAMPLES;i++){
    unsigned long t = pulseIn(OUT_PIN, LOW, 30000UL);
    if(t==0) t=30000UL;
    total += t;
  }
  return total / COLOR_SAMPLES;
}

void readColorRG(unsigned long &fR, unsigned long &fG){
  digitalWrite(S0,HIGH); digitalWrite(S1,LOW);
  fR = readTCSFrequency(LOW, LOW);
  fG = readTCSFrequency(HIGH, HIGH);
}

DetectedColor detectColor(){
  unsigned long fR, fG; readColorRG(fR,fG);
  bool redStronger   = (float)fG > COLOR_MARGIN_RATIO * (float)fR;
  bool greenStronger = (float)fR > COLOR_MARGIN_RATIO * (float)fG;
  if (redStronger)   return COLOR_RED;
  if (greenStronger) return COLOR_GREEN;
  return COLOR_UNKNOWN;
}
