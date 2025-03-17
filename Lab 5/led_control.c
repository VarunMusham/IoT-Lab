#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <pigpio.h> 
#include <MQTTClient.h> 
 
#define ADDRESS "tcp://broker.hivemq.com:1883" 
#define CLIENTID "LedSubscriber" 
#define LED_TOPIC "LED/control/yourname" 
#define LED_PIN 4 
 
void messageArrived(void *context, char *topicName, int topicLen, 
MQTTClient_message *message) { 
    char payload[message->payloadlen + 1]; 
    memcpy(payload, message->payload, message->payloadlen); 
    payload[message->payloadlen] = '\0'; 
 
    if (strcmp(payload, "on") == 0) { 
        gpioWrite(LED_PIN, 1); 
        printf("LED Turned ON\n"); 
    } else if (strcmp(payload, "off") == 0) { 
        gpioWrite(LED_PIN, 0); 
        printf("LED Turned OFF\n"); 
    } 
 
    MQTTClient_freeMessage(&message); 
    MQTTClient_free(topicName); 
} 
 
int main() { 
    gpioInitialise(); 
    gpioSetMode(LED_PIN, PI_OUTPUT); 
 
    MQTTClient client; 
    MQTTClient_create(&client, ADDRESS, CLIENTID, 
MQTTCLIENT_PERSISTENCE_NONE, NULL); 
    MQTTClient_setCallbacks(client, NULL, NULL, messageArrived, NULL); 
    MQTTClient_connect(client, 
&MQTTClient_connectOptions_initializer); 
    MQTTClient_subscribe(client, LED_TOPIC, 1); 
 
    while (1) { 
        MQTTClient_yield(); 
    } 
}