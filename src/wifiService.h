#include <WiFi.h>

#ifndef WIFI_SERVICE_H
#define WIFI_SERVICE_H

class WifiService {
    public:
        void subscribeLogEvent();
        void logWiFiStatus();
        void logWakeUpCause(esp_sleep_wakeup_cause_t wakeup_cause);
        void disconnect();
        static void logWiFiEvent(WiFiEvent_t event);
};

#endif