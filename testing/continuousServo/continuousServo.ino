/*
 * this file will show the basics of controlling continuous servos
 * to make it easy, you just type in the speed you want into the command line
 * and the arduino will update it.
 * To use it, enter the speed you want from 0 to 100.
 * 0 means no movement and 100 means as fast as it can go.
 * to go clockwise, send a positive value.
 * to go counter-clockwise, send a negative number.
 * EX. clockwise at half speed type 50
 * counter-clockwise at 3/4 max speed type -75
 */
#include <Servo.h>

Servo servo;
long speed = 50;

int cw = 5;
bool cwState = LOW;
int ccw = 6;
bool ccwState = LOW;

//scales input value to the range of -180 to 180
long map(long x){
  return (x - -100) * (255 - 0) / (100 - -100) + 0;
}

void setup() {
  Serial.begin(9600);
  pinMode(cw,INPUT);
  pinMode(ccw,INPUT);
  servo.attach(9);
  servo.write(128);

  //waits for the serial port to connect
  //while(!Serial);
  //Serial.println("Hello world!");
}

void loop() {
  Serial.println(digitalRead(cw));
  if(digitalRead(cw)){
    servo.write(150);
  }else if(digitalRead(ccw)){
    servo.write(100);
  }else
    servo.write(128);
}
