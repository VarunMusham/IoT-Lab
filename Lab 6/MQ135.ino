#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi credentials (Personal hotspot credentials)
const char* ssid = "Your_Hotspot_SSID"; // Change this to your hotspot SSID
const char* password = "Your_Hotspot_Password"; // Change this to your hotspot password

// MQTT Broker details
const char* mqtt_server = "167.71.238.68"; // Replace with your MQTT Broker IP
const int mqtt_port = 1883; // MQTT port (default 1883)
const char* mqtt_topic_raw = "SNU/MQ135/raw"; // Topic for raw data
const char* mqtt_topic_ppm = "SNU/MQ135/ppm"; // Topic for CO2 concentration in PPM
const char* client_id = "NodeMCU_MQ135_Sensor"; // MQTT client ID

// Pin configuration
const int gasSensorPin = A0; // Analog pin for MQ-135

// MQTT client setup
WiFiClient espClient;
PubSubClient client(espClient);

// Variables
unsigned long lastMsgTime = 0;
const long msgInterval = 2000; // Send data every 2 seconds
float lastSensorValue = 0;

// Function to establish WiFi connection
void setup_wifi() {
Serial.print("Connecting to WiFi: ");
Serial.println(ssid);

WiFi.mode(WIFI_STA);
WiFi.begin(ssid, password);

while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}

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
  client.publish(mqtt_topic_raw, "MQ-135 Sensor Online");
} else {
  Serial.print("failed, rc=");
  Serial.print(client.state());
  Serial.println(" try again in 2 seconds");
  delay(2000);
}
}
}

// Function to read gas sensor and convert to CO2 concentration (PPM)
float readGasSensor() {
int sum = 0;
int numReadings = 5;

for (int i = 0; i < numReadings; i++) {
sum += analogRead(gasSensorPin);
delay(10);
}

int sensorValue = sum / numReadings;
float voltage = sensorValue * (3.3 / 1023.0);

// MQ-135 CO2 sensor PPM conversion (calibration for clean air)
float ratio = (3.3 - voltage) / voltage;
float ppm = 116.6020682 * pow(ratio, -2.769); // MQ-135 conversion for CO2

Serial.print("Sensor Raw: ");
Serial.print(sensorValue);
Serial.print(" | Voltage: ");
Serial.print(voltage, 2);
Serial.print("V | CO2 PPM: ");
Serial.println(ppm, 2);

return ppm;
}

void setup() {
pinMode(gasSensorPin, INPUT);
Serial.begin(115200);
Serial.println("\nMQ-135 CO2 Sensor with MQTT");

setup_wifi();
client.setServer(mqtt_server, mqtt_port);

Serial.println("Setup complete - starting main loop");
}

void loop() {
if (!client.connected()) {
reconnect();
}
client.loop();

unsigned long now = millis();

if (now - lastMsgTime > msgInterval) {
lastMsgTime = now;

// Read the gas sensor data
float gasValue = readGasSensor();

// Publish raw sensor data
char rawDataStr[10];
itoa(gasValue, rawDataStr, 10);
client.publish(mqtt_topic_raw, rawDataStr);

// Publish CO2 concentration (PPM)
char ppmStr[10];
dtostrf(gasValue, 4, 2, ppmStr);
client.publish(mqtt_topic_ppm, ppmStr);

Serial.print("Publishing Raw Data: ");
Serial.println(rawDataStr);

Serial.print("Publishing CO2 PPM: ");
Serial.println(ppmStr);
}

delay(10);
}