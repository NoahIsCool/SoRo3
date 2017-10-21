/*
 * this file will show the basics of controlling stepper servos
 * Personally I think this one is god-like compared to continuous
 * servos but whatever...
 * Anyways, for this one just type in the position you want the
 * servo to go to and the servo will go to that position as fast
 * as its little motor can go.
 * one day when you want to be all fancy like, like monical level fancy
 * you can type in something like this:
 * 40 5
 * BUT I cant seem to figure out how to get the arduino to read
 * in more than one number at a time...
 * What that will do is send the servo to position 40 in 5 seconds. 
 * In other words, if you give it 2 arguments
 * the first one will set the positio and the second one will
 * set how long it will take to get there in seconds.
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
int max = 270;
int min = 90;
int pos = 180;
int oldPos = 180;
int time = 0;

void setup() {
  Serial.begin(9600);
  servo.attach(9);
  servo.write(180);

  //waits for the serial port to connect
  while(!Serial);
  Serial.println("Hello world!");
}

void loop() {

  //just sets the position and goes there at max speed
  if(Serial.available()){
    pos = Serial.parseInt();
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
  }else if(time > 0){
    
  }
}

/*
 * this code is for setting the speed but I cant seem to read
 * it in correctly
 * 
 * //pos = Serial.parseInt();
    //time = Serial.parseInt();
    Serial.flush();
    if(pos > max || pos < min){
      Serial.print(pos);
      Serial.println(" too large...Or small...Try something else but better...");
      return;
    }
    Serial.print(pos);
    Serial.println(" pos is accepted");
    Serial.print(time);
    Serial.println(" seconds is also accepted");
    //speed algorithm. What it does is update the 
    int step = pos > oldPos ? 1 : -1;
    step = abs(pos - oldPos) / 10;
    int cycles = 10;
    for(int i = oldPos; i != pos; i+=step){
      servo.write(i);
      delay((time * 1000.0)/cycles);
    }
    oldPos = pos;
 */
