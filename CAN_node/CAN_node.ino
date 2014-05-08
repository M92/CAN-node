#include "Canbus.h"
#include "defaults.h"
#include "global.h"
#include "mcp2515.h"
#include "mcp2515_defs.h"


/* Define Joystick connection */
#define UP     A1
#define RIGHT  A2
#define DOWN   A3
#define CLICK  A4
#define LEFT   A5


int LED2 = 8;
int LED3 = 7;
unsigned char buffer[16];

int led = 13;
tCAN message;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED2, OUTPUT); 
  pinMode(LED3, OUTPUT); 
 
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  pinMode(UP,INPUT);
  pinMode(DOWN,INPUT);
  pinMode(LEFT,INPUT);
  pinMode(RIGHT,INPUT);
  pinMode(CLICK,INPUT);

  digitalWrite(UP, HIGH);       /* Enable internal pull-ups */
  digitalWrite(DOWN, HIGH);
  digitalWrite(LEFT, HIGH);
  digitalWrite(RIGHT, HIGH);
  digitalWrite(CLICK, HIGH);
  
  
  Serial.begin(9600);
  Serial.println("ECU Reader");  /* For debug use */
  
  if(Canbus.init(CANSPEED_125))  /* Initialise MCP2515 CAN controller at the specified speed */
  {
    Serial.println("CAN Init ok");
  } 
  else {
    Serial.println("Can't init CAN");
  } 
  message.id = 0x70F;
  message.header.rtr = 0;
  message.header.length = 8;
  message.data[0] = 0x16;
  message.data[1] = 0x00;
  message.data[2] = 0x00;
  message.data[3] = 0x00;
  message.data[4] = 0x00;
  message.data[5] = 0x00;
  message.data[6] = 0x00;
  message.data[7] = 0x00;
  
}

void loop() {
  // put your main code here, to run repeatedly:
  //upValue = analogRead(joyLeft);
  
  if(mcp2515_check_message()){
    digitalWrite(LED2,HIGH);
  }
  
  //helljus på
  if (digitalRead(UP) == 0) {
      Serial.println("UP");
      message.id = 0x70F;
      message.header.rtr = 0;
      message.header.length = 8;
      message.data[0] = 0x16;
      message.data[1] = 0x00;
      message.data[2] = 0x00;
      message.data[3] = 0x00;
      message.data[4] = 0x00;
      message.data[5] = 0x00;
      message.data[6] = 0x00;
      message.data[7] = 0x00;
      
      //skicka meddelandet på bussen
      if(mcp2515_send_message(&message)){
        Serial.println("SENT");
        digitalWrite(LED2, LOW);
      }
      delay(300);
  }
  //bromsljus på
  else if (digitalRead(DOWN) == 0) {
      Serial.println("DOWN");
      message.id = 0x301;
      message.header.rtr = 0;
      message.header.length = 8;
      message.data[0] = 0x16;
      message.data[1] = 0x00;
      message.data[2] = 0x00;
      message.data[3] = 0x00;
      message.data[4] = 0x00;
      message.data[5] = 0x00;
      message.data[6] = 0x00;
      message.data[7] = 0x00;
      if(mcp2515_send_message(&message)){
        Serial.println("SENT");
        digitalWrite(LED2, LOW);
      }
      delay(300);
      
  }
  //blinka vänster
  else if (digitalRead(LEFT) == 0) {
      Serial.println("LEFT");
      message.id = 0x202;
      message.header.rtr = 0;
      message.header.length = 1;
      message.data[0] = 0x00;
      if(mcp2515_send_message(&message)){
        Serial.println("SENT");
        digitalWrite(LED3, LOW);
      }
      delay(300);
      
  }
  //blinka höger
  else if (digitalRead(RIGHT) == 0) {
      Serial.println("RIGHT");
      message.id = 0x202;
      message.header.rtr = 0;
      message.header.length = 1;
      message.data[0] = 0x01;
     /* message.data[1] = 0x00;
      message.data[2] = 0x00;
      message.data[3] = 0x00;
      message.data[4] = 0x00;
      message.data[5] = 0x00;
      message.data[6] = 0x00;
      message.data[7] = 0x00;*/
      if(mcp2515_send_message(&message)){
        Serial.println("SENT");
        digitalWrite(LED2, HIGH);
      }
      delay(300);
      
  }
  //tEXTRA: starta och stoppa motor
  else if (digitalRead(CLICK) == 0) {
      Serial.println("CLICK");
      if(Canbus.message_rx(buffer)){
        Serial.print("Recieved");
        digitalWrite(LED2, HIGH);
      }
      delay(50);
  }
  
}

