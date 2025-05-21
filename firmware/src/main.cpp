#include <Arduino.h>
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "DHT.h"

// DHT22
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
float humidity;
float temperature;
unsigned long lastDHTReadTime = millis();
constexpr long DHTReadInterval = 2000;

// Relays
#define INNER_FANS 16
#define HEATER 17
#define PELTIER 5
#define PELTIER_FAN 18

// WiFi
#ifndef WIFI_SSID
#define WIFI_SSID "SSID"
#endif
#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "PASSWORD"
#endif

// Web
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

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
    pinMode(INNER_FANS, OUTPUT);
    pinMode(HEATER, OUTPUT);
    pinMode(PELTIER, OUTPUT);
    pinMode(PELTIER_FAN, OUTPUT);

    // Logic for relay is inverted (LOW -> ON).
    digitalWrite(INNER_FANS, HIGH);
    digitalWrite(HEATER, HIGH);
    digitalWrite(PELTIER, HIGH);
    digitalWrite(PELTIER_FAN, HIGH);
}

void readDHT() {
    if (millis() - lastDHTReadTime >= DHTReadInterval) {
        humidity = dht.readHumidity();
        temperature = dht.readTemperature(); // Celsius
        lastDHTReadTime = millis();

        if (isnan(humidity) || isnan(temperature)) {
            Serial.println("Failed to read from DHT sensor!");
        }

        ws.textAll(String(humidity) + " " + String(temperature));

        Serial.print(humidity);
        Serial.print(" ");
        Serial.print(temperature);
        Serial.println("");
    }
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo *) arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        data[len] = 0;
        if (strcmp((char *) data, "toggle fan") == 0) {
            Serial.println("Toggling fan on/off...");
            ws.textAll(">>> toggling fan");
            if (digitalRead(INNER_FANS) == LOW) {
                digitalWrite(INNER_FANS, HIGH);
            } else {
                digitalWrite(INNER_FANS, LOW);
            }
        } else if (strcmp((char *) data, "toggle peltier") == 0) {
            Serial.println("Toggling peltier on/off...");
            ws.textAll(">>> toggling peltier");
            if (digitalRead(PELTIER == LOW)) {
                digitalWrite(PELTIER, HIGH);
            } else {
                digitalWrite(PELTIER, LOW);
            }
        } else if (strcmp((char *) data, "toggle heater") == 0) {
            Serial.println("Toggling heater on/off...");
            ws.textAll(">>> toggling heater");
            if (digitalRead(HEATER) == LOW) {
                digitalWrite(HEATER, HIGH);
            } else {
                digitalWrite(HEATER, LOW);
            }
        } else if (strcmp((char *) data, "toggle peltier fan") == 0) {
            Serial.println("Toggling peltier fan on/off...");
            ws.textAll(">>> toggling peltier fan");
            if (digitalRead(PELTIER_FAN) == LOW) {
                digitalWrite(PELTIER_FAN, HIGH);
            } else {
                digitalWrite(PELTIER_FAN, LOW);
            }
        }
    }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data,
             size_t len) {
    switch (type) {
        case WS_EVT_CONNECT:
            Serial.printf("WebSocket client #%u connected from %s\n", client->id(),
                          client->remoteIP().toString().c_str());
            break;
        case WS_EVT_DISCONNECT:
            Serial.printf("WebSocket client #%u disconnected\n", client->id());
            break;
        case WS_EVT_DATA:
            handleWebSocketMessage(arg, data, len);
            break;
        case WS_EVT_PONG:
        case WS_EVT_ERROR:
            break;
    }
}

void setup() {
    Serial.begin(115200);

    initWiFi();
    dht.begin();

    // Setup WebSocket
    ws.onEvent(onEvent);
    server.addHandler(&ws);

    // Setup Web Server
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(
            200,
            "text/html",
            "OK<script>"
            "let ws = new WebSocket(`ws://${window.location.host}/ws`);"
            "ws.onmessage = (event) => {console.log(event.data)};"
            "</script>"
        );
    });
    server.begin();
}

void loop() {
    readDHT();
    ws.cleanupClients();
}
