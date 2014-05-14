#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"


Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *myMotor = AFMS.getMotor(1); // For M1
void runMotor(int i);

void setup() {
  Serial.begin(9600);
  AFMS.begin();
  myMotor->setSpeed(0);
  myMotor->run(RELEASE);
  myMotor->run(FORWARD);
}

void loop() {
  int a = Serial.read()-'0';
  runMotor(a);
  Serial.flush();

}
void runMotor(int i) {
  myMotor->run(FORWARD);
  myMotor->setSpeed(i);
}
