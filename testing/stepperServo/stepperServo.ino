/*
 * this file will show the basics of controlling stepper servos
 * Personally I think this one is god-like compared to continuous
 * servos but whatever...
 * Anyways, for this one just type in the position you want the
 * servo to go to and the servo will go to that position as fast
 * as its little motor can go.
 * if you want to be all fancy like, like monical level fancy
 * you can change the flag mode to 1 and it will now go to the
 * new position within a time. Just change the time to what you want
 * What that will do is send the servo to position 40 in 5 seconds. 
 * Dont go crazy with the time man.
 * 
 * You also have to watch out to make sure you dont give it a
 * position that is bigger than what the servo can handle.
 * Servos arnt perfect and it would be nice if they could go 
 * 360 degrees...but it doesnt work that way. For now I put it
 * at a safe range of motion from 90 to 270 degrees. You can
 * change it with the max and min variables.
 */
#include <Servo.h>

Servo servo; 
int max = 180;
int min = 0;
long pos = 180;
long oldPos = 180;
int time = 0;
bool mode = 0;

void setup() {
  Serial.begin(9600);
  servo.attach(9);
  servo.write(90);

  //waits for the serial port to connect
  while(!Serial);
  Serial.println("Hello world!");
}

void gotoPos(long pos){
  if(pos > max || pos < min){
      Serial.print(pos);
      Serial.println(" too large...Or small...Try something else but better...");
      return;
    }
    servo.write(pos);
    //just a delay to ensure it is finished moving before accepting
    //another position
    delay(15);  //delay 15 microseconds
    Serial.print(pos);
    Serial.println(" is accepted");
}

void gotoPosInTime(long pos,int time){
  if(pos > max || pos < min){
      Serial.print(pos);
      Serial.println(" too large...Or small...Try something else but better...");
      return;
    }
    Serial.println("going to ");
    Serial.print(pos);
    Serial.println(" in ");
    Serial.print(time);
    Serial.println(" seconds");
    //speed algorithm. What it does is update the the stepper motor position in 
    //regular intervals so that it will get to the disired position in time seconds
    int step = pos > oldPos ? 1 : -1;
    step = abs(pos - oldPos) / 10;
    int cycles = 10;
    for(int i = oldPos; i != pos; i+=step){
      servo.write(i);
      delay((time * 1000.0)/cycles);
    }
    oldPos = pos;
}

void loop() {

  //just sets the position and goes there at max speed
  if(Serial.available()){
    pos = Serial.parseInt();
    
    if(!mode){
	    gotoPos(pos);
    }else{
	    gotoPosInTime(pos,time);
    }
  }
}
