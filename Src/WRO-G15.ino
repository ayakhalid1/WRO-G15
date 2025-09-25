#include <Arduino.h>
#include "config.h"
#include "motors.h"
#include "sensors.h"
#include "utils.h"

// ===== حالة البدء/اللفات =====
bool started = false;
bool lapsDone = false;

enum PairFSM { WAIT_FIRST, WAIT_SECOND };
PairFSM pfsm = WAIT_FIRST;

bool startCornerSeen = false;
bool expectStartNext = false;

int  cornerCount = 0;     // 1..4
int  lapCount    = 0;

unsigned long firstTs = 0, cooldownUntil = 0;
Col firstColorLocal;

uint16_t PAIR_WINDOW_MS = 1200;
uint16_t COOLDOWN_MS    = 1500;

// ===== تصريحات =====
void updateLapLogicCW();

// ================== Setup ==================
void setup() {
  Serial.begin(115200);

  motorsInit();
  sensorsInit();

  motorsStop();
  motorsSteerCenter();

  pinMode(START_BTN, INPUT_PULLUP);
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
    motorsStop();
    motorsSteerCenter();
    delay(5);
    return;
  }

  if (lapsDone) {
    motorsStop();
    motorsSteerCenter();
    return;
  }

  // ---- Obstacle avoidance ----
  long distance = readUltrasonicCM();
  if (distance > 0 && distance < OBSTACLE_DISTANCE) {
    motorsStop();
    delay(200);

    int redFreq   = readColor(LOW, LOW);    // R
    int greenFreq = readColor(LOW, HIGH);   // G

    Serial.print("RED: ");   Serial.print(redFreq);
    Serial.print("  GREEN: "); Serial.println(greenFreq);

    if      (greenFreq > redFreq) motorsAvoidLeftBurst();
    else if (redFreq   > greenFreq) motorsAvoidRightBurst();
    else motorsAvoidRightBurst();
  } else {
    motorsForward();
    motorsSteerCenter();
  }

  // ---- Corner/Lap counting ----
  updateLapLogicCW();

  if (lapCount >= 3) {
    lapsDone = true;
    motorsStop();
    motorsSteerCenter();
    Serial.println("[State] 3 laps done (parking not implemented yet).");
  }

  delay(50);
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
