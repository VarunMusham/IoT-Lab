#include <wiringPi.h>
#include <stdio.h>
#include <unistd.h>  // For sleep()

#define LED_PIN 1  // GPIO18  or PIN12

int main(void) {
	if (wiringPiSetup()==-1){
		printf("Pin Initialization Failed");
		return 1;
	}
    // Set LED_PIN to be an output pin
    pinMode(LED_PIN, PWM_OUTPUT);
	
	pwmSetMode(PWM_MODE_MS);
	pwmSetClock(192);   // Base Frequency
	pwmSetRange(6667);  // 1.5MHz / 6667 = 300Hz
    // Blink the LED
    while (1) {
		pwmWrite(LED_PIN,3333);
		usleep(1000000);
    }

    return 0;
}
