#include <ESP8266WiFi.h> // Use <ESP8266WiFi.h> for ESP8266
#include <NTPClient.h>
#include <WiFiUdp.h>

const long utcOffsetInSeconds = 19800;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

const char* ssid = "Varun";
const char* password = "Varun1234";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to Wi-Fi.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  timeClient.begin();
}

void loop() {
  timeClient.update();
  Serial.print(timeClient.getHours());
  Serial.print(" : ");
  Serial.print(timeClient.getMinutes());
  Serial.print(" : ");
  Serial.println(timeClient.getSeconds());

  delay(1000);
}