#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <MQTTClient.h>
#include <unistd.h>

//gcc -o subscribe subscribe.c -lpaho-mqtt3c

#define ADDRESS   "167.71.238.68"
#define CLIENTID  "RaspberryPi_Subscriber"
#define QOS       1

// Callback for message arrival
int messageArrived(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    printf("Received [%s]: %.*s\n", topicName, message->payloadlen, (char *)message->payload);
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

int main() {
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    MQTTClient_setCallbacks(client, NULL, NULL, messageArrived, NULL);
    MQTTClient_connect(client, &conn_opts);

    MQTTClient_subscribe(client, "CPU/temperature/Varun", QOS);
    MQTTClient_subscribe(client, "Sense/temperature/Varun", QOS);
    MQTTClient_subscribe(client, "Sense/humidity/Varun", QOS);

    while (1) sleep(1);  // Keep running

    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return 0;
}
