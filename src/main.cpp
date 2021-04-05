#include <Arduino.h>
#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFiClientSecure.h>
#include <MQTT.h>
#include <time.h>
#include <esp_wifi.h>
#include <measurements.h>
#include <config.h>
#include <wifiService.h>
#include <deepSleepService.h>
#include <timeService.h>
#include <connectToWiFiTask.h>

// DS18B20
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

RTC_DATA_ATTR Measurements measurements;
WifiService wifiService;
DeepSleepService deepSleepService;

WiFiClient net;
MQTTClient client(4096);

RTC_DATA_ATTR int bootCount = 0;
bool tryingToSendMessage = true;
signed long long lastMeasurementTime = 0;

void readSensorValue()
{
  signed long long time = TimeService::getCurrentTime();

  if (time > 100000000)
  {
    sensors.begin();
    sensors.requestTemperatures();
    float temp = sensors.getTempCByIndex(0);

    lastMeasurementTime = TimeService::getCurrentTime();
    Serial.printf("[%lu] Last measurement time: %lld\n", millis(), lastMeasurementTime);

    measurements.add(lastMeasurementTime / 1000, temp);
    measurements.log();
  }
}

bool sendData()
{
  Serial.printf("[%lu] Connecting to MQTT\n", millis());
  client.setTimeout(10000);
  client.begin(MQTT_SERVER, MQTT_PORT, net);

  bool clientConnected = client.connect(MQTT_CLIENT_ID);

  if (!clientConnected)
  {
    Serial.printf("[%lu] Connecting to MQTT failed.\n", millis());
    return false;
  }

  String mqttMessage = measurements.createMqttMessage();

  // Send MQTT message
  Serial.printf("[%lu] Sending message to MQTT: ", millis());
  Serial.println(mqttMessage);

  bool messageSent = client.publish("temperature", mqttMessage, false, 1);

  if (!messageSent)
  {
    Serial.printf("[%lu] Sending MQTT message failed\n", millis());
    client.disconnect();
    return false;
  }

  Serial.printf("[%lu] MQTT message sent successfully\n", millis());
  measurements.clear();

  return true;
}

// void onWiFiDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
// {
//   if (tryingToSendMessage)
//   {
//     WiFi.reconnect();
//   }
//   else
//   {
//     Serial.printf("[%lu] WiFi status before sleep: ", millis());
//     wifiService.logWiFiStatus();
//     deepSleepService.sleep(lastMeasurementTime, bootCount);
//   }
// }

void setup()
{
  Serial.begin(115200);
  Serial.println("");
  Serial.printf("[%lu] Boot count: %d\n", millis(), ++bootCount);

  ConnectToWiFiTask connectToWiFiTask(wifiService);

  bool shouldSyncTime = TimeService::shouldSyncTime();
  bool connectedToWiFi = false;

  if (shouldSyncTime)
  {
    connectedToWiFi = connectToWiFiTask.run(SSID, PASSWORD, 10);
    if (connectedToWiFi)
    {
      TimeService::syncTime(10);
      wifiService.disconnect();
    }
  }
  else
  {
    readSensorValue();
    if ( measurements.getCount() >= CACHE_SIZE)
    {
      connectedToWiFi = connectToWiFiTask.run(SSID, PASSWORD, 10);
      if (connectedToWiFi)
      {
        sendData();
        wifiService.disconnect();
      }
    }
  }

  Serial.printf("[%lu] WiFi status before sleep: ", millis());
  wifiService.logWiFiStatus();

  deepSleepService.sleep(lastMeasurementTime, bootCount);
}

void loop() {}