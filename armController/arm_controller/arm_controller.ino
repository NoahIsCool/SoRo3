#include <Servo.h>

// pin mapping
/*
*/

const char DEVICE_ID = 1;

Servo base, shoulder, elbow, wristL, wristR, clawL, clawR;
Servo left1, left2, right1, right2;

char myHash = 90;
int base_pos = 90;
int shoulder_pos = 90;
int elbow_pos = 90;
int wristL_pos = 90;
int wristR_pos = 90;
int clawL_pos = 170;
int clawR_pos = 70;
int left1_pos = 0;
int left2_pos = 0;
int right1_pos = 0;
int right2_pos = 0;

char incomingByte;
int bytesRead = 0;
long timeDifference = 0;
long rps = 0;

char serResp[] = " 1,190,200!!!!!!!!!!!!!!!!!!!!!!!!!\n";
char serialHash;
bool inTransmission = false;

//setup wheels
void setup() {
  shoulder.attach(3);
  elbow.attach(4);
  wristL.attach(5);
  wristR.attach(6);
  clawL.attach(7);
  clawR.attach(8);
  left1.attach(9);
  left2.attach(10);
  right1.attach(11);
  right2.attach(12);

  moveWrist();
  
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

long computeRPS()
{
  int cycles = 0; // Interrupt value..
  long cyclesPerSecond = (cycles / timeDifference) * 1000;
  rps = cyclesPerSecond / (1,669.656);
}

// Move wrist back to hit button
void moveWrist()
{
  long startTime = millis();
  left1.write(10);
  left2.write(10);
  right1.write(10);
  right2.write(10);
  // When button is hit, stop
  long endTime = millis();
  timeDifference = endTime - startTime;
  rps = computeRPS();
}

// Given an array of integers, move to that position (one alpha, one phi)
void move(int pos[]) {
  /*base_pos = pos[0];
  shoulder_pos = pos[1];
  elbow_pos = pos[2];
  wristL_pos = pos[3];
  wristR_pos = pos[4];
  clawL_pos = pos[5];
  clawR_pos = pos[6];
  left1_pos = pos[7];
  left2_pos = pos[8];
  right1_pos = pos[9];
  right2_pos = pos[10];
  // Want to move wrist back to hit button
  // Then you have a relative position , so you know where to move
  // Use mills. like project 5 ticks??
  // Idea: Count millis and divide by cycles per revolution
  updateServos();
  */
}
void updateServos(){
  
      shoulder.write(shoulder_pos);
      elbow.write(elbow_pos);
      clawL.write(clawL_pos);
      clawR.write(clawR_pos);
  
}
