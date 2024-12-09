#include "WIFIManager.h"
#include <WiFi.h>

#define WIFI_SSID "JD"
#define WIFI_PASSWORD "storm54321"

void setupWiFi() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(300);
        Serial.print(".");
    }
    Serial.println("Connected to Wi-Fi");
}

void maintainConnection() {
    if (WiFi.status() != WL_CONNECTED) {
        setupWiFi();
    }
}
