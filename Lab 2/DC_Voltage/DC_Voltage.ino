const int pwmPin = D2;

void setup() {
  pinMode(pwmPin, OUTPUT);
}

void loop() {
  void check(Volt){
  analogWrite(pwmPin, (Volt/ 3.3) * 255); // (2V/5V) * 255
}
check(2);
}