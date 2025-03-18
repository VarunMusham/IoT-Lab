#include <wiringPi.h>
#include <stdio.h>
#include <unistd.h>  // For sleep()

#define LED_PIN 7

int main(void) {
	if (wiringPiSetup()==-1){
		printf("Pin Initialization Failed");
		return 1;
	}
    // Set LED_PIN to be an output pin
    pinMode(LED_PIN, OUTPUT);

    // Blink the LED
    while (1) {
        digitalWrite(LED_PIN, HIGH);  // Turn LED on
        sleep(1);                     // Wait for 1 second
        digitalWrite(LED_PIN, LOW);   // Turn LED off
        sleep(1);                     // Wait for 1 second
    }

    return 0;
}
