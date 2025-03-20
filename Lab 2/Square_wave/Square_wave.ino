#define WAVE_PIN D5

void setup() {
  pinMode(WAVE_PIN, OUTPUT);
  Serial.begin(115200);
}

// Set the parameters accordingly
float freq=800;
float dutyCycle=0.75;


float cycleTime=10e5/freq;

void loop() {
  digitalWrite(WAVE_PIN, HIGH);
  delayMicroseconds(int(cycleTime*dutyCycle));  // 50% duty cycle for 300Hz
  digitalWrite(WAVE_PIN, LOW);
  delayMicroseconds(int(cycleTime*(1-dutyCycle)));
}
