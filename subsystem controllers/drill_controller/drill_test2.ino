#include <Wire.h>

#include <Servo.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <Adafruit_MotorShield.h>

Servo myservo;
Adafruit_MotorShield afms = Adafruit_MotorShield();
Adafruit_DCMotor *motor = afms.getMotor(3);
Adafruit_DCMotor *fan = afms.getMotor(2);


const char DEVICE_ID = 2;

int drillStuff[] = { 2, 0, 0 };
int fanSpeed = 0;
int clientHash = 0;
int myHash = 0;
// Need MAC and IP address of the Arduino
byte mac[] = {0xAC, 0xDC, 0x0D, 0xAD, 0x00, 0x00};
unsigned int localPort = 21;
//EthernetServer me = EthernetServer(localPort);

const int packetSize = 9;
char packetBuffer[packetSize];

char incomingByte;
int bytesRead = 0;

char serResp[] = "1,190,200!!!!!!!!!!!!!!!!!!!!!!!!!\n";
char serialHash;
bool inTransmission = false;

Servo drill;
int drillSpeed = 0;


void setup() {
  myservo.attach(8);
  Serial.begin(9600);
  Serial.setTimeout(1000);
  afms.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
  //initialize the network sheild
  //Ethernet.begin(mac);
  //start listening for a connection
  //me.begin();

  drill.attach(0);

  motor->setSpeed(150);
  motor->run(FORWARD);
  // turn on motor???
  motor->run(RELEASE);
}

void loop() {
  if (Serial.available() > 7) // trying to read from an empty buffer usually breaks stuff
  {/**/
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
            Serial.write(incomingByte);
            inTransmission = false;
          }/**/
          break;
        case 1:
          drillStuff[0] = uint8_t(incomingByte);
          break;
        case 2: // shoulder
          drillStuff[1] = uint8_t(incomingByte);
          break;
        case 3: // elbow
          drillStuff[2] = int8_t(incomingByte);
          break;
        case 4:
          drillStuff[3] = int8_t(incomingByte);// converts from int to float. change the transmission
          break;
        case 5:
          fanSpeed = incomingByte;
          break;
        case 6: // hash (last byte recieved in transmission)
          serialHash = incomingByte;
          inTransmission = false;
          //compute hash
          myHash = (drillStuff[0] + drillStuff[1] + drillStuff[2] + drillStuff[3] + fanSpeed) /5;

          if (serialHash == myHash ) {
            sprintf(serResp, "%d,%d,%d,%d,%d,%d", drillStuff[0], drillStuff[1], drillStuff[2], drillStuff[3],fanSpeed, myHash);
            Serial.println(serResp);
            moveDrill();
            spinOfDeath(); 
            spinFan();
          } else {
            sprintf(serResp, "%d,%d,%d,%d,%d,%d!!!!!!!", drillStuff[0], drillStuff[1], drillStuff[2], drillStuff[3], fanSpeed, myHash);
            Serial.println(serResp);
          }
          break;
      }
      bytesRead++;
    }
  }

}

void stow()
{
  // Move all servos to stowed positions
}

void spinOfDeath() {
  // Spin the drill
  int val = drillStuff[2];
  if (drillStuff[2] == 0 ) {
    myservo.write(92);
    return;
  }
  if (drillStuff[3]) {//overDrive is off (remember c++ is semi boolean) this is to protect motors
    myservo.write(90 + val);
    //afms.setPWM(1, (val * 2048/255) + 2045 );
  }
  else {
    if (drillStuff[2] < 0) {
      myservo.write(84 + (val * 18 / 24));
      //afms.setPWM(1, (2045 - (val * 2048/255)));
    } else {
      myservo.write(90 + (val * 18 / 24));
    }
  }
}

void spinFan() {
  fan->setSpeed(fanSpeed);
  if ( fan <= 0)
    fan->run(RELEASE);
  else
    fan->run(FORWARD);

}

void moveDrill() {
  motor->setSpeed(drillStuff[1]);

  switch (drillStuff[0]) {
    case 0:
      motor->run(FORWARD);
      break;
    case 1:
      motor->run(BACKWARD);
      break;
    case 2:
      motor->run(RELEASE);
      break;
  }
}
