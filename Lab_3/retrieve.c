#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>

#define READ_API_KEY "L597BFDG60RVM205"  // Replace with your ThingSpeak Read API Key
#define CHANNEL_ID "2882460"      // Replace with your ThingSpeak Channel ID

// Callback function to handle HTTP response data
size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata) {
    printf("Latest CPU Temperature Data: %s\n", (char *)ptr);
    return size * nmemb;
}

// Function to fetch data from ThingSpeak
void fetchTemperature() {
    CURL *curl;
    CURLcode res;
    char url[256];

    snprintf(url, sizeof(url), "https://api.thingspeak.com/channels/%s/fields/1/last.txt?api_key=%s", CHANNEL_ID, READ_API_KEY);

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}

int main() {
    printf("Fetching latest CPU temperature data from ThingSpeak...\n");
    fetchTemperature();
    return 0;
}
