#include <wiringPi.h>
#include <stdio.h>
#include <unistd.h>  // For sleep()

#define LED_PIN 1  // GPIO18  or PIN12

const int req=333;	// (1/300)*100000

int main(void) {
	if (wiringPiSetup()==-1){
		printf("Pin Initialization Failed");
		return 1;
	}
    // Set LED_PIN to be an output pin
    pinMode(LED_PIN, PWM_OUTPUT);
	
	pwmSetMode(PWM_MODE_MS);
	pwmSetClock(192);   // Base Frequency
	pwmSetRange(req);  
    // Blink the LED
    while (1) {
		pwmWrite(LED_PIN,req/2);
		usleep(1000000);
    }

    return 0;
}
