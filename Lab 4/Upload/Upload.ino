#include <ESP8266WiFi.h>
#include <DHT.h>

#define DHTPIN D2  // GPIO2 (D2) on NodeMCU
#define DHTTYPE DHT11 // Use DHT22 if required
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "Varun";  
const char* password = "Varun1234";
const char* server = "api.thingspeak.com";
const String apiKey = "UXKAE2L14Y8T5HQT";

WiFiClient client;

void setup() {
    Serial.begin(115200);
    delay(10);
    WiFi.begin(ssid, password);

    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");

    dht.begin();
}

void loop() {
    float temp = dht.readTemperature();  // Read temperature (Celsius)
    float hum = dht.readHumidity();  // Read humidity

    if (isnan(temp) || isnan(hum)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }

    Serial.print("Temperature: "); Serial.print(temp);
    Serial.print(" °C, Humidity: "); Serial.print(hum); Serial.println(" %");

    if (client.connect(server, 80)) {
        String url = "/update?api_key=" + apiKey + "&field1=" + String(temp) + "&field2=" + String(hum);
        client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                     "Host: " + server + "\r\n" +
                     "Connection: close\r\n\r\n");
        delay(1000);
    }
    client.stop();
    delay(5000);  // Upload data every 15 seconds
}
