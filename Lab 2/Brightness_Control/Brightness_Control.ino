#define LED_PIN D5

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  for (int i = 0; i <= 1023; i++) {
    analogWrite(LED_PIN, i);
    Serial.print("i = ");
    Serial.println(i);
    delay(10);
  }
  for (int i = 1023; i >= 0; i--) {
    analogWrite(LED_PIN, i);
    Serial.print("i = ");
    Serial.println(i);
    delay(10);
  }
}
