#include<Wire.h>
#include<math.h>

const char DEVICE_ID = 4;

const int MPU=0x68;
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
int8_t output_data[6];
char incomingByte;
int bytesRead = 0;
bool inTransmission = false;
bool sendData = false;

void setup()
{
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.begin(9600);
  delay(10);
}

void loop()
{
  if(sendData)
  {
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU,12,true);
  
    // OPTION 1: convert 16 bit word to 8 bits
    // accel x, accel y, accel z, gyro x, gyro y, gyro z (all multiplied by 2^-8)
    AcX=Wire.read()<<8|Wire.read();
    output_data[0] = AcX * pow(2, -8);
    AcY=Wire.read()<<8|Wire.read();
    output_data[1] = AcY * pow(2, -8);
    AcZ=Wire.read()<<8|Wire.read();
    output_data[2] = AcZ * pow(2, -8);
    GyX=Wire.read()<<8|Wire.read();
    output_data[3] = GyX * pow(2, -8);
    GyY=Wire.read()<<8|Wire.read();
    output_data[4] = GyY * pow(2, -8);
    GyZ=Wire.read()<<8|Wire.read();
    output_data[5] = GyZ * pow(2, -8);
  
    Serial.write(-127); // start transmission
    Serial.write(DEVICE_ID);
    for(int i = 0; i < 6; i++)
    {
      Serial.write(output_data[i]);
    }
    delay(50);
  }

  //serial control:
  // -127, device id (4), enable
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
        case 1: // send data
          inTransmission = false;
          if(incomingByte == 0) // enable/disable sending of data
            sendData = true;
          else
            sendData = false;
          break;
      }
      bytesRead++;
    }
  }
  
}
