#include <Servo.h>

// pin mapping
/*
     [],pin  front   [],pin
     0,4     0-|-0   3,5
               |
     1,3     0-|-0   4,6
               |
     2,7     0-|-0   5,2

     gimbal pan: 8
     gimbal tilt: 9
*/

const char DEVICE_ID = 0;

Servo wheel[6];
Servo gimbal_pan, gimbal_tilt;

char myHash = 0;
char serialHash = 0;
char pan = 0;
char tilt = 0;
char leftWheels = 0;
char rightWheels = 0;
char overdrive = 0;

char incomingByte;
int bytesRead = 0;

char serResp[] = " 1,190,200!!!!!!!!!!!!!!!!!\n";

bool inTransmission = false;

//setup wheels
void setup() {
  wheel[0].attach(4);
  wheel[1].attach(3);
  wheel[2].attach(7);
  wheel[3].attach(5);
  wheel[4].attach(6);
  wheel[5].attach(2);
  
  gimbal_pan.attach(8);
  gimbal_tilt.attach(9);
  
  Serial.begin(9600);
  delay(10);
}

// serial transmission blueprint:

// [start transmission = -127 or 255] [device id] [overdrive] [left wheels]...
// ...[right wheels] [gimble tilt] [gimble pan] [hash]

// hash = (sum of data bytes--no start or id) / (num of bytes)

// all message bytes will be within the range [-90,90]

// device id #'s:
// drive    0
// arm      1
// science  2

void loop() {
  if(Serial.available()) // trying to read from an empty buffer usually breaks stuff
  {
    incomingByte = Serial.read();
    if(incomingByte == -127 && !inTransmission)
    {
      inTransmission = true;
      bytesRead = 0;
    }
    else if(inTransmission)
    {
      switch(bytesRead)
      {
        case 0: // device id
          if(incomingByte != DEVICE_ID)
          {
            // transmission is invalid, clear buffer and then reply with device id
            Serial.flush();
            Serial.write(-126);
            Serial.write(DEVICE_ID);
            inTransmission = false;
          }
          break;
        case 1: // overdrive
          overdrive = incomingByte;
          break;
        case 2: // left wheels
          leftWheels = incomingByte;
          break;
        case 3: // right wheels
          rightWheels = incomingByte;
          break;
        case 4: // gimble tilt
          tilt = incomingByte;
          break;
        case 5: // gimble pan
          pan = incomingByte;
          break;
        case 6: // hash (last byte recieved in transmission)
          serialHash = incomingByte;
          inTransmission = false;
          myHash = (pan + tilt + leftWheels + rightWheels + overdrive)/5;
          if(myHash == serialHash)
          {
            sprintf(serResp, "%d\t%d\t%d\t%d\t%d\t%d", overdrive, leftWheels, rightWheels, tilt, pan, serialHash);
            Serial.println(serResp);
            updateServos();
          } else {
            sprintf(serResp, "%d\t%d\t%d\t%d\t%d\t%d\t%d!!!!!!!", -125, overdrive, leftWheels, rightWheels, tilt, pan, serialHash);
            Serial.println(serResp);
            
          }
          break;
      }
      bytesRead++;
    }
  }
}

void updateServos(){
   if(overdrive) {
      wheel[0].write(90 + leftWheels);
      wheel[1].write(90 + leftWheels);
      wheel[2].write(90 + leftWheels);
      wheel[3].write(90 - rightWheels);
      wheel[4].write(90 - rightWheels);
      wheel[5].write(90 + rightWheels);
   } else {
      wheel[0].write(90 + (leftWheels * 18/24));
      wheel[1].write(90 + (leftWheels * 18/24));
      wheel[2].write(90 + (leftWheels * 18/24));
      wheel[3].write(90 - (rightWheels * 18/24));
      wheel[4].write(90 - (rightWheels * 18/24));
      wheel[5].write(90 + (rightWheels * 18/24));  
   }
   gimbal_pan.write(90 + pan);
   gimbal_tilt.write(90 + tilt);
}
