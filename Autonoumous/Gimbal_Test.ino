#include <Servo.h>

Servo myservo; // Green band on wire
Servo swivel; // 93 is approximately stop point
int pos = 70;
int tilt = 0;
int pan = 0;
int rest = 93;
int myHash = 0;
int hash = 0;
char *out = "456465465455465446 ";

void setup() {
  // put your setup code here, to run once:
  myservo.attach(2);
  swivel.attach(5);
  Serial.begin(9600);

  myservo.write(pos);
  delay(15);
  swivel.write(rest);
  delay(15);

}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available() > 0)
  {
    pan = Serial.parseInt();
    tilt = Serial.parseInt();
    hash = Serial.parseInt();
    myHash = pan + tilt;
    if (myHash == hash) { // We good
      look();
      sprintf(out, "%d,%d,%d",pan, tilt, hash);
      Serial.println(out); 
    }
    else{
      sprintf(out, "%d,%d,%d!!!!!!!!!!",pan, tilt, hash);
      Serial.println(out);
    }
  }
}
void look(){
        // Pan
      switch(pan){
        case 1:
          swivel.write(rest - 3);
          delay(15);
          break;
        case 2:
          // Pan right
          swivel.write(rest + 3);
          delay(15);
        case 0:
          swivel.write(rest);
          break;
        default:
          swivel.write(rest);
      }
      // Tilt
      myservo.write(tilt);
}

//  for (pos = 0; pos <= 180; pos += 1)
//  {
//    myservo.write(pos);
//    delay(15);
//  }
//  for ( pos = 180; pos >= 0; pos -=1)
//  {
//    myservo.write(pos);
//    delay(15);
//  }
//  for (rest = 93; rest <= 100; rest += 1)
//  {
//    swivel.write(rest);
//    delay(15);
//  }
//  for (rest = 100; rest >= 86; rest -=1)
//  {
//    swivel.write(rest);
//    delay(15);
//  }
//  for (rest = 86; rest <= 93; rest += 1)
//  {
//    swivel.write(rest);
//    delay(15);
//  }
//  Serial.println("yo");
//  delay(500000);
