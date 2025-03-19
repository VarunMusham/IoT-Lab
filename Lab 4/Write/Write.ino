/* This arduino code is sending data to thingspeak server every 30 seconds.

Created By Embedotronics Technologies*/

#include "DHT.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <SPI.h>
#include <MFRC522.h>
#include "ThingSpeak.h"

#define DHTPIN D2

#define DHTTYPE DHT11

DHT dht(DHTPIN,DHTTYPE);


float humidityData;
float temperatureData;


const char* ssid = "Varun";// 
const char* password = "Varun1234";
//WiFiClient client;
char server[] = "api.thingspeak.com";   //eg: 192.168.0.222


WiFiClient client;    


void setup()
{
 Serial.begin(115200);
  delay(10);
  dht.begin();
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Start the server
//  server.begin();
  Serial.println("Server started");
  Serial.print(WiFi.localIP());
  delay(1000);
  Serial.println("connecting...");
 }
void loop()
{ 
  humidityData = dht.readHumidity();
  temperatureData = dht.readTemperature(); 
  Sending_To_thingspeak(); 
  delay(5000); // interval
  readData(1);
  delay(1000);
  readData(2);
  delay(1000)
 }

 void Sending_To_thingspeak()   //CONNECTING WITH MYSQL
 {
   if (client.connect(server, 80)) {
    Serial.println("connected");
    // Make a HTTP request:
    Serial.print("GET /update?api_key=8YY5OVLH8PCYYCOE&field1=0");
    client.print("GET /update?api_key=8YY5OVLH8PCYYCOE&field1=0");     //YOUR URL
    Serial.println(humidityData);
    client.print(humidityData);
    client.print("&field2=");
    Serial.println("&field2=");
    client.print(temperatureData);
    Serial.println(temperatureData);
    client.print(" ");      //SPACE BEFORE HTTP/1.1
    client.print("HTTP/1.1");
    client.println();
    client.println("Host: api.thingspeak.com");
    client.println("Connection: close");
    client.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
 }

 void readData(int fieldNo)
 {
  int count = ThingSpeak.readLongField(2849831, fieldNo, "1U8AYHTM1UB3UP0X");
  // Check the status of the read operation to see if it was successful
  int statusCode = 0;
  statusCode = ThingSpeak.getLastReadStatus();
  if (statusCode == 200)
  {
    Serial.println("Temparature: " + String(count));
  }
  else
  {
    Serial.println(F("Problem reading channel"));
  }
 }