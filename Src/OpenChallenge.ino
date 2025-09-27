#include <ESP32Servo.h>

#define IN1 14
#define IN2 12
#define IN3 27
#define IN4 25
#define ENA 26
#define ENB 17

#define SERVO_PIN 13
Servo steeringServo;

#define SERVO_LEFT   45
#define SERVO_CENTER 90
#define SERVO_RIGHT  135

void setup() {
  Serial.begin(115200);

  // إعداد المحركات
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  // إعداد السيرفو
  steeringServo.attach(SERVO_PIN);
  steeringServo.write(SERVO_CENTER);

  Serial.println("Starting Open Challenge...");
}

void loop() {
  // تنفيذ المهمة: 3 لفات مع عقارب الساعة
  for (int i = 0; i < 3; i++) {
    moveForward();
    delay(3000); // تقدير مدة مستقيمة
    turnRight();
  }

  // بعد 3 لفات يرجع لنقطة البداية
  moveBackward();
  delay(4000); // يرجع للخلف نفس المسافة

  stopMotors();
  while (true); // يوقف البرنامج
}

// ------------ دوال الحركة ------------
void moveForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
}

void moveBackward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  digitalWrite(ENA, LOW);
  digitalWrite(ENB, LOW);
}

void turnRight() {
  steeringServo.write(SERVO_RIGHT);
  moveForward();
  delay(800); // مدة الالتفاف لربع لفة
  steeringServo.write(SERVO_CENTER);
}
