#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>  // Install ArduinoJson library

const char* ssid = "Varun";
const char* password = "Varun1234";
const String channelID = "2882460";
const String readAPIKey = "L597BFDG60RVM205";

WiFiClient client;

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = "http://api.thingspeak.com/channels/" + channelID + "/feeds.json?api_key=" + readAPIKey + "&results=1";
        
        http.begin(client, url);
        int httpCode = http.GET();
        
        if (httpCode > 0) {
            String payload = http.getString();
            Serial.println("Raw Data: " + payload);  // Print full response

            // Parse JSON
            DynamicJsonDocument doc(1024);
            deserializeJson(doc, payload);

            float temp = doc["feeds"][0]["field1"];  // Extract field1 (temperature)
            float hum = doc["feeds"][0]["field2"];   // Extract field2 (humidity)

            Serial.print("Temperature: "); Serial.print(temp); Serial.println(" °C");
            Serial.print("Humidity: "); Serial.print(hum); Serial.println(" %");

        } else {
            Serial.println("Error fetching data");
        }
        
        http.end();
    }

    delay(15000);  // Fetch data every 15 seconds
}
