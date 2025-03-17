#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <MQTTClient.h> 
#define ADDRESS "tcp://broker.hivemq.com:1883" 
#define CLIENTID "RaspiPublisher" 
#define QOS 1 
#define TIMEOUT 10000 
#define SENSE_TEMP_TOPIC "Sense/temperature/yourname" 
#define SENSE_HUMID_TOPIC "Sense/humidity/yourname" 
 
void get_sensehat_data(float *temperature, float *humidity) { 
    FILE *fp; 
    char buffer[50]; 
 
    fp = popen("python3 sensehat_reader.py", "r"); 
    if (fp == NULL) { 
        fprintf(stderr, "Error: Could not run Sense HAT script\n"); 
        *temperature = -1; 
        *humidity = -1; 
        return; 
    } 
 
    if (fgets(buffer, sizeof(buffer), fp) != NULL) { 
        sscanf(buffer, "%f,%f", temperature, humidity); 
    } 
 
    pclose(fp); 
} 
 
void publish(MQTTClient client, char *topic, char *payload) { 
    MQTTClient_message pubmsg = MQTTClient_message_initializer; 
    MQTTClient_deliveryToken token; 
 
    pubmsg.payload = payload; 
    pubmsg.payloadlen = strlen(payload); 
    pubmsg.qos = QOS; 
    pubmsg.retained = 0; 
 
    MQTTClient_publishMessage(client, topic, &pubmsg, &token); 
    MQTTClient_waitForCompletion(client, token, TIMEOUT); 
} 
 
int main() { 
    MQTTClient client; 
    MQTTClient_connectOptions conn_opts = 
MQTTClient_connectOptions_initializer; 
 
    MQTTClient_create(&client, ADDRESS, CLIENTID, 
MQTTCLIENT_PERSISTENCE_NONE, NULL); 
    MQTTClient_connect(client, &conn_opts); 
 
    while (1) { 
        float temp, humidity; 
        char temp_str[10], humid_str[10]; 
 
        get_sensehat_data(&temp, &humidity); 
 
        sprintf(temp_str, "%.2f", temp); 
        sprintf(humid_str, "%.2f", humidity); 
 
        publish(client, SENSE_TEMP_TOPIC, temp_str); 
        publish(client, SENSE_HUMID_TOPIC, humid_str); 
 
        sleep(5); 
    } 
 
    MQTTClient_disconnect(client, TIMEOUT); 
    MQTTClient_destroy(&client); 
    return 0; 
}