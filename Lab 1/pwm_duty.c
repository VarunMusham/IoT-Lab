
#include <wiringPi.h>
#include <stdio.h>
#include <unistd.h>

#define GPIO_PIN 1  // WiringPi Pin 1 (BCM GPIO 18)

int main() {
    if (wiringPiSetup() == -1) {
        printf("Setup failed!\n");
        return 1;
    }

    pinMode(GPIO_PIN, PWM_OUTPUT);

    // Set PWM frequency: 300 Hz
    pwmSetMode(PWM_MODE_MS);
    pwmSetClock(100);    
    pwmSetRange(2500);    // Change here for Freq  80Hz=((1/2500)*2)*100000

    while (1) {
        pwmWrite(GPIO_PIN, 2500/4);  // 50% duty cycle (High for half the time)
        //usleep(1000000);           // Wait 1 sec (optional)
    }

    return 0;
}
