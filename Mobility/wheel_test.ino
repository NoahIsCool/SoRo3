#include <Servo.h>

Servo wheel[6];


//wheel mapping
/*
       Left       Right
        0  0|xxx|0  3
            |xxx|   
        1  0|xxx|0  4
            |xxx|   
        2  0|xxx|0  5
*/
        

int clientHash = 0;
int myHash = 0;

int gimble = 0;
int tilt = 0;
int leftWheels = 0;
int rightWheels = 0;
int overdrive = 0;
int hach = 0;


char *serResp = " 1,190,200!!!!!!!!!!!!!!!!!\n";

//setup wheels
void setup() {
  wheel[0].attach(2);
  wheel[1].attach(3);
  wheel[2].attach(4);
  wheel[3].attach(5);
  wheel[4].attach(6);
  wheel[5].attach(7);
  
  Serial.begin(9600);
  Serial.setTimeout(1000);
}


void loop() {
  if(Serial.read() == 2){
    //read through message
        gimble = Serial.parseInt();
        tilt = Serial.parseInt();
        leftWheels = Serial.parseInt();
        rightWheels = Serial.parseInt();
        overdrive = Serial.parseInt();
        clientHash = Serial.parseInt();

        //compute hash
        myHash = gimble + tilt + leftWheels + rightWheels + overdrive;

        

        if(myHash == clientHash){
          sprintf(serResp, "%d,%d,%d,%d,%d,%d", gimble, tilt, leftWheels, rightWheels, overdrive, clientHash);
          Serial.println(serResp);
          spinWheel();
        }else{
          sprintf(serResp, "%d,%d,%d,%d,%d,%d!!!!!!!", gimble, tilt, leftWheels, rightWheels, overdrive, clientHash);
          Serial.println(serResp);
        }
      }
}
void spinWheel(){
   if(overdrive){
      wheel[0].write(90 + leftWheels);
      wheel[1].write(90 + leftWheels);
      wheel[2].write(90 + leftWheels);
      wheel[3].write(90 + rightWheels);
      wheel[4].write(90 + rightWheels);
      wheel[5].write(90 + rightWheels);
   }
    else{
      wheel[0].write(90 + (leftWheels * 18/24));
      wheel[1].write(90 + (leftWheels * 18/24));
      wheel[2].write(90 + (leftWheels * 18/24));
      wheel[3].write(90 + (rightWheels * 18/24));
      wheel[4].write(90 + (rightWheels * 18/24));
      wheel[5].write(90 + (rightWheels * 18/24));
      
   }
}
/*
void spinWheel(){
  //for now just 1 wheel
  if(wheelMessage[OVERDRIVE]){
    if(wheelMessage[0] >= 0){
      wheel[0].write(90 + wheelMessage[0]);
    }else{
      wheel[0].write(74 + wheelMessage[0]);
    }
  }else{
    if(wheelMessage[0] >= 0){
      wheel[0].write(90 + (wheelMessage[0] * 18/24));
    }else{
      wheel[0].write(74 + (wheelMessage[0] * 18/24));
    }
  }
}
*/

