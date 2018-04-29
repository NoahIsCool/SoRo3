#include <Servo.h>


/*
*/

const char DEVICE_ID = 1;

// pin mapping
Servo base, shoulder, elbow, clawL, clawR;
// pin  3, 5, 6, 9, 10, and 11 are for analogWrite
const int wristL1 = 5, wirstL2 = 6, wristR1 = 9, wristR2 = 10;
const int lEncPin = 11;
const int rEncPin = 12;

// math constants
const float ticksPerRev = 1669656;
const float ticksRatio = 360.0 / ticksPerRev;
const int wristTopSpeed = 127;// 255 max for analog write
const int motorDeadZone = 1;

char myHash = 90;
int elbow_pos = 90;
float wristTheta_dest = 0;
float wristPhi_dest = 0;
int clawL_pos = 170;
int clawR_pos = 70;
int shoulder_pos;
long lTicks = 0; // ticks since last update
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
    if (Serial.available() > 4) // trying to read from an empty buffer usually breaks stuff
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
        if (incomingByte == -127 && !inTransmission) {
            inTransmission = true;
            bytesRead = 0;
        } else if (inTransmission) {
            switch (bytesRead) {
                case 0: // device id
                    if (incomingByte != DEVICE_ID) {
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
                    wristTheta_dest = uint8_t(incomingByte);// converts from int to float. change the transmission
                    break;
                case 4:
                    wristPhi_dest = uint8_t(incomingByte);// TODO change the transmission to be a float for precision
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
                    myHash = (shoulder_pos + elbow_pos + wristTheta_dest + wristPhi_dest + clawL_pos + clawR_pos) / 4;
                    if (myHash == serialHash) {
                        sprintf(serResp, "%d\t%d\t%d\t%d\t%d\t%d", shoulder_pos, elbow_pos, wristTheta_dest,
                                wristPhi_dest, clawL_pos, clawR_pos);
                        Serial.println(serResp);
                        updateServos();
                    } else {
                        sprintf(serResp, "%d\t%d\t%d\t%d\t%d\t%d!!!!!!!", shoulder_pos, elbow_pos, wristTheta_dest,
                                wristPhi_dest, clawL_pos, clawR_pos);
                        Serial.println(serResp);
                    }
                    break;
            }
            bytesRead++;
        }
    }/**/
}

void updateServos() {

    shoulder.write(shoulder_pos);
    elbow.write(elbow_pos);
    clawL.write(clawL_pos);
    clawR.write(clawR_pos);
    // Right now we can't rotate and tilt ar the same time we'll need to fix this later
    if (wristPhi_dest - wristPhi > motorDeadZone) {
        // rotate right
        analogWrite(wristR1, wristTopSpeed);
        analogWrite(wristR2, 0);
        analogWrite(wristR1, 0);
        analogWrite(wristR2, wristTopSpeed);

        wristTheta += lTicks * ticksRatio;
        lTicks = 0;
        rTicks = 0;
        //here only one encoder value is used, I don't know what to do with the other. Error check with the other maybe?
    } else if (wristPhi_dest - wristPhi < -motorDeadZone) {
        //rotate left
        analogWrite(wristR1, 0);
        analogWrite(wristR2, wristTopSpeed);
        analogWrite(wristR1, wristTopSpeed);
        analogWrite(wristR2, 0);
        // if moving back it will subtract the chage int theta
        wristTheta -= lTicks * ticksRatio;
        lTicks = 0;
        rTicks = 0;
    } else { // if were not rotating we can
        // here, the value on the right is the margin of error
        if (wristTheta_dest - wristTheta > motorDeadZone) {
            // move forward
            analogWrite(wristR1, 0);
            analogWrite(wristR2, wristTopSpeed);
            analogWrite(wristR1, 0);
            analogWrite(wristR2, wristTopSpeed);
            // learn our new value and reset ticks
            //may be a problem if interruption happens right after here, this may cause a drift in the reference point.
            wristTheta += lTicks * ticksRatio;
            lTicks = 0;
            rTicks = 0;
            //here only one encoder value is used, I don't know what to do with the other. Error check with the other maybe?
        } else if (wristTheta_dest - wristTheta < -motorDeadZone) {
            // move back
            analogWrite(wristR1, wristTopSpeed);
            analogWrite(wristR2, 0);
            analogWrite(wristR1, wristTopSpeed);
            analogWrite(wristR2, 0);
            // if moving back it will subtract the chage int theta
            wristTheta -= lTicks * ticksRatio;
            lTicks = 0;
            rTicks = 0;
        } else {
            // stay still
            analogWrite(wristR1, 0);
            analogWrite(wristR2, 0);
            analogWrite(wristR1, 0);
            analogWrite(wristR2, 0);
            // don't do math here to avoid drift... i think?
        }
    }

}
/*
 * this interuptes the processor to count the number of pulses that come from the encoder.
 *
 * Warning this can happen as much as 1,669,656 times a second, and the arduino clock is only 16mhz
 */
void leftEncoder() {
    lTicks++;
}
/*
 * same as left encoder
 */
void rightEncoder() {
    rTicks++;
}

void calibrate() {


}

