CAN node
========

This is the Arduino node in a project on CAN communication for a real-time systems course at Chalmers. The network consists of two microcontrollers, an Arduino Uno and an Freescale 68HC12.

This node is responsible for controlling voltage to a motor based on motor control information sent by the other node. It also sends commands from a joystick to the other node.

*Due to incompatibility between the shields it's not possible to utilize interrupts for the joystick, only for CAN messages.*

Hardware
--------
* Arduino Uno
* Arduino CAN shield DEV-10039
* Adafruit Motorshield V2
* LCD Display
* Motor

![lcd](https://cloud.githubusercontent.com/assets/6613394/5481045/5bf7de92-864c-11e4-8d2f-711f74a50512.jpg)

![can-shield](https://cloud.githubusercontent.com/assets/6613394/5481050/5f2e91d2-864c-11e4-82a0-37eb3ae0356a.jpg)
