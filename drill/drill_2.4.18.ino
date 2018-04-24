#include <Wire.h>
#include <Servo.h>
#include <SPI.h>
#include <Adafruit_MotorShield.h>

Servo drill;
Adafruit_MotorShield afms = Adafruit_MotorShield();
Adafruit_DCMotor *actuator = afms.getMotor(1);

int drillStuff[] = { 2, 0, 0, 0 };
// drillStuff[0] = actuator direction (0 is forwards, 1 is backwards, 2 is still)
// drillStuff[1] = actuator speed (0 to 2)
// drillStuff[2] = drill speed (-90 to 90)
// drillStuff[3] = overdrive (on or off)

int clientHash = 0;
int myHash = 0;
char *serResp = " 1,190,200!!!!!!!!!!!!!!!!!\n";

int drillSpeed = 0;

int string_pot;

void setup() {
  drill.attach(9);
  Serial.begin(9600);
  Serial.setTimeout(1000);
  afms.begin();
  pinMode(A0, INPUT); // feedback potentiometer
  
  actuator->setSpeed(150);
  actuator->run(FORWARD);
  // turn on actuator???
  actuator->run(RELEASE);
}

void loop() {

      string_pot = analogRead(A0);
      
      if(Serial.read() == 2){
        drillStuff[0] = Serial.parseInt(); // actuator direction
        drillStuff[1] = Serial.parseInt(); // actuator speed
        drillStuff[2] = Serial.parseInt(); // drill speed
        drillStuff[3] = Serial.parseInt(); // overdrive
        clientHash = Serial.parseInt();

        //compute hash
        myHash = drillStuff[0] + drillStuff[1] + drillStuff[2] + drillStuff[3];

        

        if(Serial.read() == '\n'&& myHash == clientHash){
          sprintf(serResp, "%d,%d,%d,%d,%d,%d", drillStuff[0], drillStuff[1], drillStuff[2], drillStuff[3], myHash, string_pot);
          Serial.println(serResp);
          moveDrill();
          spinOfDeath();
        }else{
          sprintf(serResp, "%d,%d,%d,%d!!!!!!!", drillStuff[0], drillStuff[1], drillStuff[2], drillStuff[3], myHash);
          Serial.println(serResp);
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
 if (drillStuff[3]) {//overDrive is off (remember c++ is semi boolean) this is to protect motors
    drill.write(90 + val);
    //afms.setPWM(1, (val * 2048/255) + 2045 );
  }
  else {
    if (val > 0){
      drill.write(84 + (val * 18/24));
     //afms.setPWM(1, (2045 - (val * 2048/255)));
    }else{
      drill.write(90 + (val * 18/24));
    }
  }
}

void moveDrill(){
  actuator->setSpeed(drillStuff[1]);
  
  switch(drillStuff[0]){
    case 0:      
      actuator->run(FORWARD);
      break;
    case 1:
      actuator->run(BACKWARD);
      break;
    case 2:
      actuator->run(RELEASE);
      break;
  };
}
