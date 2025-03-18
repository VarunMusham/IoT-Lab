#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <MQTTClient.h>

//gcc -o publish publish.c -lpaho-mqtt3c

#define ADDRESS   "167.71.238.68"  // Free Broker
#define CLIENTID  "RaspberryPi_Publisher"
#define QOS       1
#define TIMEOUT   10000
#define NAME      "Varun"  // Replace with your name

// Function to get CPU temperature
float getCPUTemperature() {
    FILE *fp;
    char buffer[10];
    float temp;
    fp = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    if (fp == NULL) return -1;
    fgets(buffer, sizeof(buffer), fp);
    fclose(fp);
    temp = atof(buffer) / 1000.0;
    return temp;
}

void senseHat(float* temperature, float* humidity)
{
    FILE *fp;
    char buffer[50];

    // Run Python script to get temperature & humidity
    fp = popen("python3 -c 'from sense_emu import SenseHat; "
               "sense=SenseHat(); print(sense.get_temperature(), sense.get_humidity())'", "r");

    if (fp == NULL) {
        printf("Failed to get data from Sense HAT emulator\n");
        return ;
    }

    fgets(buffer, sizeof(buffer), fp);
    pclose(fp);

    // float temp, humidity;
    sscanf(buffer, "%f %f", temperature, humidity);

    printf("Emulated Sense HAT Temperature: %.2f°C\n", temperature);
    printf("Emulated Sense HAT Humidity: %.2f%%\n", humidity);
}

// Function to publish MQTT messages
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
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    MQTTClient_connect(client, &conn_opts);

    while (1) {
        float cpu_temp = getCPUTemperature();
        // float sense_temp = 25.5;  // Dummy Sense HAT temp
        // float sense_humidity = 60.0;  // Dummy Sense HAT humidity
        float sense_temp,sense_humidity;
        senseHat(&sense_temp,&sense_humidity);
        char payload[50];

        snprintf(payload, sizeof(payload), "%.2f", cpu_temp);
        publish(client, "CPU/temperature/" NAME, payload);

        snprintf(payload, sizeof(payload), "%.2f", sense_temp);
        publish(client, "Sense/temperature/" NAME, payload);

        snprintf(payload, sizeof(payload), "%.2f", sense_humidity);
        publish(client, "Sense/humidity/" NAME, payload);

        printf("Published: CPU Temp=%.2f, Sense Temp=%.2f, Humidity=%.2f\n", cpu_temp, sense_temp, sense_humidity);

        sleep(5);  // Publish every 5 seconds
    }

    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return 0;
}
