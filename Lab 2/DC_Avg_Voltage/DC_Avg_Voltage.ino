#define PWM_PIN D5

float out_volt=1.6;

void setup() {
  pinMode(PWM_PIN, OUTPUT);
}

void loop() {
  analogWrite(PWM_PIN, (out_volt / 3.3) * 255);  // Convert 2V to PWM scale (0-255)
}
