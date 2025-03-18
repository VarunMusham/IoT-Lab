#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *fp;
    char buffer[50];

    // Run Python script to get temperature & humidity
    fp = popen("python3 -c 'from sense_emu import SenseHat; "
               "sense=SenseHat(); print(sense.get_temperature(), sense.get_humidity())'", "r");

    if (fp == NULL) {
        printf("Failed to get data from Sense HAT emulator\n");
        return 1;
    }

    fgets(buffer, sizeof(buffer), fp);
    pclose(fp);

    float temp, humidity;
    sscanf(buffer, "%f %f", &temp, &humidity);

    printf("Emulated Sense HAT Temperature: %.2f°C\n", temp);
    printf("Emulated Sense HAT Humidity: %.2f%%\n", humidity);

    return 0;
}
