#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mosquitto.h>
#include <unistd.h>
#include <signal.h>

#define MQTT_HOST "localhost"  // Using local Mosquitto server
#define MQTT_PORT 1883
#define MQTT_TOPIC "NodeMCU/Sensor/"

bool running = true;

// Signal handler for clean termination
void handle_signal(int s) {
    running = false;
}

// Callback when connection is established
void on_connect(struct mosquitto *mosq, void *obj, int reason_code) {
    printf("Connected to MQTT broker with code %d\n", reason_code);
    
    if (reason_code != 0) {
        /* Connection failed, check reason_code */
        mosquitto_disconnect(mosq);
        return;
    }
    
    // Subscribe to topic
    mosquitto_subscribe(mosq, NULL, MQTT_TOPIC, 0);
    printf("Subscribed to topic: %s\n", MQTT_TOPIC);
}

// Callback when a message is received
void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message) {
    // Ensure the message is not NULL
    if (message->payload != NULL) {
        printf("Received message on topic %s: %s\n", 
               message->topic, (char *)message->payload);
        
        // Convert message payload to float (gas sensor value)
        float gasValue = atof((char *)message->payload);
        
        // Process the gas value (example: check if it exceeds a threshold)
        if (gasValue > 1000.0) {
            printf("WARNING: Gas level is high! Value: %.2f ppm\n", gasValue);
            // You could trigger an alarm or other action here
        } else {
            printf("Gas level normal: %.2f ppm\n", gasValue);
        }
    }
}

// Callback for disconnection
void on_disconnect(struct mosquitto *mosq, void *obj, int reason_code) {
    printf("Disconnected from broker with code %d\n", reason_code);
}

int main() {
    struct mosquitto *mosq;
    int rc;
    
    // Set up signal handling
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    
    // Initialize the mosquitto library
    mosquitto_lib_init();
    
    // Create a new mosquitto client instance
    mosq = mosquitto_new(NULL, true, NULL);
    if (!mosq) {
        fprintf(stderr, "Error: Out of memory.\n");
        return 1;
    }
    
    // Set callback functions
    mosquitto_connect_callback_set(mosq, on_connect);
    mosquitto_message_callback_set(mosq, on_message);
    mosquitto_disconnect_callback_set(mosq, on_disconnect);
    
    // Connect to MQTT broker
    rc = mosquitto_connect(mosq, MQTT_HOST, MQTT_PORT, 60);
    if (rc != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Unable to connect to broker: %s\n", mosquitto_strerror(rc));
        mosquitto_destroy(mosq);
        mosquitto_lib_cleanup();
        return 1;
    }
    
    // Start the mosquitto network loop
    rc = mosquitto_loop_start(mosq);
    if (rc != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Error starting loop: %s\n", mosquitto_strerror(rc));
        mosquitto_destroy(mosq);
        mosquitto_lib_cleanup();
        return 1;
    }
    
    printf("Gas Sensor MQTT Subscriber\n");
    printf("Waiting for messages on %s...\n", MQTT_TOPIC);
    printf("Press Ctrl+C to exit\n");
    
    // Keep the program running until signaled to stop
    while (running) {
        sleep(1);
    }
    
    // Clean up
    mosquitto_loop_stop(mosq, true);
    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    
    printf("\nSubscriber terminated cleanly\n");
    return 0;
}