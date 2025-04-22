#include <Arduino.h>
#include "WiFi.h"
#include "DHT.h"

// DHT22
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
float humidity;
float temperature;
unsigned long lastDHTReadTime = millis();
constexpr long DHTReadInterval = 2000;

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

void readDHT() {
    if (millis() - lastDHTReadTime >= DHTReadInterval) {
        humidity = dht.readHumidity();
        temperature = dht.readTemperature(); // Celsius
        lastDHTReadTime = millis();

        if (isnan(humidity) || isnan(temperature)) {
            Serial.println("Failed to read from DHT sensor!");
        }

        Serial.print(humidity);
        Serial.print(" ");
        Serial.print(temperature);
        Serial.println("");
    }

}

void setup() {
    Serial.begin(115200);

    initWiFi();
    dht.begin();
}

void loop() {
    readDHT();
}
