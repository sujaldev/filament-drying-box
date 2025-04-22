#include <Arduino.h>
#include "WiFi.h"
#include "DHT.h"

// DHT22
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
float humidity;
float temperature;

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
    dht.begin();
}

void loop() {
    delay(2000);

    humidity = dht.readHumidity();
    temperature = dht.readTemperature(); // Celcius

    if (isnan(humidity) || isnan(temperature)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }

    Serial.print(humidity);
    Serial.print(" ");
    Serial.print(temperature);
    Serial.println("");
}
