#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "Varun";
const char* password = "Varun1234";

// MQTT Broker details - using local Mosquitto server
const char* mqtt_server = "167.71.238.68";  
const int mqtt_port = 1883;
const char* mqtt_topic = "NodeMCU/Sensor/Varun";
const char* client_id = "NodeMCU_CO_Sensor";  

// Pin configuration
const int gasSensorPin = A0;  // Analog pin for MQ-7
const int ledPin = D4;        // Built-in LED for status indication
const int buzzerPin = D3;     // Buzzer for alerts

// MQTT client setup
WiFiClient espClient;
PubSubClient client(espClient);

// Variables
unsigned long lastMsgTime = 0;
const long msgInterval = 2000;  // Send data every 2 seconds
float lastSensorValue = 0;
const float sensorThreshold = 50.0;  // Adjusted threshold for CO in PPM

// Heating cycle timing
unsigned long lastHeatCycleTime = 0;
bool isHighTemp = true;  // Start with high temperature (5V)
const int highTempDuration = 60000;  // 60 seconds
const int lowTempDuration = 90000;   // 90 seconds

// Function to establish WiFi connection
void setup_wifi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(ledPin, !digitalRead(ledPin));  // Blink LED while connecting
  }

  digitalWrite(ledPin, HIGH);
  Serial.println("\nWiFi connected successfully");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// Function to reconnect to MQTT broker
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    if (client.connect(client_id)) {
      Serial.println("connected");
      client.publish(mqtt_topic, "MQ-7 Gas Sensor Online");
      
      for (int i = 0; i < 3; i++) {
        digitalWrite(ledPin, LOW);
        delay(100);
        digitalWrite(ledPin, HIGH);
        delay(100);
      }
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      delay(2000);
    }
  }
}

// Function to read gas sensor and convert to CO PPM
float readGasSensor() {
  int sum = 0;
  int numReadings = 5;

  for (int i = 0; i < numReadings; i++) {
    sum += analogRead(gasSensorPin);
    delay(10);
  }

  int sensorValue = sum / numReadings;
  float voltage = sensorValue * (3.3 / 1023.0);

  // MQ-7 CO sensor PPM conversion
  float ratio = (3.3 - voltage) / voltage;
  float ppm = 100.0 * pow(ratio, -1.2);  // Adjust coefficients as per MQ-7 datasheet

  Serial.print("Sensor Raw: ");
  Serial.print(sensorValue);
  Serial.print(" | Voltage: ");
  Serial.print(voltage, 2);
  Serial.print("V | CO PPM: ");
  Serial.println(ppm, 2);

  return ppm;
}

// Alert function if CO levels are high
void checkGasAlert(float gasValue) {
  if (gasValue > sensorThreshold) {
    digitalWrite(ledPin, LOW);
    if (buzzerPin > 0) {
      tone(buzzerPin, 1000, 200);
    }
    Serial.println("WARNING: High CO level detected!");
  } else {
    digitalWrite(ledPin, HIGH);
  }
}

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
  
  if (buzzerPin > 0) {
    pinMode(buzzerPin, OUTPUT);
  }

  Serial.begin(115200);
  Serial.println("\nMQ-7 CO Sensor with MQTT");
  
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);

  Serial.println("Taking initial sensor reading...");
  lastSensorValue = readGasSensor();
  Serial.println("Setup complete - starting main loop");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();

  // Handle MQ-7 heating cycle
  if (now - lastHeatCycleTime >= (isHighTemp ? highTempDuration : lowTempDuration)) {
    isHighTemp = !isHighTemp;
    lastHeatCycleTime = now;
    Serial.println(isHighTemp ? "High Temp Phase (5V)" : "Low Temp Phase (1.4V)");
  }

  if (now - lastMsgTime > msgInterval) {
    lastMsgTime = now;
    float gasValue = readGasSensor();
    checkGasAlert(gasValue);

    if (abs(gasValue - lastSensorValue) > 5.0 || now % 10000 < msgInterval) {
      char gasValueStr[10];
      dtostrf(gasValue, 4, 2, gasValueStr);

      Serial.print("Publishing CO PPM: ");
      Serial.println(gasValueStr);

      digitalWrite(ledPin, LOW);
      boolean published = client.publish(mqtt_topic, gasValueStr);
      digitalWrite(ledPin, HIGH);

      if (published) {
        Serial.println("Published successfully");
      } else {
        Serial.println("Publish failed");
      }

      lastSensorValue = gasValue;
    }
  }

  delay(10);
}
