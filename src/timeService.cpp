#include <Arduino.h>
#include <time.h>
#include <timeService.h>
#include <config.h>
#include <esp_wifi.h>

bool TimeService::shouldSyncTime()
{
  return TimeService::getCurrentTime() < 100000000;
}

void TimeService::myDelay(unsigned long ms, bool log)
{
  if (log)
    Serial.printf("[%lu] Waiting for %lums\n", millis(), ms);

  unsigned long start = millis();
  while (millis() - start < ms)
    ;
}

__int64_t TimeService::getCurrentTime()
{
  struct timeval tv_now;
  gettimeofday(&tv_now, NULL);
  return (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;
}

bool TimeService::syncTime(int timeout)
{
  Serial.printf("[%lu] Sending request to NTP server\n", millis());
  configTime(GMT_OFFSET, DAYLIGHT_OFFSET, NTP_SERVER);

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo, timeout * 1000))
  {
    Serial.printf("[%lu] Failed to sync time with NTP server\n", millis());
    return false;
  }

  Serial.printf("[%lu] Current time : %lld\n", millis(), getCurrentTime());
  return true;
}