#include <ESP32Servo.h>

// ====== L298N Motor Pins ======
#define IN1 14
#define IN2 16    
#define IN3 27
#define IN4 25
#define ENA 26     // enable left motor
#define ENB 17     // enable right motor

// ====== Servo Pin  ======
#define SERVO_PIN 13
Servo steeringServo;

// ====== Ultrasonic Pins ======
#define TRIG_FRONT 33
#define ECHO_FRONT 32
#define TRIG_RIGHT 22
#define ECHO_RIGHT 35  
#define TRIG_LEFT  21
#define ECHO_LEFT  34   

// ====== TCS3200 (Color Sensor) —  ======
#define S0 18
#define S1 19
#define S2 23
#define S3 5
#define OUT_PIN 4
// OE -> GND

// ======  PWM للموتورات (LEDC) ======
const int PWM_FREQ = 20000;   
const int PWM_RES  = 8;     
const int CH_A     = 0;       
const int CH_B     = 1;      

// ====== إعدادات السيرفو ======
const int SERVO_MIN_US = 500;
const int SERVO_MAX_US = 2500;
const int SERVO_CENTER = 90;   // مستقيم
const int SERVO_RIGHT  = 60;   // لليمين (عدّل حسب ميكانيكك)
const int SERVO_LEFT   = 120;  // لليسار

// ====== عتبات مسافات (سم) ======
const int FRONT_STOP_CM    = 18;   // إيقاف/تجنّب قريب جدًا
const int FRONT_SLOW_CM    = 30;   // تهدئة
const int SIDE_KEEP_CM     = 22;   // للمساعدة في الانحياز/التمركز

// ====== سرعة الموتورات ======
const uint8_t SPEED_CRUISE = 200;  // سرعة سير
const uint8_t SPEED_SLOW   = 150;  // تهدئة
const uint8_t SPEED_STOP   = 0;

// ====== منطق اللون ======
enum DetectedColor { COLOR_UNKNOWN, COLOR_RED, COLOR_GREEN };


const int COLOR_SAMPLES = 8;

const float COLOR_MARGIN_RATIO = 1.20f; 

// ====== أدوات صغيرة ======
long readUltrasonicCM(uint8_t trigPin, uint8_t echoPin) {
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  unsigned long dur = pulseIn(echoPin, HIGH, 30000UL); // ~5m
  if (dur == 0) return 999;
  return dur / 58;
}


unsigned long readTCSFrequency(uint8_t s2, uint8_t s3) {
  digitalWrite(S2, s2);
  digitalWrite(S3, s3);
  delayMicroseconds(200);

  unsigned long total = 0;
  for (int i = 0; i < COLOR_SAMPLES; i++) {
    unsigned long t = pulseIn(OUT_PIN, LOW, 30000UL);
    if (t == 0) t = 30000UL; 
    total += t;
  }
  return total / COLOR_SAMPLES;
}

void readColorRG(unsigned long &fR, unsigned long &fG) {

  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  // أحمر: S2=LOW,  S3=LOW
  fR = readTCSFrequency(LOW, LOW);

  // أخضر: S2=HIGH, S3=HIGH
  fG = readTCSFrequency(HIGH, HIGH);
}

// إرجاع اللون (نعتمد مقارنة بسيطة على الترددات)
DetectedColor detectColor() {
  unsigned long fR, fG;
  readColorRG(fR, fG);

  // تردد أقل يعني لون أقوى (عكس الشدة)
  // نقرر حسب تفوق واضح بمقدار COLOR_MARGIN_RATIO
  bool redStronger   = (float)fG > COLOR_MARGIN_RATIO * (float)fR; // fR أصغر كثير ⇒ أحمر قوي
  bool greenStronger = (float)fR > COLOR_MARGIN_RATIO * (float)fG; // fG أصغر كثير ⇒ أخضر قوي

  if (redStronger)   return COLOR_RED;
  if (greenStronger) return COLOR_GREEN;
  return COLOR_UNKNOWN;
}

// ====== دوال الموتورات ======
void motorsCoast() {
  // إطلاق (لا كبح)
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
  ledcWrite(CH_A, 0);
  ledcWrite(CH_B, 0);
}

void motorsBrake() {

  digitalWrite(IN1, HIGH); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, HIGH);
  ledcWrite(CH_A, 0);
  ledcWrite(CH_B, 0);
}

void motorsForward(uint8_t speedA, uint8_t speedB) {
  // A
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  // B
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  ledcWrite(CH_A, speedA);
  ledcWrite(CH_B, speedB);
}

// ====== دوال التوجيه ======
void steerCenter() { steeringServo.write(SERVO_CENTER); }
void steerRightSoft() { steeringServo.write((SERVO_CENTER + SERVO_RIGHT) / 2); }
void steerRightHard() { steeringServo.write(SERVO_RIGHT); }
void steerLeftSoft()  { steeringServo.write((SERVO_CENTER + SERVO_LEFT) / 2); }
void steerLeftHard()  { steeringServo.write(SERVO_LEFT); }

// ====== الانحياز مع عقارب الساعة ======
void applyClockwiseBias(unsigned long ms=120) {
  steerRightSoft();
  delay(ms);
  steerCenter();
}

// ====== الإعداد ======
void setup() {
  Serial.begin(115200);

  // موتورات
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);

  ledcSetup(CH_A, PWM_FREQ, PWM_RES);
  ledcSetup(CH_B, PWM_FREQ, PWM_RES);
  ledcAttachPin(ENA, CH_A);
  ledcAttachPin(ENB, CH_B);

  // ألتراسونك
  pinMode(TRIG_FRONT, OUTPUT); pinMode(ECHO_FRONT, INPUT);
  pinMode(TRIG_RIGHT, OUTPUT); pinMode(ECHO_RIGHT, INPUT);
  pinMode(TRIG_LEFT,  OUTPUT); pinMode(ECHO_LEFT,  INPUT);

  // TCS3200
  pinMode(S0, OUTPUT); pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT); pinMode(S3, OUTPUT);
  pinMode(OUT_PIN, INPUT);

  // سيرفو
  steeringServo.attach(SERVO_PIN, SERVO_MIN_US, SERVO_MAX_US);
  steerCenter();

  motorsCoast();
  delay(400);
}

// ====== الحلقة الرئيسية ======
void loop() {
  // 1) قراءات المسافة
  long dF = readUltrasonicCM(TRIG_FRONT, ECHO_FRONT);
  long dR = readUltrasonicCM(TRIG_RIGHT, ECHO_RIGHT);
  long dL = readUltrasonicCM(TRIG_LEFT,  ECHO_LEFT);

  // 2) قراءة اللون (خفيفة؛ إن حبيت، ممكن تقرأها كل N دورات فقط)
  DetectedColor c = detectColor();

  // 3) منطق اللون يأخذ أولوية "توجيهية" (لكن لا يتجاوز أمان المسافة الأمامية)
  if (dF <= FRONT_STOP_CM) {
    // أمان أولًا: قريب جدًا من الأمام
    motorsBrake();
    // اختر مناورة بسيطة: افترض دوران يمين (مع عقارب الساعة)
    steerRightHard();
    delay(250);
    motorsForward(SPEED_SLOW, SPEED_SLOW);
    delay(250);
    steerCenter();
  } else {
    // الدرب مفتوح أماميًا
    uint8_t speed = (dF < FRONT_SLOW_CM) ? SPEED_SLOW : SPEED_CRUISE;

    // منطق اللون (إذا واضح) يوجّه حركة قصيرة، ثم نكمل
    if (c == COLOR_RED) {
      // أحمر ⇒ يمين
      steerRightHard();
      motorsForward(speed, speed);
      delay(220);
      steerCenter();
    } else if (c == COLOR_GREEN) {
      // أخضر ⇒ يسار
      steerLeftHard();
      motorsForward(speed, speed);
      delay(220);
      steerCenter();
    } else {
      // لون غير واضح ⇒ افتراضي انحياز يمين خفيف
      motorsForward(speed, speed);
      applyClockwiseBias(100); // نبضة يمين قصيرة
    }

    // 4) مساعدة جانبية بسيطة: إذا صار الجدار الأيمن قريب جدًا، ابتعد شوي يسار، والعكس
    if (dR < SIDE_KEEP_CM && dL > SIDE_KEEP_CM) {
      steerLeftSoft(); delay(120); steerCenter();
    } else if (dL < SIDE_KEEP_CM && dR > SIDE_KEEP_CM) {
      steerRightSoft(); delay(120); steerCenter();
    }
  }

  // Debug
  static unsigned long t0 = millis();
  if (millis() - t0 > 300) {
    Serial.print("F:"); Serial.print(dF); Serial.print("cm  ");
    Serial.print("R:"); Serial.print(dR); Serial.print("cm  ");
    Serial.print("L:"); Serial.print(dL); Serial.print("cm  ");
    Serial.print("  Color:");
    if (c == COLOR_RED) Serial.print("RED");
    else if (c == COLOR_GREEN) Serial.print("GREEN");
    else Serial.print("UNKNOWN");
    Serial.println();
    t0 = millis();
  }

  delay(10);
}