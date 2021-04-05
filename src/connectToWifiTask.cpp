#include <Arduino.h>
#include <WiFi.h>
#include <connectToWiFiTask.h>
#include <wifiService.h>
#include <timeService.h>

ConnectToWiFiTask::ConnectToWiFiTask(WifiService wifiService) {
    this->wifiService = wifiService;
}

bool ConnectToWiFiTask::run(const char* ssid, const  char* password, int timeout) {
    unsigned long connectingStartTime;
    Serial.printf("[%lu] Connecting to WiFi\n", connectingStartTime = millis());

    WiFi.mode(WIFI_STA);
    wifiService.subscribeLogEvent();
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED && millis() - connectingStartTime < timeout * 1000)
        TimeService::myDelay(10, false);

    if (WiFi.status() != WL_CONNECTED) {
        Serial.printf("[%lu] Connecting to WiFi failed with status: ", millis());
        wifiService.logWiFiStatus();
        WiFi.disconnect();
        return false;
    }

    Serial.printf("[%lu] Successfully connected to WiFi\n", millis());

    return true;
}