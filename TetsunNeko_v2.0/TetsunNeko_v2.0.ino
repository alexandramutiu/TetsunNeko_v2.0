#include <Wire.h>
#include <Adafruit_VL53L0X.h>

Adafruit_VL53L0X tof;

#define START_STOP_PIN 22

#define TOF_LIMIT_CLOSE 700
#define TOF_LIMIT_FAR   1200

#define SHARP_PIN A3
#define SHARP_LIMIT 300

#define LIGHT_L A0
#define LIGHT_C A1
#define LIGHT_R A2
#define LIGHT_LIMIT 500

#define ENA 5
#define ENB 6
#define IN1 8
#define IN2 9
#define IN3 10
#define IN4 11

int motorSpeed = 220;

void setup() {
  Serial.begin(9600);
  delay(1000);

  pinMode(START_STOP_PIN, INPUT);

  pinMode(SHARP_PIN, INPUT);
  pinMode(LIGHT_L, INPUT);
  pinMode(LIGHT_C, INPUT);
  pinMode(LIGHT_R, INPUT);

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  stopMotors();

  Wire.begin();

  Serial.println("Starting TOF...");
  if (!tof.begin()) {
    Serial.println("TOF FAILED");
    while (1) {
      stopMotors();
    }
  }

  Serial.println("TOF OK");
}

void loop() {
  if (digitalRead(START_STOP_PIN) == LOW) {
    stopMotors();
    Serial.println("STOPPED");
    delay(100);
    return;
  }

  stopMotors();
  delay(10);

  int lightL = analogRead(LIGHT_L);
  int lightC = analogRead(LIGHT_C);
  int lightR = analogRead(LIGHT_R);

  bool whiteLine =
    lightL > LIGHT_LIMIT ||
    lightC > LIGHT_LIMIT ||
    lightR > LIGHT_LIMIT;

  if (whiteLine) {
    setMotors(255,255);
    Serial.println("WHITE LINE - Go forward");
    delay(100);
    return;
  }

  VL53L0X_RangingMeasurementData_t m;
  tof.rangingTest(&m, false);

  int tofDistance = m.RangeMilliMeter;
  int tofStatus = m.RangeStatus;

  int sharpValue = analogRead(SHARP_PIN);

  bool tofVeryClose =
    tofStatus != 4 &&
    tofDistance > 50 &&
    tofDistance <= TOF_LIMIT_CLOSE;

  bool tofSees =
    tofStatus != 4 &&
    tofDistance > 50 &&
    tofDistance <= TOF_LIMIT_FAR;

  bool leftSees =
    sharpValue > SHARP_LIMIT;

  Serial.print("TOF=");
  Serial.print(tofDistance);
  Serial.print(" | TOF_CLOSE=");
  Serial.print(tofVeryClose ? 1 : 0);

  Serial.print(" | TOF_SEES=");
  Serial.print(tofSees ? 1 : 0);

  Serial.print(" | LEFT_SHARP=");
  Serial.print(sharpValue);
  Serial.print(" | LEFT_SEES=");
  Serial.print(leftSees ? 1 : 0);

  Serial.print(" | ACTION=");

  // TOF has priority. If front sees anything up to 1200 mm, attack forward.
  if (tofSees) {
    Serial.println("ATTACK_FRONT");
    forwardPulse();
  }
  else if (leftSees) {
    Serial.println("SPIN_LEFT_THEN_ATTACK");
    spinLeftPulse();
    safeDelay(100);
    forwardPulse();
  }
  else {
    Serial.println("SEARCH_RIGHT");
    spinRightPulse();
  }

  delay(20);
}

void forwardPulse() {
  setMotors(245, 245);
  //safeDelay(260);
 // stopMotors();
}

void spinLeftPulse() {
  setMotors(-180, 180);
  safeDelay(120);
 // stopMotors();
}

void spinRightPulse() {
  setMotors(220, -220);
  safeDelay(160);
 // stopMotors();
}

void setMotors(int leftSpeed, int rightSpeed) {
  if (digitalRead(START_STOP_PIN) == LOW) {
    stopMotors();
    return;
  }

  leftSpeed = constrain(leftSpeed, -255, 255);
  rightSpeed = constrain(rightSpeed, -255, 255);

  if (leftSpeed > 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, leftSpeed);
  } else if (leftSpeed < 0) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, -leftSpeed);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 0);
  }

  if (rightSpeed > 0) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, rightSpeed);
  } else if (rightSpeed < 0) {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENB, -rightSpeed);
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, 0);
  }
}

void safeDelay(int ms) {
  unsigned long startTime = millis();

  while (millis() - startTime < ms) {
    if (digitalRead(START_STOP_PIN) == LOW) {
      stopMotors();
      return;
    }
    delay(5);
  }
}

void stopMotors() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
