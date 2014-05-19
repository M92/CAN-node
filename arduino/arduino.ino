/*
      Projektgrupp 2:6
      - - - - - - - -
      Marcus Thorström
      Magnus Rising
      Vidar Åsberg
      Johan Strand
      Sanny Mitra
*/


/* --------------- INCLUDES ---------------- */

// CAN shield
#include <Canbus.h>
#include <defaults.h>
#include <global.h>
#include <mcp2515.h>
#include <mcp2515_defs.h>

// Motor shield
#include <Wire.h>
#include <Adafruit_MotorShield.h>

// Display
#include <LiquidCrystal.h>


/* ---------------- DEFINES ---------------- */

// Joystick
#define J_UP       A1
#define J_RIGHT    A2
#define J_DOWN     A3
#define J_CLICK    A4
#define J_LEFT     A5
#define J_DELAY    300

// CAN ID's
#define ID_MOTOR   0x111
#define ID_UP      0x122
#define ID_DOWN    0x122
#define ID_LEFT    0x144
#define ID_RIGHT   0x144

// CAN DATA
#define CAN_OFF     0x00
#define CAN_RPM     0x6B
#define CAN_UP      0x01
#define CAN_DOWN    0x02
#define CAN_LEFT    0x01
#define CAN_RIGHT   0x02

// Motor control
#define OFF         0
#define MAX_RPM     255
#define MOTOR_PORT  1


/* ---------------- GLOBALS ---------------- */

Adafruit_DCMotor *motor;
int current_speed = OFF;
boolean new_speed = false;

// LCD with 4 data lines and no RW pin
LiquidCrystal lcd(4,5,6,7,8,9);


/* ----------------- SETUP ----------------- */

void setup()
{
    // Initialize the display
    lcd.begin(16, 2);
    
    // Initialize the CAN shield
    if (!Canbus.init(CANSPEED_125))
        print_debug("CAN init error");
    
    // Initialize the joystick
    pinMode(J_UP, INPUT);
    pinMode(J_DOWN, INPUT);
    pinMode(J_LEFT, INPUT);
    pinMode(J_RIGHT, INPUT);
    pinMode(J_CLICK, INPUT);
    digitalWrite(J_UP, HIGH);
    digitalWrite(J_DOWN, HIGH);
    digitalWrite(J_LEFT, HIGH);
    digitalWrite(J_RIGHT, HIGH);
    digitalWrite(J_CLICK, HIGH);
    
    // Initialize the motor shield
    Adafruit_MotorShield motor_shield = Adafruit_MotorShield();
    motor_shield.begin();
    
    // Initialize the motor
    motor = motor_shield.getMotor(MOTOR_PORT);
    motor_control(OFF);
    
    // Initialize interrupts
    attachInterrupt(0, receive_message, FALLING);
}


/* ----------------- LOOP ------------------ */

void loop()
{
    // Change the motor speed after interrupts
    if (new_speed) {
        motor_control(current_speed);
        new_speed = false;
    }
    
    // Continuously look for joystick input
    if (digitalRead(J_UP) == 0) {
        send_message(ID_UP, CAN_UP);
        delay(J_DELAY);
    } else if (digitalRead(J_DOWN) == 0) {
        send_message(ID_DOWN, CAN_DOWN);
        delay(J_DELAY);
    } else if (digitalRead(J_LEFT) == 0) {
        send_message(ID_LEFT, CAN_LEFT);
        delay(J_DELAY);
    } else if (digitalRead(J_RIGHT) == 0) {
        send_message(ID_RIGHT, CAN_RIGHT);
        delay(J_DELAY);
    } else if (digitalRead(J_CLICK) == 0) {
        if (current_speed == OFF)
            motor_control(MAX_RPM); // Start the motor if it is off
        else
            motor_control(OFF);     // Stop the motor if it is running
        delay(J_DELAY);
    }
}


/* --------------- FUNCTIONS --------------- */

void receive_message()
{
    tCAN message;
    
    // Try to get the new message
    if (mcp2515_get_message(&message)) {
        // Look for the motor control message
        if (message.header.rtr == 0 && message.id == ID_MOTOR) {
            // Re-map the incoming data to RPM values
            int rpm = map(message.data[0], CAN_OFF, CAN_RPM, OFF, MAX_RPM);
            // Only update the speed if it is different
            if (current_speed != rpm) {
                current_speed = rpm;
                new_speed = true;
            }
        }
    }
}

void send_message(int id, int data)
{
    tCAN message;
    
    message.id = id;
    message.header.rtr = 0;
    message.header.length = 1;
    message.data[0] = data;
    
    if (!mcp2515_check_free_buffer()) {
        print_debug("buffer is full");
    } else if (!mcp2515_send_message(&message)) {
        print_debug("error sending");
    }
}

void motor_control(int rpm)
{
    current_speed = rpm;  // Remember the current RPM
    motor->run(FORWARD);  // Set the direction of rotation
    motor->setSpeed(rpm); // Change the motor RPM
    print_rpm(rpm);       // Update the RPM on the display
}

void print_rpm(int rpm) 
{
    // Bar graph for simulating the motor speed on the display
    char *graph[] = {" ","|","||","|||","||||","|||||","||||||","|||||||","||||||||","|||||||||","||||||||||","|||||||||||","||||||||||||","|||||||||||||","||||||||||||||","|||||||||||||||","||||||||||||||||"};
    
    int bar = map(rpm, OFF, MAX_RPM, 0, 16);      // Re-map RPM for the bar graph
    int percent = map(rpm, OFF, MAX_RPM, 0, 100); // Re-map RPM to percentage
    
    lcd.clear();         // Clear the current text on the display
    lcd.setCursor(0, 0); // Begin printing on the first row
    lcd.print("Motor ");
    if (rpm == OFF) {
        lcd.print("Off");
    } else {
        lcd.print("RPM ");
        lcd.print(percent);
        lcd.print(" %");
    }
    lcd.setCursor(0, 1);   // Continue printing on the second row
    lcd.print(graph[bar]); // Print the bar graph
}

void print_debug(const char *info) 
{
    lcd.clear();         // Clear the current text on the display
    lcd.setCursor(0, 0); // Begin printing on the first row
    lcd.print("DEBUG");
    lcd.setCursor(0, 1); // Continue printing on the second row
    lcd.print(info);
    delay(5000);         // Pause for readability
}
