#include <WiFi.h>

class DeepSleepService {
    public:
        void sleep(signed long long lastMeasurementTime, int bootCount);
        void logWakeUpCause(esp_sleep_wakeup_cause_t wakeup_cause);
};