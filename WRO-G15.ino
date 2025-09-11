#include <ESP32Servo.h>  // Servo on ESP32

// ===== Motor Pins (L298N) =====
#define IN1 14
#define IN2 12
#define IN3 27
#define IN4 25
#define ENA 26  // enable left motor
#define ENB 17  // enable right motor

// ===== Servo (steering) =====
#define SERVO_PIN 13
Servo steeringServo;

// ===== Ultrasonic =====
#define TRIG_PIN 33
#define ECHO_PIN 32

// ===== TCS3200 Color Sensor (floor) =====
#define S0 18
#define S1 19
#define S2 23
#define S3 5
#define OUT_PIN 4


// ===== Start Button =====
#define START_BTN 0   // زر BOOT على ESP32

// ===== Parameters =====
#define OBSTACLE_DISTANCE 20   // cm
#define TURN_DELAY 600         // ms for avoidance burst
#define SERVO_LEFT 45
#define SERVO_CENTER 90
#define SERVO_RIGHT 135

bool started = false;
bool lapsDone = false;

// ===== Colors / Corners / Laps =====
enum Col { C_WHITE, C_BLUE, C_ORANGE, C_OTHER };
enum PairFSM { WAIT_FIRST, WAIT_SECOND };
PairFSM pfsm = WAIT_FIRST;

bool startCornerSeen = false;   
bool expectStartNext  = false; 

int  cornerCount = 0;           // 1..4
int  lapCount    = 0;

unsigned long firstTs = 0, cooldownUntil = 0;
Col firstColorLocal;

uint16_t PAIR_WINDOW_MS = 1200;  
uint16_t COOLDOWN_MS    = 1500;  

// ================== Forward Decls ==================
long readUltrasonicCM();
int  readColor(bool s2State, bool s3State);
int  readChannelMedian(bool s2, bool s3);
Col  readMatColorNorm();
void updateLapLogicCW();

void forward();
void stopMotors();
void turnLeftAvoid();
void turnRightAvoid();

// ================== Setup ==================
void setup() {
  Serial.begin(115200);

  // Motors
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);

  // Servo
  steeringServo.attach(SERVO_PIN);
  steeringServo.write(SERVO_CENTER);

  // Ultrasonic
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Color sensor
  pinMode(S0, OUTPUT); pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT); pinMode(S3, OUTPUT);
  pinMode(OUT_PIN, INPUT);
  digitalWrite(S0, HIGH); // 20% scaling
  digitalWrite(S1, LOW);

  // Start button
  pinMode(START_BTN, INPUT_PULLUP);

  stopMotors();
  Serial.println("Waiting for START (press BOOT)...");
}

// ================== Loop ==================
void loop() {
  // ---- Start gate ----
  if (!started) {
    if (digitalRead(START_BTN) == LOW) {
      delay(30);
      if (digitalRead(START_BTN) == LOW) {
        started = true;
        Serial.println("START pressed -> begin run");
      }
    }
    stopMotors();
    steeringServo.write(SERVO_CENTER);
    delay(5);
    return;
  }

  if (lapsDone) {
    stopMotors();
    steeringServo.write(SERVO_CENTER);

    return;
  }

  // ---- Obstacle avoidance  ----
  long distance = readUltrasonicCM();
  if (distance > 0 && distance < OBSTACLE_DISTANCE) {
    stopMotors();
    delay(200);

    int redFreq   = readColor(LOW, LOW);    // R
    int greenFreq = readColor(LOW, HIGH);   // G

    Serial.print("RED: ");   Serial.print(redFreq);
    Serial.print("  GREEN: "); Serial.println(greenFreq);

    if      (greenFreq > redFreq) turnLeftAvoid();    
    else if (redFreq   > greenFreq) turnRightAvoid(); 
    else turnRightAvoid(); 
  } else {
    forward();
    steeringServo.write(SERVO_CENTER);
  }

  // ---- Corner/Lap counting ----
  updateLapLogicCW();

  if (lapCount >= 3) {
    lapsDone = true;
    stopMotors();
    steeringServo.write(SERVO_CENTER);
    Serial.println("[State] 3 laps done (parking not implemented yet).");
  }

  delay(50);
}

// ================== Motor Helpers ==================
void forward() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  digitalWrite(ENA, HIGH); digitalWrite(ENB, HIGH);
}

void stopMotors() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
  digitalWrite(ENA, LOW); digitalWrite(ENB, LOW);
}

void turnLeftAvoid() {
  steeringServo.write(SERVO_LEFT);
  forward();
  delay(TURN_DELAY);
  steeringServo.write(SERVO_CENTER);
}

void turnRightAvoid() {
  steeringServo.write(SERVO_RIGHT);
  forward();
  delay(TURN_DELAY);
  steeringServo.write(SERVO_CENTER);
}

// ================== Ultrasonic ==================
long readUltrasonicCM() {
  digitalWrite(TRIG_PIN, LOW); delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return -1;
  long distance = duration * 0.034 / 2;
  return distance;
}

// ================== Color (Median + Normalize) ==================
int readChannelMedian(bool s2, bool s3){
  int a = readColor(s2,s3);
  int b = readColor(s2,s3);
  int c = readColor(s2,s3);
  if (a>b){int t=a;a=b;b=t;}
  if (b>c){int t=b;b=c;c=t;}
  if (a>b){int t=a;a=b;b=t;}
  return b;
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

// ================== Corner / Lap Logic (CW) ==================
void updateLapLogicCW(){
  unsigned long now = millis();
  if (now < cooldownUntil) return;

  // (1) أول زيارة: الأزرق منفرد = زاوية 1
  if (!startCornerSeen){
    if (readMatColorNorm() == C_BLUE){
      startCornerSeen = true;
      cornerCount = 1;
      cooldownUntil = now + 900; 
      Serial.println("[Corner] Start BLUE -> corner=1");
    }
    return;
  }


  Col c = readMatColorNorm();
  if (c==C_WHITE || c==C_OTHER) return;

  switch(pfsm){
    case WAIT_FIRST:
      if (c==C_BLUE || c==C_ORANGE){
        firstColorLocal = c;
        firstTs = now;
        pfsm = WAIT_SECOND;
      }
      break;

    case WAIT_SECOND:
      if (now - firstTs > PAIR_WINDOW_MS){ pfsm = WAIT_FIRST; break; }
      if (c!=firstColorLocal && (c==C_BLUE || c==C_ORANGE)){
        if (expectStartNext){
       
          if (firstColorLocal == C_ORANGE){
            lapCount++;
            Serial.printf("[Lap] Completed -> %d\n", lapCount);
          }
          expectStartNext = false;
          pfsm = WAIT_FIRST;
          cooldownUntil = now + COOLDOWN_MS;
          break;
        }

        // زاوية عادية = BLUE->ORANGE
        if (firstColorLocal==C_BLUE && c==C_ORANGE){
          cornerCount++;
          Serial.printf("[Corner] %d\n", cornerCount);

          if (cornerCount >= 4){
            cornerCount = 0;
            expectStartNext = true; // الركن القادم هو ركن البداية
          }
          pfsm = WAIT_FIRST;
          cooldownUntil = now + COOLDOWN_MS;
        } else {
          pfsm = WAIT_FIRST; 
        }
      }
      break;
  }
}

// ================== Raw TCS3200 reading ==================
int readColor(bool s2State, bool s3State) {
  digitalWrite(S2, s2State);
  digitalWrite(S3, s3State);
  delay(5); 
  int duration = pulseIn(OUT_PIN, HIGH, 50000); // 50ms timeout
  if (duration == 0) duration = 50000;
  int frequency = 1000000 / duration;
  return frequency;
}