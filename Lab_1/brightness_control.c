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

    // Blink the LED
    while (1) {
    for (int i=0;i<1024;i++){
        pwmWrite(LED_PIN,i);
	printf("Brightness = %d\n",i);
	usleep(50000);
    }
    
    for (int i=1023;i>=0;i--){
        pwmWrite(LED_PIN,i);
	printf("Brightness = %d\n",i);
	usleep(50000);
    }
 }

    return 0;
}
