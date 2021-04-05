#include <WiFi.h>
#include <deepSleepService.h>
#include <config.h>
#include <timeService.h>

void DeepSleepService::sleep(signed long long lastMeasurementTime, int bootCount) {
  uint64_t sleepTime = TIME_TO_SLEEP * 1000000;

  if (lastMeasurementTime > 100000000 && bootCount > 1) {
    uint64_t currentTime = TimeService::getCurrentTime();
    uint64_t timeFromLastMeasurement = currentTime > lastMeasurementTime ? currentTime - lastMeasurementTime : 0;

    sleepTime = sleepTime > timeFromLastMeasurement ? sleepTime - timeFromLastMeasurement : 0;
    sleepTime -= sleepTime > 858000 ? 859000 : 0;
  } else {
    uint64_t timeFromWakeUp = micros();
    sleepTime = sleepTime > timeFromWakeUp ? sleepTime - timeFromWakeUp : 0;
  }

  // Max sleep time 1 hour
  if (sleepTime > 3600000000)
    sleepTime = 3600000000;

  if (sleepTime < 1000)
    sleepTime = 1000;

  Serial.printf("[%lu] Going to deep sleep for %llu ms\n", millis(), sleepTime / 1000);
  Serial.flush();

  esp_sleep_enable_timer_wakeup(sleepTime);
  esp_deep_sleep_start();
}

void DeepSleepService::logWakeUpCause(esp_sleep_wakeup_cause_t wakeup_cause) {
  switch(wakeup_cause)
  {
    case ESP_SLEEP_WAKEUP_EXT0:
      Serial.println("Wakeup caused by external signal using RTC_IO");
      break;
    case ESP_SLEEP_WAKEUP_EXT1:
      Serial.println("Wakeup caused by external signal using RTC_CNTL");
      break;
    case ESP_SLEEP_WAKEUP_TIMER:
      Serial.println("Wakeup caused by timer");
      break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
      Serial.println("Wakeup caused by touchpad");
      break;
    case ESP_SLEEP_WAKEUP_ULP:
      Serial.println("Wakeup caused by ULP program");
      break;
    default:
      Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_cause);
      break;
  }
}