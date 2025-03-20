#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

const char* ssid = "Varun";  
const char* password = "Varun1234";  

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 5*3600+30*60, 60000);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nConnected!");
  timeClient.begin();
}

void loop() {
  timeClient.update();
  Serial.println(timeClient.getFormattedTime());
  delay(1000);
}
