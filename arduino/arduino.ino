
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

// Joystick
#define UP     A1
#define RIGHT  A2
#define DOWN   A3
#define CLICK  A4
#define LEFT   A5

// LED
#define LED2 8
#define LED3 7

tCAN message;

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *myMotor = AFMS.getMotor(1); // For M1

int motorSpeed = 0;  //motorn körs ej
void runMotor(int i);

void sendMessage(tCAN *message);

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
  Serial.println();

  if (Canbus.init(CANSPEED_125)) {
    Serial.println("CAN initialized...");
  } else {
    Serial.println("CAN initialization error!");
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
  
  AFMS.begin();
  myMotor->setSpeed(0);
  myMotor->run(RELEASE);
  myMotor->run(FORWARD);
  
  Serial.println("Setup Finished.\n");
}

/* ------------- LOOP ------------- */
void loop() {

  // Read from joystick
  if (digitalRead(UP) == 0) {
    Serial.println("UP: Helljus");
    message.id = 0x133;
    message.header.rtr = 0;
    message.header.length = 1;
    message.data[0] = 0x01;
    sendMessage(&message);
    
  } else if (digitalRead(DOWN) == 0) {
    Serial.println("DOWN: Bromsljus");
    message.id = 0x122;
    message.header.rtr = 0;
    message.header.length = 1;
    message.data[0] = 0x01;
    
    sendMessage(&message);
    
  } else if (digitalRead(LEFT) == 0) {
    Serial.println("LEFT: Blinkers");
    message.id = 0x144;
    message.header.rtr = 0;
    message.header.length = 1;
    message.data[0] = 0x01;
    
    sendMessage(&message);
    
  } else if (digitalRead(RIGHT) == 0) {
    Serial.println("RIGHT: Blinkers");
    message.id = 0x144;
    message.header.rtr = 0;
    message.header.length = 1;
    message.data[0] = 0x02;
    
    sendMessage(&message);
    
  } else if (digitalRead(CLICK) == 0) {
    Serial.println("CLICK: Motor");
    if (motorSpeed)  {
      runMotor(0);  //stoppa motorn
      Serial.println("Stopped");
    }
    else  {
      runMotor(60);  //kör motorn med lägre hastighet
      Serial.println("RPM: 60");
    }
    delay(300);
  }
  
    // Read from CAN bus
  if (mcp2515_check_message()) {
    digitalWrite(LED2, HIGH);
    if (mcp2515_get_message(&message)) {
      if (message.header.rtr == 0 && message.id == 0x111)  {
        runMotor(message.data[0]);
        Serial.print("RPM: ");
        Serial.println(message.data[0]);
      }
    } else {
      Serial.println("Error getting message!");
    }
  }
}

void runMotor(int i) {
  motorSpeed = i;
  myMotor->run(FORWARD);
  myMotor->setSpeed(i);
}

void sendMessage(tCAN *message)  {
  
  if (!mcp2515_check_free_buffer())  {
    Serial.println("Buffer full!");
  }
  else if (mcp2515_send_message(message)) {
      Serial.println("SENT");
      digitalWrite(LED2, LOW);
  }
  else  {
    Serial.println("Error! Not sent!");
  }
  delay(300);
}

