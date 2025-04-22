#include <Arduino.h>
#include "WiFi.h"

// WiFi
#ifndef WIFI_SSID
#define WIFI_SSID "SSID"
#endif
#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "PASSWORD"
#endif

void initWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to WiFi");

    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.print("\nConnected: ");
    Serial.println(WiFi.localIP());
}

void setup() {
    Serial.begin(115200);

    initWiFi();
}

void loop() {
// write your code here
}
