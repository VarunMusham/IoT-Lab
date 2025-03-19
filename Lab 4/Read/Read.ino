#include "ESP8266WiFi.h"
#include "ThingSpeak.h"
char ssid[] = "Varun";  // your network SSID (name)
char pass[] = "Varun1234"; // your network password
#include <WiFiClient.h>
WiFiClient client; 

// #include "SoftwareSerial.h"
// SoftwareSerial Serial1(5, 6); // RX, TX
// Channel details
unsigned long myChannelNumber = 2849831; //your public channel number
const char * myReadAPIKey = "1U8AYHTM1UB3UP0X"; //read API
unsigned int FieldToMonitor = 1; //the field you want to monitor
long count;

void setup()
{

 Serial.begin(115200);
  delay(10);
  // dht.begin();
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, pass);
 
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
  ThingSpeak.begin(client);  // Initialize ThingSpeak

}

void loop()
{

  // Connect or reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(F("Attempting to connect to SSID: "));
    Serial.println(ssid);
    while (WiFi.status() != WL_CONNECTED)
    {
      WiFi.begin(ssid, pass);
      Serial.print(".");
      delay(5000);
    }
    Serial.println(F("\nConnected"));
  }


  //read from channel
  count = ThingSpeak.readLongField(myChannelNumber, 1, myReadAPIKey);
  // Check the status of the read operation to see if it was successful
  int statusCode = 0;
  statusCode = ThingSpeak.getLastReadStatus();
  if (statusCode == 200)
  {
    Serial.println("Humidity: " + String(count));
  }
  else
  {
    Serial.println(F("Problem reading channel"));
  }


  count = ThingSpeak.readLongField(myChannelNumber, 2, myReadAPIKey);
  // Check the status of the read operation to see if it was successful
   statusCode = 0;
  statusCode = ThingSpeak.getLastReadStatus();
  if (statusCode == 200)
  {
    Serial.println("Temparature: " + String(count));
  }
  else
  {
    Serial.println(F("Problem reading channel"));
  }

  delay(15000); // No need to read the counter too often.

}