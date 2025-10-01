#include "motors.h"
#include <ESP32Servo.h>

static Servo steeringServo;

void motorsInit(){
  pinMode(IN1,OUTPUT); pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT); pinMode(IN4,OUTPUT);
  ledcSetup(CH_A, PWM_FREQ, PWM_RES);
  ledcSetup(CH_B, PWM_FREQ, PWM_RES);
  ledcAttachPin(ENA, CH_A);
  ledcAttachPin(ENB, CH_B);
  coast();
}

void setLR(int l, int r){
  if(l>=0){ digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW); }
  else    { digitalWrite(IN1,LOW ); digitalWrite(IN2,HIGH); l=-l; }
  if(r>=0){ digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW); }
  else    { digitalWrite(IN3,LOW ); digitalWrite(IN4,HIGH); r=-r; }
  if(l>255) l=255; if(r>255) r=255;
  ledcWrite(CH_A, l);
  ledcWrite(CH_B, r);
}

void coast(){ digitalWrite(IN1,LOW); digitalWrite(IN2,LOW); digitalWrite(IN3,LOW); digitalWrite(IN4,LOW); ledcWrite(CH_A,0); ledcWrite(CH_B,0); }
void brake(){ digitalWrite(IN1,HIGH);digitalWrite(IN2,HIGH);digitalWrite(IN3,HIGH);digitalWrite(IN4,HIGH); ledcWrite(CH_A,0); ledcWrite(CH_B,0); }

void spinLeft (uint16_t ms,int sp){ setLR(-sp, sp); delay(ms); }
void spinRight(uint16_t ms,int sp){ setLR( sp,-sp); delay(ms); }

void motorsForward(uint8_t a, uint8_t b){
  digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW);
  digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW);
  ledcWrite(CH_A, a);
  ledcWrite(CH_B, b);
}

// ----- Servo -----
void steeringInit(){
  steeringServo.attach(SERVO_PIN, SERVO_MIN_US, SERVO_MAX_US);
  steeringServo.write(SERVO_CENTER);
}
void steerCenter()    { steeringServo.write(SERVO_CENTER); }
void steerRightSoft() { steeringServo.write((SERVO_CENTER + SERVO_RIGHT)/2); }
void steerRightHard() { steeringServo.write(SERVO_RIGHT); }
void steerLeftSoft()  { steeringServo.write((SERVO_CENTER + SERVO_LEFT)/2); }
void steerLeftHard()  { steeringServo.write(SERVO_LEFT); }
