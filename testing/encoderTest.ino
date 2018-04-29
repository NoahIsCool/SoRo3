void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int A = digitalRead(2);
  Serial.print("A: ");
  Serial.println(A);
  int B = digitalRead(3);
  Serial.print("B: ");
  Serial.println(B);
}
