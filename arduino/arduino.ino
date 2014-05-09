
/* ------------------
    Projektgrupp 2:6
   ------------------ */

// CAN Shield
#include <Canbus.h>
#include <defaults.h>
#include <global.h>
#include <mcp2515.h>
#include <mcp2515_defs.h>

// Motor Shield
#include <Wire.h>
#include <Adafruit_MotorShield.h>
//#include <Adafruit_PWMServoDriver.h>  <- remove??

// Joystick
#define UP     A1
#define RIGHT  A2
#define DOWN   A3
#define CLICK  A4
#define LEFT   A5

// LED
#define LED2 8
#define LED3 7
//int LED2 = 8;  <- remove??
//int LED3 = 7;  <- remove??

tCAN message;

//int led = 13;  <- remove??
//unsigned char buffer[16];  <- remove??

/* ------------- SETUP ------------- */
void setup() {

  pinMode(LED2, OUTPUT); 
  pinMode(LED3, OUTPUT); 
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);

  pinMode(UP, INPUT);
  pinMode(DOWN, INPUT);
  pinMode(LEFT, INPUT);
  pinMode(RIGHT, INPUT);
  pinMode(CLICK, INPUT);
  digitalWrite(UP, HIGH);
  digitalWrite(DOWN, HIGH);
  digitalWrite(LEFT, HIGH);
  digitalWrite(RIGHT, HIGH);
  digitalWrite(CLICK, HIGH);

  Serial.begin(9600);

  if (Canbus.init(CANSPEED_125)) {
    Serial.println("CAN initialized...");
  } else {
    Serial.println("CAN initialization ERROR!");
  }
  message.id = 0x00;
  message.header.rtr = 0;
  message.header.length = 0;
  message.data[0] = 0x00;
  message.data[1] = 0x00;
  message.data[2] = 0x00;
  message.data[3] = 0x00;
  message.data[4] = 0x00;
  message.data[5] = 0x00;
  message.data[6] = 0x00;
  message.data[7] = 0x00;
}

/* ------------- LOOP ------------- */
void loop() {

  // Read from CAN bus
  if (mcp2515_check_message()) {
    digitalWrite(LED2, HIGH);
  }

  // Read from joystick
  if (digitalRead(UP) == 0) {
    Serial.println("UP: Helljus");
    message.id = 0x133;
    message.header.rtr = 0;
    message.header.length = 1;
    message.data[0] = 0x01;
    if (mcp2515_send_message(&message)) {
      Serial.println("SENT");
      digitalWrite(LED2, LOW);
    }
    delay(300);
    
  } else if (digitalRead(DOWN) == 0) {
    Serial.println("DOWN: Bromsljus");
    message.id = 0x122;
    message.header.rtr = 0;
    message.header.length = 1;
    message.data[0] = 0x01;
    if (mcp2515_send_message(&message)) {
      Serial.println("SENT");
      digitalWrite(LED2, LOW);
    }
    delay(300);
    
  } else if (digitalRead(LEFT) == 0) {
    Serial.println("LEFT: Blinkers");
    message.id = 0x144;
    message.header.rtr = 0;
    message.header.length = 1;
    message.data[0] = 0x01;
    if (mcp2515_send_message(&message)) {
      Serial.println("SENT");
      digitalWrite(LED3, LOW);
    }
    delay(300);
    
  } else if (digitalRead(RIGHT) == 0) {
    Serial.println("RIGHT: Blinkers");
    message.id = 0x144;
    message.header.rtr = 0;
    message.header.length = 1;
    message.data[0] = 0x02;
    if (mcp2515_send_message(&message)) {
      Serial.println("SENT");
      digitalWrite(LED2, HIGH);
    }
    delay(300);
    
  } else if (digitalRead(CLICK) == 0) {
    Serial.println("CLICK: Motor");
    // start or stop motor
    delay(300);
  }
}

