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
long speed = 0;

//scales input value to the range of -180 to 180
long map(long x){
  return (x - -100) * (180 - 0) / (100 - -100) + 0;
}

void setup() {
  Serial.begin(9600);
  servo.attach(9);
  servo.write(map(0));

  //waits for the serial port to connect
  while(!Serial);
  Serial.println("Hello world!");
}

void loop() {
  if(Serial.available()){
    speed = Serial.parseInt();
    servo.write(map(speed));
    Serial.print("updated to ");
    Serial.println(map(speed));
  }
}
