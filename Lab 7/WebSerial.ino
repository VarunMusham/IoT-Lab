#include <WebSerial.h>

#define LED 2

AsyncWebServer server(80);

const char* ssid = "Varun";        
const char* password = "Varun1234";

void recvMsg(uint8_t *data, size_t len){
  WebSerial.println("Received Data...");
  String receivedData = "";
  
  for(int i = 0; i < len; i++){
    receivedData += char(data[i]);
  }

  WebSerial.println("Received Message: " + receivedData);

  if (receivedData == "ON"){
    digitalWrite(LED, LOW);
    WebSerial.println("LED turned ON");
  }
  else if (receivedData == "OFF"){
    digitalWrite(LED, HIGH);
    WebSerial.println("LED turned OFF");
  }
  else {
    WebSerial.println("Unknown command");
  }
}

int attempt=0;
void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
        attempt++;
        
        if (attempt > 20) {
            Serial.println("\nRestarting ESP due to WiFi failure...");
            ESP.restart();
        }
    }
  
  Serial.println("WiFi connected!");
  Serial.println("IP Address: ");
  Serial.println(WiFi.localIP());
  
  WebSerial.begin(&server);
  WebSerial.onMessage(recvMsg);
  
  server.begin();
  delay(2000);
  WebSerial.println("Connected to Board");
}

void loop() {
}
