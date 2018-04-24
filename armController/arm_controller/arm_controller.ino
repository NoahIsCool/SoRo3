#include <Servo.h>

// pin mapping
/*
*/

const char DEVICE_ID = 1;

Servo shoulder, elbow, clawL, clawR;

char myHash = 90;
int shoulder_pos = 90;
int elbow_pos = 90;
int clawL_pos = 170;
int clawR_pos = 70;

char incomingByte;
int bytesRead = 0;

char serResp[] = " 1,190,200!!!!!!!!!!!!!!!!!!!!!!!!!\n";
char serialHash;
bool inTransmission = false;

//setup wheels
void setup() {
  shoulder.attach(3);
  elbow.attach(4);
  clawL.attach(5);
  clawR.attach(6);
  
  Serial.begin(9600);
  delay(10);
}

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
        case 1: // shoulder
          shoulder_pos = uint8_t(incomingByte);
          break;
        case 2: // elbow
          elbow_pos = uint8_t(incomingByte);
          break;
        case 3:
          clawL_pos = uint8_t(incomingByte);
          break;
        case 4:
          clawR_pos = uint8_t(incomingByte);
          break;
        case 5: // hash (last byte recieved in transmission)
          serialHash = incomingByte;
            inTransmission = false;
          myHash = (shoulder_pos + elbow_pos+clawL_pos+ clawR_pos)/4;
          if(myHash == serialHash)
          {
            sprintf(serResp, "%d\t%d\t%d\t%d", shoulder_pos, elbow_pos, clawL_pos, clawR_pos);
            Serial.println(serResp);
            updateServos();
          } else {
            sprintf(serResp, "%d\t%d\t%d\t%d!!!!!!!", shoulder_pos, elbow_pos, clawL_pos, clawR_pos);
            Serial.println(serResp);
          }
          break;
      }
      bytesRead++;
    }
  }
}

void updateServos(){
  
      shoulder.write(shoulder_pos);
      elbow.write(elbow_pos);
      clawL.write(clawL_pos);
      clawR.write(clawR_pos);
  
}
