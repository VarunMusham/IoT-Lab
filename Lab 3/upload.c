#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <unistd.h>  // For sleep()
#define API_KEY "UXKAE2L14Y8T5HQT"  // Replace with your ThingSpeak Write API Key

// Function to get CPU temperature
float getCPUTemperature() {
    FILE *fp;
    char buffer[10];
    float temp;

    fp = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    if (fp == NULL) {
        perror("Error opening temperature file");
        return -1;
    }

    fgets(buffer, sizeof(buffer), fp);
    fclose(fp);

    temp = atof(buffer) / 1000.0;  // Convert millidegrees to degrees Celsius
    return temp;
}

// Function to upload temperature to ThingSpeak
void uploadTemperature(float temperature) {
    CURL *curl;
    CURLcode res;
    char url[256];

    snprintf(url, sizeof(url), "https://api.thingspeak.com/update?api_key=%s&field1=%.2f", API_KEY, temperature);

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}

int main() {
	while(1)
	{
    float temp = getCPUTemperature();
    if (temp != -1) {
        printf("Uploading CPU Temperature: %.2f°C\n", temp);
        uploadTemperature(temp);
    }
    usleep(5000000);
	}
    return 0;
}
