#include <Servo.h>
Servo base, shoulder, elbow, leftWrist, rightWrist, leftClaw, rightClaw;
int bPos, sPos, ePos, wLPos, wRPos, cLPos, cRPos = 90;
int pos[] = {90, 90, 90, 90, 90, 90, 90};
void setup() {
  // put your setup code here, to run once:
  base.attach(2);
  shoulder.attach(3);
  elbow.attach(4);
  leftWrist.attach(5);
  rightWrist.attach(6);
  leftClaw.attach(7);
  rightClaw.attach(8);
}

// Given an array of integers, move to that position
void move(int pos[]) {
  base.write(pos[0]);
  delay(15);
  shoulder.write(pos[1]);
  delay(15);
  elbow.write(pos[2]);
  delay(15);
  leftWrist.write(pos[3]);
  delay(15);
  rightWrist.write(pos[4]);
  delay(15);
  leftClaw.write(pos[5]);
  delay(15);
  rightClaw.write(pos[6]);
  delay(15);
}

void loop() {
  // put your main code here, to run repeatedly:
  move(pos);
}
