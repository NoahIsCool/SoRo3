#include <Wire.h>

#include <Servo.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
// Read Data from Grove - Multichannel Gas Sensor
#include <Wire.h>
#include "MutichannelGasSensor.h"
//Temperature and Moisture Sensor libs
#include <dht11.h>

//#include <Adafruit_MotorShield.h>


//Adafruit_MotorShield afms = Adafruit_MotorShield();
//Adafruit_DCMotor *motor = afms.getMotor(3);
//Adafruit_DCMotor *fan = afms.getMotor(4);
#define FAN_PIN 10
#define ACTUATOR_PIN_A 3
#define ACTUATOR_PIN_B 5
#define DRILL_PIN 12
dht11 DHT11;
#define DHT11PIN 2

float gasData[8];
float TandMData[7];
int flag = -127;


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
Servo myservo;
Servo drill;
int drillSpeed = 0;


void setup() {
  // Start gas sensor
  gas.begin(0x04);//the default I2C address of the slave is 0x04
  gas.powerOn();
  delay(1000);
  myservo.attach(FAN_PIN);
  Serial.begin(9600);
  Serial.setTimeout(1000);
  Serial.println("NH3,CO,NO2,C3H8,C4H10,CH4,H2,C2H5OH,Flag,Humidity,Tempature( C ),Fahrenheit,Kelvin,Dew Point,Dew Point,Hash");
  drill.attach(DRILL_PIN);

 
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
          fanSpeed = uint8_t(incomingByte);
          break;
        case 6: // hash (last byte recieved in transmission)
          serialHash = incomingByte;
          inTransmission = false;
          //compute hash
          myHash = (drillStuff[0] + drillStuff[1] + drillStuff[2] + drillStuff[3] + fanSpeed) /5;
          // read data everytime regarless of hash mismatch 
          readGasSensor();
          readTandMSensor();
          
          for(int i = 0; i < 8; i++){
            Serial.print(gasData[i]);
            Serial.print(",");
          }
          for(int i = 0; i < 7; i++){
            Serial.print(TandMData[i]);
            Serial.print(",");
          }
          
          // check hash and actually move things 
          if (serialHash == myHash ) {
            moveDrill();
            spinOfDeath(); 
            spinFan();
          } else {
            Serial.print("Data Corrupted");
          }
          Serial.println();
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
    drill.write(92);
    return;
  }
  if (drillStuff[3]) {//overDrive is off (remember c++ is semi boolean) this is to protect motors
    drill.write(90 + val);
    //afms.setPWM(1, (val * 2048/255) + 2045 );
  }
  else {
    if (drillStuff[2] < 0) {
      drill.write(84 + (val * 18 / 24));
      //afms.setPWM(1, (2045 - (val * 2048/255)));
    } else {
      drill.write(90 + (val * 18 / 24));
    }
  }
}

void readTandMSensor()
{
  // Flag,Humidity,Tempature( C ),Fahrenheit,Kelvin,Dew Point,Dew Point  ),
  int chk = DHT11.read(DHT11PIN);
  switch (chk)
  {
    case DHTLIB_OK:
      flag = -127;
      break;
    case DHTLIB_ERROR_CHECKSUM:
      flag = -120;//"Checksum error");
      break;
    case DHTLIB_ERROR_TIMEOUT:
      flag = -119;// "Time out error");
      break;
    default:
      flag = -118;//"Unknown error");
      break;
  }
  TandMData[0] = flag;
  TandMData[1] = (float)DHT11.humidity;// Humidity (%)
  TandMData[2] = (float)DHT11.temperature; // Temperature (°C)
  TandMData[3] = Fahrenheit(DHT11.temperature);
  TandMData[4] = Kelvin(DHT11.temperature);
  TandMData[5] = dewPoint(DHT11.temperature, DHT11.humidity);// Dew Point (°C):
  TandMData[6] =  dewPointFast(DHT11.temperature, DHT11.humidity); // Dew PointFast (°C)??? TODO: use one or the other
}

void readGasSensor()
{
  // NH3,CO,NO2,C3H8,C4H10,CH4,H2,C2H5OH
  gasData[0] = gas.measure_NH3();
  gasData[1] = gas.measure_CO();
  gasData[2] = gas.measure_NO2();
  gasData[3] = gas.measure_C3H8();
  gasData[4] = gas.measure_C4H10();
  gasData[5] = gas.measure_CH4();
  gasData[6] = gas.measure_H2();
  gasData[7] = gas.measure_C2H5OH();
}

double Fahrenheit(double celsius)
{
  return 1.8 * celsius + 32;
}

double Kelvin(double celsius)
{
  return celsius + 273.15;
}

// dewPoint function NOAA
// reference (1) : http://wahiduddin.net/calc/density_algorithms.htm
// reference (2) : http://www.colorado.edu/geography/weather_station/Geog_site/about.htm
//
double dewPoint(double celsius, double humidity)
{
  // (1) Saturation Vapor Pressure = ESGG(T)
  double RATIO = 373.15 / (273.15 + celsius);
  double RHS = -7.90298 * (RATIO - 1);
  RHS += 5.02808 * log10(RATIO);
  RHS += -1.3816e-7 * (pow(10, (11.344 * (1 - 1 / RATIO ))) - 1) ;
  RHS += 8.1328e-3 * (pow(10, (-3.49149 * (RATIO - 1))) - 1) ;
  RHS += log10(1013.246);

  // factor -3 is to adjust units - Vapor Pressure SVP * humidity
  double VP = pow(10, RHS - 3) * humidity;

  // (2) DEWPOINT = F(Vapor Pressure)
  double T = log(VP / 0.61078); // temp var
  return (241.88 * T) / (17.558 - T);
}

// delta max = 0.6544 wrt dewPoint()
// 6.9 x faster than dewPoint()
// reference: http://en.wikipedia.org/wiki/Dew_point
double dewPointFast(double celsius, double humidity)
{
  double a = 17.271;
  double b = 237.7;
  double temp = (a * celsius) / (b + celsius) + log(humidity * 0.01);
  double Td = (b * temp) / (a - temp);
  return Td;
}

void spinFan() {
    myservo.write(90 + fanSpeed);
}

void moveDrill() {

  switch (drillStuff[0]) {
    case 0:
      analogWrite(ACTUATOR_PIN_A, drillStuff[1]);
      analogWrite(ACTUATOR_PIN_B, 0);
      break;
    case 1:
      analogWrite(ACTUATOR_PIN_A, 0);
      analogWrite(ACTUATOR_PIN_B, drillStuff[1]);
      break;
    case 2:
      analogWrite(ACTUATOR_PIN_A, 0);
      analogWrite(ACTUATOR_PIN_B, 0);
      break;
  }
}
