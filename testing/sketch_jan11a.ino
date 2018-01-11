#import <Servo.h>
Servo drill;
void setup() 
{
  // put your setup code here, to run once:
  pinMode(2, OUTPUT);
  /*drill.attach(2);
  drill.write(10);
  delay(5000);
  drill.write(0);*/
  
//  int upTime = 1000;
//  int cycle = 6000;
//  for (int i = 36000; i > 0; i--)
//  {
//    digitalWrite(2, HIGH);
//    delayMicroseconds(upTime);
//    digitalWrite(2, LOW);
//    delayMicroseconds(cycle - upTime);
//  }
//  digitalWrite(2, LOW);
}


int upTime= 1450;
int cycle = 3000;

void loop()
{
  for (int i = -450; i < 450; i++)
  {
    for(int j = 0; j<3; j++){
      digitalWrite(2, HIGH);
      delayMicroseconds(upTime-i);
      digitalWrite(2, LOW);
      delayMicroseconds((cycle - upTime)+i);
  }}
  digitalWrite(2, LOW);
  delay(500000);
  
//  for (int i = 0; i < 1400; i++)
//  {
//    digitalWrite(2, HIGH);
//    delayMicroseconds(-(upTime - 1400) - i);
//    digitalWrite(2, LOW);
//    delayMicroseconds((cycle - upTime + 1400) + i);
//  }
//  delay(500000);
}
