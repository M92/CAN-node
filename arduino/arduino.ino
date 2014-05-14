
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

//LCD Display
#include <LiquidCrystal.h>
#include "utility/Adafruit_PWMServoDriver.h"

// LED's
#define LED2 8
#define LED3 7

// Global Variables
tCAN message;
int motorSpeed = 0;
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *myMotor = AFMS.getMotor(1); // Port M1
LiquidCrystal lcd(12,11,5,4,3,2);


/* ------------- SETUP ------------- */

void setup()
{
  pinMode(LED2, OUTPUT); 
  pinMode(LED3, OUTPUT); 
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  
  //LCD
  lcd.begin(16,2); 
  lcd.print("Motor RMPM");

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
    Serial.println("CAN initialized.");
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
  myMotor->run(FORWARD);
  myMotor->run(RELEASE);
  
  Serial.println("Setup Finished.\n");
}

/* ------------- LOOP ------------- */

void loop()
{
  // Handle joystick input...
  if (digitalRead(UP) == 0) {
    Serial.print("UP: ");
    message.id = 0x133;
    message.header.rtr = 0;
    message.header.length = 1;
    message.data[0] = 0x01;
    sendMessage(&message);
    
  } else if (digitalRead(DOWN) == 0) {
    Serial.print("DOWN: ");
    message.id = 0x122;
    message.header.rtr = 0;
    message.header.length = 1;
    message.data[0] = 0x01;
    sendMessage(&message);
    
  } else if (digitalRead(LEFT) == 0) {
    Serial.print("LEFT: ");
    message.id = 0x144;
    message.header.rtr = 0;
    message.header.length = 1;
    message.data[0] = 0x01;
    sendMessage(&message);
    
  } else if (digitalRead(RIGHT) == 0) {
    Serial.print("RIGHT: ");
    message.id = 0x144;
    message.header.rtr = 0;
    message.header.length = 1;
    message.data[0] = 0x02;
    sendMessage(&message);
    
  } else if (digitalRead(CLICK) == 0) {
    Serial.print("CLICK: ");
    if (motorSpeed) { // The motor is running
      runMotor(0);    // Stop the motor
    } else {          // The motor is not running
      runMotor(30);   // Start the motor
    }
    delay(300); // Joystick sensitivity
  }
  
  // Check if there are any new messages waiting
  if (mcp2515_check_message()) {
    digitalWrite(LED2, HIGH);
    Serial.print("CAN: ");
    if (mcp2515_get_message(&message)) {
      // Look for the motor control message
      if (message.header.rtr == 0 && message.id == 0x111) {
        runMotor(message.data[0]);
      } else {
        // Not a motor control message
        Serial.println("wrong formatting");
      }
    } else {
      // mcp2515_get_message failed
      Serial.println("error");
    }
  }
}

/* ----------- FUNCTIONS ----------- */

void sendMessage(tCAN *message)
{
  if (!mcp2515_check_free_buffer()) {
    Serial.println("buffer is full");
  } else if (mcp2515_send_message(message)) {
    digitalWrite(LED2, LOW);
    Serial.println("sent");
  } else {
    // mcp2515_send_message failed
    Serial.println("error");
  }
  delay(300); // Joystick sensitivity
}

void runMotor(int rpm)
{
  motorSpeed = rpm;
  myMotor->run(FORWARD);
  myMotor->setSpeed(rpm);
  Serial.print("RPM ");
  Serial.println(rpm);
  lcdPrintRPM(rpm);
  }
  
void lcdPrintRPM(int i ) 
{
  int val;
  char* Str1[17] = {" ","|","||","|||","||||","|||||","||||||","|||||||","||||||||","|||||||||","||||||||||","|||||||||||","||||||||||||","|||||||||||||","||||||||||||||", "|||||||||||||||","||||||||||||||||"};
  lcd.setCursor(0,0);
  lcd.print("Motor RPM ");
  lcd.print(i);
  val = map(i, 0,256, 0,17);
  lcd.setCursor(0,1);
  lcd.print(Str1[val]);
  delay(50);
  lcd.clear();
  }
