#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "Varun";
const char* password = "Varun1234";

// MQTT Broker details - using local Mosquitto server
const char* mqtt_server = "167.71.238.68";  // Replace with your Raspberry Pi IP address
const int mqtt_port = 1883;
const char* mqtt_topic = "NodeMCU/Sensor/Varun";
const char* client_id = "NodeMCU_GasSensor";  // Client ID for MQTT connection

// Pin configuration
const int gasSensorPin = A0;  // Analog pin to connect the gas sensor (MQ series)
const int ledPin = D4;        // Built-in LED for status indication
const int buzzerPin = D3;     // Optional buzzer for alerts (if you have one)

// MQTT client setup
WiFiClient espClient;
PubSubClient client(espClient);

// Variables
unsigned long lastMsgTime = 0;
const long msgInterval = 2000;  // Send data every 2 seconds
float lastSensorValue = 0;
const float sensorThreshold = 800.0;  // Match the Raspberry Pi warning threshold

// Function to establish WiFi connection
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Wait for connection with indicator
  int dots = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(ledPin, dots % 2 == 0 ? LOW : HIGH);  // Blink LED while connecting
    dots++;
  }

  // Connected - show details
  digitalWrite(ledPin, HIGH);  // LED off (active low)
  Serial.println("");
  Serial.println("WiFi connected successfully");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// Function to reconnect to MQTT broker
void reconnect() {
  // Loop until we're reconnected
  int attempt = 0;
  while (!client.connected() && attempt < 5) {
    Serial.print("Attempting MQTT connection...");
    
    // Attempt to connect
    if (client.connect(client_id)) {
      Serial.println("connected");
      
      // Once connected, publish an announcement
      client.publish(mqtt_topic, "NodeMCU Gas Sensor Online");
      
      // Flash LED to indicate success
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
      
      // Flash LED differently to indicate failure
      for (int i = 0; i < 5; i++) {
        digitalWrite(ledPin, LOW);
        delay(50);
        digitalWrite(ledPin, HIGH);
        delay(50);
      }
      
      delay(2000);
      attempt++;
    }
  }
}

// Function to read the gas sensor and convert to PPM (calibrated for MQ sensor)
float readGasSensor() {
  // Take multiple readings for stability
  int sum = 0;
  int numReadings = 5;
  
  for (int i = 0; i < numReadings; i++) {
    sum += analogRead(gasSensorPin);
    delay(10);
  }
  
  int sensorValue = sum / numReadings;
  
  // Convert analog reading to voltage (0-3.3V for NodeMCU)
  float voltage = sensorValue * (3.3 / 1023.0);
  
  // Convert to PPM using calibration formula
  // Note: This is a general formula - you should calibrate for your specific MQ sensor
  // Different MQ sensors detect different gases and have different sensitivity curves
  
  // For example, for MQ-2 (LPG, Smoke):
  float ppm = 0;
  
  // Simple conversion method:
  ppm = map(sensorValue, 0, 1023, 0, 2000);  // Map raw values to 0-2000 ppm range
  
  // Alternatively, a more realistic formula for MQ sensors (uncomment and adjust for your sensor):
  // float ratio = (3.3 - voltage) / voltage;
  // ppm = 100.0 * pow(ratio, -1.53);  // Adjust these parameters based on your sensor datasheet
  
  Serial.print("Sensor Raw: ");
  Serial.print(sensorValue);
  Serial.print(" | Voltage: ");
  Serial.print(voltage, 2);
  Serial.print("V | PPM: ");
  Serial.println(ppm, 2);
  
  return ppm;
}

// Alert function if gas levels are high
void checkGasAlert(float gasValue) {
  if (gasValue > sensorThreshold) {
    // Warning condition
    digitalWrite(ledPin, LOW);  // Turn on LED
    
    if (buzzerPin > 0) {
      // If buzzer is connected
      tone(buzzerPin, 1000, 200);  // Beep at 1kHz for 200ms
    }
    
    Serial.println("WARNING: High gas level detected!");
  } else {
    digitalWrite(ledPin, HIGH);  // LED off (normal condition)
  }
}

void setup() {
  // Initialize pins
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);  // LED off initially (active low)
  
  if (buzzerPin > 0) {
    pinMode(buzzerPin, OUTPUT);
  }
  
  // Initialize serial communication
  Serial.begin(115200);
  Serial.println("\n\nMQ Gas Sensor with MQTT");
  Serial.println("------------------------");
  
  // Connect to WiFi
  setup_wifi();
  
  // Configure MQTT server
  client.setServer(mqtt_server, mqtt_port);
  
  // Initial sensor reading
  Serial.println("Taking initial sensor reading...");
  lastSensorValue = readGasSensor();
  Serial.println("Setup complete - starting main loop");
}

void loop() {
  // Ensure MQTT connection
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Read and publish at regular intervals
  unsigned long now = millis();
  if (now - lastMsgTime > msgInterval) {
    lastMsgTime = now;
    
    // Read gas sensor
    float gasValue = readGasSensor();
    
    // Check if we should alert
    checkGasAlert(gasValue);
    
    // Only publish if value has changed significantly or 10 seconds have passed
    if (abs(gasValue - lastSensorValue) > 5.0 || now % 10000 < msgInterval) {
      // Convert to string with 2 decimal places
      char gasValueStr[10];
      dtostrf(gasValue, 4, 2, gasValueStr);
      
      // Publish to MQTT
      Serial.print("Publishing gas value: ");
      Serial.println(gasValueStr);
      
      // Blink LED briefly to indicate transmission
      digitalWrite(ledPin, LOW);
      
      // Publish and check success
      boolean published = client.publish(mqtt_topic, gasValueStr);
      
      if (published) {
        Serial.println("Published successfully");
      } else {
        Serial.println("Publish failed");
      }
      
      digitalWrite(ledPin, HIGH);
      
      // Update last value
      lastSensorValue = gasValue;
    }
  }
  
  // Small delay to prevent WDT reset
  delay(10);
}