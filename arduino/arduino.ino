
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

//LCD Display
#include <LiquidCrystal.h>

// Joystick
#define UP     A1
#define RIGHT  A2
#define DOWN   A3
#define CLICK  A4
#define LEFT   A5

// Global Variables
tCAN message;
int motorSpeed = 0;
boolean changedSpeed = false;
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *myMotor = AFMS.getMotor(1); // Port M1
LiquidCrystal lcd(4,5,6,7,8,9);


/* ------------- SETUP ------------- */

void setup()
{  
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

  //Serial.begin(9600);
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
  
  attachInterrupt(0,messageISR,FALLING);
  //attachInterrupt(1,breakISR,FALLING);
  
  AFMS.begin();
  myMotor->setSpeed(0);
  myMotor->run(FORWARD);
  myMotor->run(RELEASE);
  
  lcd.begin(16,2);
  lcdPrintRPM(0);

  Serial.println("Setup Finished.\n");
}


/* ------------- LOOP ------------- */

void loop()
{
  // Handle joystick input...
  if (digitalRead(UP) == 0) {
    Serial.print("UP: ");
    message.id = 0x122;
    message.header.rtr = 0;
    message.header.length = 1;
    message.data[0] = 0x01;
    sendMessage(&message);
    
  } else if (digitalRead(DOWN) == 0) {
    Serial.print("DOWN: ");
    message.id = 0x122;
    message.header.rtr = 0;
    message.header.length = 1;
    message.data[0] = 0x02;
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
    if (motorSpeed) {
      Serial.println("off");
      runMotor(0); // Stop the motor if it is running
    } else {
      Serial.println("on");
      runMotor(26); // Start the motor otherwise
    }
    delay(300); // Joystick sensitivity
  }
  
  if (changedSpeed) {
    runMotor(motorSpeed);
    changedSpeed = false;
  }
}


/* ----------- FUNCTIONS ----------- */

void breakISR()
{
  Serial.println("joystickISR");
}

void messageISR()
{
  Serial.print("CAN: ");
  if (mcp2515_get_message(&message)) {
    // Look for the motor control message
    if (message.header.rtr == 0 && message.id == 0x111) {
      if (motorSpeed != message.data[0]) {
        motorSpeed = message.data[0];
        motorSpeed = map(motorSpeed, 0,107, 0, 255);
        changedSpeed = true;
        Serial.println("changed speed");
      } else {
        Serial.println("same speed");
      }
    } else {
      // Not a motor control message
      Serial.println("wrong formatting");
    }
   } else {
    // mcp2515_get_message failed
    Serial.println("error");
  }
}

void sendMessage(tCAN *message)
{
  if (!mcp2515_check_free_buffer()) {
    Serial.println("buffer is full");
  } else if (mcp2515_send_message(message)) {
    Serial.println("sent");
  } else {
    // mcp2515_send_message failed
    Serial.println("error");
  }
  delay(300); // Joystick sensitivity
}

void runMotor(int rpm)
{
  motorSpeed = rpm; // Update the global variable
  myMotor->run(FORWARD); // Ensure normal operation
  myMotor->setSpeed(rpm);
  Serial.print("RPM ");
  Serial.println(rpm);
  lcdPrintRPM(rpm);
}

void lcdPrintRPM(int rpm) 
{
  char* barGraph[17] = {" ","|","||","|||","||||","|||||","||||||","|||||||","||||||||","|||||||||","||||||||||","|||||||||||","||||||||||||","|||||||||||||","||||||||||||||", "|||||||||||||||","||||||||||||||||"};
  
  // Re-map the rpm for the display
  int bar = map(rpm,0,256,0,17);
  int percent = map(rpm,0,255,0,100);
  
  // Clear previous text on the display
  lcd.clear();
  
  // Begin printing on the first row
  lcd.setCursor(0,0);
  lcd.print("Motor ");
  if (rpm == 0) {
    lcd.print("Off");
  } else {
    lcd.print("RPM ");
    lcd.print(percent);
    lcd.print(" %");
  }
  
  // Continue printing on the second row
  lcd.setCursor(0,1);
  lcd.print(barGraph[bar]);
}

