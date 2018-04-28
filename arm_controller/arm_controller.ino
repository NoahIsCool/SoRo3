#include <Servo.h>

// pin mapping
/*
*/

const char DEVICE_ID = 1;

Servo base, shoulder, elbow, clawL, clawR;
// pin  3, 5, 6, 9, 10, and 11 are for analogWrite
const int wristL1 = 5, wirstL2 = 6, wristR1 = 9, wristR2 = 10;
const int ticksPerRev = 1669656;
const int wristTopSpeed = 127;// 255 max for analog write
const int lEncPin = 11;
const int rEncPin = 12;

char myHash = 90;
int elbow_pos = 90;
int clawL_pos = 170;
int clawR_pos = 70;
int shoulder_pos;
long lTicks = 0;
long rTicks = 0;
bool rForward = true;
bool lForward = true;

int wristTheta = 0;
int wristPhi = 0;


char incomingByte;
int bytesRead = 0;

char serResp[] = "1,190,200!!!!!!!!!!!!!!!!!!!!!!!!!\n";
char serialHash;
bool inTransmission = false;

//setup wheels
void setup() {
  
  shoulder.attach(3);
  elbow.attach(4);
  clawL.attach(7);
  clawR.attach(8);
  attachInterrupt(digitalPinToInterrupt(lEncPin), leftEncoder, RISING);
  attachInterrupt(digitalPinToInterrupt(rEncPin), rightEncoder, RISING);
  Serial.begin(9600);
  delay(10);
}

void loop() {
  if(Serial.available()>4) // trying to read from an empty buffer usually breaks stuff
  {/*
    
    incomingByte = Serial.read();
    if(incomingByte == -127)
    { 
      Serial.println("start flag");
      if(Serial.read() != DEVICE_ID)
      {
        Serial.println("incorrect id");
        // transmission is invalid, clear buffer and then reply with device id
        Serial.flush();
        Serial.write(-126);
        Serial.write(DEVICE_ID);
      }
      else{
        Serial.println("reading message");
        shoulder_pos = uint8_t(Serial.read());
        elbow_pos = uint8_t(Serial.read());
        clawL_pos = uint8_t(Serial.read());
        clawR_pos = uint8_t(Serial.read());
        serialHash = Serial.read();
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
      }
    }
  }
  else
  {    
    //Serial.println("no bytes to read");
    //Serial.read();// dispose packets untill the start flag(-127)
  }
    
    /**/
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
          wristTheta = uint8_t(incomingByte);
          break;
        case 4:
          wristPhi  = uint8_t(incomingByte);
          break;
        case 5:
          clawL_pos = uint8_t(incomingByte);
          break;
        case 6:
          clawR_pos = uint8_t(incomingByte);
          break;
        case 7: // hash (last byte recieved in transmission)
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
  }/**/
}

void updateServos(){
  
      shoulder.write(shoulder_pos);
      elbow.write(elbow_pos);
      int ticks = ((wristTheta  * ticksPerRev / 360.0) - lTicks)
      if( ticks > 50){
        analogWrite(wristL1, wristTopSpeed);
        analogWrite(wristL2, 0);
        lForward = true;
      }
      else if(ticks < -50){
        analogWrite(wristL1, 0);
        analogWrite(wristL2, wristTopSpeed);
        lForward = false;
      }
      ticks = ((wristTheta  * ticksPerRev / 360.0) - rTicks)
      if( ticks > 50){
        analogWrite(wristR1, wristTopSpeed);
        analogWrite(wristR2, 0);
        rForward = true;
      }
      else if(ticks < -50){
        analogWrite(wristR1, 0);
        analogWrite(wristR2, wristTopSpeed);
        rForward = false;
      }
      clawL.write(clawL_pos);
      clawR.write(clawR_pos);
  
}

void leftEncoder(){
  lTicks++;
}
void rightEncoder(){
  rTicks++;
}

void calibrate(){
        

}

