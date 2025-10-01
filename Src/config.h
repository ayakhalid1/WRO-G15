#ifndef CONFIG_H
#define CONFIG_H

// ----- L298N -----
#define IN1 14
#define IN2 16
#define IN3 27
#define IN4 25
#define ENA 26
#define ENB 17

// ----- Servo -----
#define SERVO_PIN 13
#define SERVO_MIN_US 500
#define SERVO_MAX_US 2500
#define SERVO_CENTER 90
#define SERVO_RIGHT  60
#define SERVO_LEFT   120

// ----- Ultrasonic -----
#define TRIG_FRONT 33
#define ECHO_FRONT 32
#define TRIG_RIGHT 22
#define ECHO_RIGHT 35
#define TRIG_LEFT  21
#define ECHO_LEFT  34

// ----- TCS3200 -----
#define S0 18
#define S1 19
#define S2 23
#define S3 5
#define OUT_PIN 4

// ----- PWM -----
static const int PWM_FREQ = 20000;
static const int PWM_RES  = 8;
static const int CH_A     = 0;
static const int CH_B     = 1;

// ----- Thresholds (cm) -----
static const int FRONT_STOP_CM = 18;
static const int FRONT_SLOW_CM = 30;
static const int SIDE_KEEP_CM  = 22;

// ----- Speeds (0..255) -----
static const uint8_t SPEED_CRUISE = 200;
static const uint8_t SPEED_SLOW   = 150;

// ----- Color -----
enum DetectedColor { COLOR_UNKNOWN, COLOR_RED, COLOR_GREEN };
static const int   COLOR_SAMPLES = 8;
static const float COLOR_MARGIN_RATIO = 1.20f;

#endif
